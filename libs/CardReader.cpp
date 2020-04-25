#include "StdAfx.h"
#include "F3API.h"
#include "CardReader.h"
#include "ObjectManager.h"

// 传输控制字符
//
#define ACK	0x06
#define NAK	0x15
#define ENQ	0x05
#define	EOT	0x04
#define STX	0xF2
#define ETX	0x03
#define CMT	'C'

#define PURGE_FLAGS  (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR)


static BYTE GetBCC(PBYTE pbBuff, DWORD cbLength)
{
	BYTE bcc = 0;
	for (USHORT i = 0; i < cbLength - 1; i++)
		bcc ^= pbBuff[i];
	
	return bcc;
}

static LONG GetResult(BYTE e0, BYTE e1)
{
#define ERRMAP(_e0, _e1, _res) \
	if (e0 == _e0 && e1 == _e1)	return _res;

#include "errmap.h"

	return F3_E_UNKNOWN_ERROR;
}



static CObjectManager _objmgr;


CCardReader::CCardReader()
	:  m_hDevice(INVALID_HANDLE_VALUE)
{
// 初始化临界区对象
	InitializeCriticalSection(&m_csLock);
}


CCardReader::~CCardReader()
{
	// 释放临界区对象
	DeleteCriticalSection(&m_csLock);
}


LONG	CCardReader::Connect(DWORD dwPort, DWORD dwBaudRate, BYTE bAddress )
{
	LONG lResult = InitialPort(dwPort, dwBaudRate);
	if ( lResult != 0 )
		return lResult;


	m_Address = bAddress;

	if (! _objmgr.Add(this))
	{
		Disconnect();
		return F3_E_NO_MEMORY;
	}

	return 0;
}


LONG	CCardReader::Disconnect()
{
	LONG lResult = 0;

	EnterCriticalSection(&m_csLock);

	if ( m_hDevice != INVALID_HANDLE_VALUE)
	{
		if ( !CloseHandle(m_hDevice))
			lResult = F3_E_UNKNOWN_ERROR;

		m_hDevice = INVALID_HANDLE_VALUE;

		_objmgr.Remove(this);
	}

	LeaveCriticalSection(&m_csLock);

	return lResult;
}


LONG	CCardReader::ExecCommand(BYTE bCMDByte1, BYTE bCMDByte2, PBYTE pbCMPBuff, USHORT cbCMPLength, PBYTE pbRDTBuff, PDWORD pcbRDTLength, PCRSTATUS pStatus)
{
	LONG lResult = 0;

	EnterCriticalSection(&m_csLock);

	lResult = MakeMessage(bCMDByte1, bCMDByte2, pbCMPBuff, cbCMPLength);
	if (lResult != 0)
		goto L_EXIT;

	lResult = TransmissionControl(NULL);
	if (lResult != 0)
		goto L_EXIT;

	lResult = ProcessResponseData(FALSE, pbRDTBuff, pcbRDTLength, pStatus);

L_EXIT:
	LeaveCriticalSection(&m_csLock);

	return lResult;
}


LONG	CCardReader::ExecApduCommand(BYTE bCMDByte1, BYTE bCMDByte2, BYTE bCLA, BYTE bINS, BYTE bP1, BYTE bP2, BYTE bLength, PBYTE pbSendData, PBYTE pbRecvData, PDWORD pcbRecvLength)
{
	LONG lResult = 0;

	EnterCriticalSection(&m_csLock);

	lResult = MakeMessage(bCMDByte1, bCMDByte2, bCLA, bINS, bP1, bP2, bLength, pbSendData);
	if (lResult != 0)
		goto L_EXIT;

	lResult = TransmissionControl(NULL);
	if (lResult != 0)
		goto L_EXIT;

	lResult = ProcessResponseData(TRUE, pbRecvData, pcbRecvLength, NULL);

L_EXIT:
	LeaveCriticalSection(&m_csLock);

	return lResult;
}


LONG CCardReader::TransmissionControl(PVOID pvParam)
{
	LONG lResult = 0;

	if (! PurgeComm( m_hDevice, PURGE_FLAGS))
		return F3_E_UNKNOWN_ERROR;

	lResult = SendCommand();
	if (lResult != 0)
		return lResult;

	lResult = WaitForACK();
	if (lResult != 0)
		return lResult;

	lResult = WaitForResponse();
	if ( lResult != 0)
		return lResult;

	lResult = RecvResponse();

	return lResult;
}


LONG	CCardReader::ProcessResponseData(BOOL bHasSW, PBYTE pbRecvData, PDWORD pcbRecvLength, PCRSTATUS pStatus)
{
	UINT RDTLen = m_MsgLength - 12;
	
	if (m_MsgBuff[4] == 0x4E)
		return GetResult(m_MsgBuff[7], m_MsgBuff[8]);
	else if (m_MsgBuff[4] != 'P' )
		return F3_E_COMM_ERROR;
		
	if (pStatus != NULL )
	{
		// 检测 ST0，ST1，ST2 的值是否是通信协议上定义的值/
		//
		if ( !( m_MsgBuff[7] >= 0x30 && m_MsgBuff[7] <= 0x32))
			return F3_E_COMM_ERROR;
		if (! (m_MsgBuff[8] >= 0x30 && m_MsgBuff[8] <= 0x32))
			return F3_E_COMM_ERROR;
		if (! (m_MsgBuff[9] >= 0x30 && m_MsgBuff[9] <= 0x31))
			return F3_E_COMM_ERROR;

		pStatus->bLaneStatus = m_MsgBuff[7];
		pStatus->bCardBoxStatus = m_MsgBuff[8];
		pStatus->fCaptureBoxFull = m_MsgBuff[9] == 0x31;
	}

	if (bHasSW)
	{
		RDTLen -= 2;

		LONG SW1 = m_MsgBuff[m_MsgLength - 4];
		LONG SW2 = m_MsgBuff[m_MsgLength - 3];

		if (SW1 == 0x90 && SW2 == 0)
			goto L2;
		else if (SW1 == 0x6F && SW2 == 0)
			return F3_E_COMMAND_FAILURE; // 操作失败
		else if (SW1 == 0x6F && SW2 == 0x1)
			return F3_E_DISAGREEMENT_OF_VC; // 密码不正确
		else if (SW1 == 0x6F && SW2 == 0x2)
			return F3_E_CARD_LOCKED; // 卡片锁死
		else if (SW1 == 0x6B && SW2 == 0)
			return F3_E_ADDRESS_OVERFLOW; // 地址溢出
		else if (SW1 == 0x67 && SW2 == 0)
			return F3_E_LENGTH_OVERFLOW; // 长度溢出
		else
			return F3_E_COMMAND_FAILURE;
	}

	L2:

	if (pbRecvData != NULL
		&& pcbRecvLength != NULL)
	{
		if (*pcbRecvLength < RDTLen)
			return F3_E_BUFFER_TOO_SMALL;

		*pcbRecvLength = RDTLen;
		memcpy( pbRecvData, &m_MsgBuff[10], RDTLen);
	}

	return 0;
}


LONG CCardReader::SendCommand()
{
	BOOL bTimeOut;

	if (! SendData(m_MsgBuff, m_MsgLength, &bTimeOut))
	{
			if (bTimeOut)
				return F3_E_COMM_TIMEOUT;
			else
				return F3_E_UNKNOWN_ERROR;
	}
	
	return 0;
}


LONG CCardReader::SendENQ()
{
	BOOL bTimeOut;

	BYTE ENQBuff[] = { ENQ };
	if (! SendData(ENQBuff, 1, &bTimeOut))
		{
			if (bTimeOut)
				return F3_E_COMM_TIMEOUT;
			else
				return F3_E_UNKNOWN_ERROR;
		}
	
	return 0;
}


LONG CCardReader::WaitForACK()
{
	if (!  WaitForBytesAvailable( 500))
		return F3_E_COMM_TIMEOUT;

	BOOL bTimeOut;
	BYTE ACKBuff[1];
	while ( TRUE )
	{
		bTimeOut = FALSE;
		if (! RecvData(ACKBuff, 1, &bTimeOut))
		{
			if (bTimeOut)
				return F3_E_COMM_TIMEOUT;
			else
				return F3_E_UNKNOWN_ERROR;
		}

		if (ACKBuff[0] == ACK )
			return 0;
	}
}


LONG CCardReader::WaitForResponse()
{
	if (! WaitForBytesAvailable( 31000 ))
		return F3_E_COMM_TIMEOUT;

	BOOL bTimeOut;

	while ( TRUE )
	{
		if (! RecvData(m_MsgBuff, 1, &bTimeOut))
		{
			if (bTimeOut)
				return F3_E_COMM_TIMEOUT;
			else
				return F3_E_UNKNOWN_ERROR;
		}

		if ( m_MsgBuff[0] == STX)
			return 0;
	}
}


LONG CCardReader::RecvResponse()
{
	BOOL bTimeOut;
	
	if (! RecvData(&m_MsgBuff[1], 3, &bTimeOut))
	{
		if (bTimeOut)
			return F3_E_COMM_TIMEOUT;
		else
			return F3_E_UNKNOWN_ERROR;
	}

	UINT nRemainderBytes = ((m_MsgBuff[2] << 8) | m_MsgBuff[3]) + 2; // +2: for [ETX][BCC]
	if ( nRemainderBytes > MAX_MSG_LENGTH - 4)
		return F3_E_MESSAGE_TOO_LONG;

	if (! RecvData(&m_MsgBuff[4], nRemainderBytes, &bTimeOut))
	{
		if (bTimeOut)
			return F3_E_COMM_TIMEOUT;
		else
			return F3_E_UNKNOWN_ERROR;
	}

	m_MsgLength = nRemainderBytes + 4;

	if ( m_MsgLength < 11 )	// 最小的响应消息长度为11
		return F3_E_COMM_ERROR;

	// 检查异或校验
	BYTE bcc = GetBCC(m_MsgBuff, m_MsgLength);
	if (m_MsgBuff[m_MsgLength - 1] != bcc)
		return F3_E_COMM_ERROR;

	return 0;
}


BOOL	CCardReader::SendData(PBYTE pbBuffer, DWORD dwBytesToWrite, PBOOL pbTimeOut)
{
	DWORD	dwBytesWritten;
	BOOL	fRes;
	OVERLAPPED oWrite = { 0 };

	oWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oWrite.hEvent == NULL)
		return FALSE;
	
	*pbTimeOut = FALSE;
	if (! WriteFile(m_hDevice, pbBuffer, dwBytesToWrite, &dwBytesWritten, &oWrite))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			switch (WaitForSingleObject(oWrite.hEvent, INFINITE))
			{
			case WAIT_OBJECT_0:
				if (GetOverlappedResult(m_hDevice, &oWrite, &dwBytesWritten, FALSE))
				{
					if (dwBytesWritten != dwBytesToWrite)
					{
						fRes = FALSE;
						*pbTimeOut = TRUE;
					}
					else
						fRes = TRUE;
				}
				else
					fRes = FALSE;
				break;
			case WAIT_TIMEOUT:
				fRes = FALSE;
				*pbTimeOut = TRUE;
				break;
			default:
				fRes = FALSE;
				break;
			}
		}
		else
		{
			// WriteFile 失败但不延时.
			fRes = FALSE;
			SetLastError(dwError);
		}
	}
	else 
	{
		if (dwBytesWritten != dwBytesToWrite)
			fRes = FALSE;
		else
			fRes = TRUE;
	}
	
	CloseHandle(oWrite.hEvent);
	
	return fRes;
}


BOOL	CCardReader::RecvData(PBYTE pbBuffer, DWORD dwBytesToRead, PBOOL pbTimeOut)
{
	DWORD dwBytesRead;
	OVERLAPPED oRead = { 0 };
	oRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (oRead.hEvent == NULL)
		return FALSE;

	BOOL fRes;
	*pbTimeOut = FALSE;

	if (! ReadFile(m_hDevice, pbBuffer, dwBytesToRead, &dwBytesRead, &oRead))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_IO_PENDING)
		{
			switch (WaitForSingleObject(oRead.hEvent, INFINITE))
			{
			case WAIT_OBJECT_0:
				if (GetOverlappedResult(m_hDevice, &oRead, &dwBytesRead, FALSE))
				{
					if (dwBytesRead != dwBytesToRead)
					{
						fRes = FALSE;
						*pbTimeOut = TRUE;
					}
					else
						fRes = TRUE;
				}
				else
					fRes = FALSE;
				break;	
			case WAIT_TIMEOUT:
				fRes = FALSE;
				*pbTimeOut = TRUE;
				break;	
			default:
				fRes = FALSE;
				break;
			}
		}
		else
		{
			// ReadFile 失败但不延时
			fRes = FALSE;
			SetLastError(dwError);
		}
	}
	else
	{
		if (dwBytesRead != dwBytesToRead)
			fRes = FALSE;
		else
			fRes = TRUE;
	}
	
	CloseHandle(oRead.hEvent);

	return fRes;
}


LONG	CCardReader::InitialPort(DWORD dwPort, DWORD dwBaudRate)
{
	char szPortName[32];
	sprintf_s(szPortName, sizeof(szPortName),
		"\\\\.\\COM%d", dwPort);

	HANDLE hDevice = CreateFileA(szPortName, 
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			NULL, 
			OPEN_EXISTING, 
			FILE_FLAG_OVERLAPPED,	// 重叠 I/O
			NULL);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		switch ( GetLastError() )
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_ACCESS_DENIED:
			return F3_E_PORT_UNAVAILABLE;
		default:
			return F3_E_UNKNOWN_ERROR;
		}
	}

	DCB dcb;
	if (! GetCommState(hDevice, &dcb))
		goto CLEARUP;

	dcb.BaudRate = dwBaudRate;
	dcb.Parity = NOPARITY;
	dcb.ByteSize = DATABITS_8;
	dcb.StopBits = ONESTOPBIT;

	if (! SetCommState(hDevice, &dcb))
		goto CLEARUP;

	if (! SetupComm(hDevice, 4096, 4096))
		goto CLEARUP;

	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutMultiplier = 50;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 50;
	timeouts.WriteTotalTimeoutConstant = 100;

	if (! SetCommTimeouts(hDevice, &timeouts))
		goto CLEARUP;

	m_hDevice = hDevice;

	return 0;

CLEARUP:
	CloseHandle(hDevice);
	return F3_E_UNKNOWN_ERROR;;
}


LONG	CCardReader::MakeMessage(BYTE bCMDByte1, BYTE bCMDByte2, PBYTE pbCMPBuff, USHORT cbCMPLength)
{
	USHORT nTextLength = 3;	// CMT + CM + PM
	if (pbCMPBuff != NULL
		&& cbCMPLength > 0)
		nTextLength += cbCMPLength;

	if (nTextLength > MAX_MSG_LENGTH - 6)
		return F3_E_MESSAGE_TOO_LONG;

	m_MsgLength = 0;
	m_MsgBuff[m_MsgLength++] = STX;
	m_MsgBuff[m_MsgLength++] = m_Address;
	m_MsgBuff[m_MsgLength++] = (BYTE) HIBYTE(nTextLength);
	m_MsgBuff[m_MsgLength++] = (BYTE) LOBYTE(nTextLength);
	m_MsgBuff[m_MsgLength++] = CMT;
	m_MsgBuff[m_MsgLength++] = bCMDByte1;
	m_MsgBuff[m_MsgLength++] = bCMDByte2;

	if (nTextLength > 3)
	{
		memcpy(&m_MsgBuff[m_MsgLength], pbCMPBuff, cbCMPLength);
		m_MsgLength += cbCMPLength;
	}

	m_MsgBuff[m_MsgLength++] = ETX;
	m_MsgLength++;	// + BCC
	m_MsgBuff[m_MsgLength - 1] = GetBCC(m_MsgBuff, m_MsgLength);

	return 0;
}


LONG	CCardReader::MakeMessage(BYTE bCMDByte1, BYTE bCMDByte2, BYTE bCLA, BYTE bINS, BYTE bP1, BYTE bP2, BYTE bLength, PBYTE pbSendData)
{
	BOOL bWriteSec = FALSE;
	if ( bCMDByte1 == 0x60 && bCMDByte2 == 0x33 && bCLA == 0x0 && bINS == 0xD1 )
		bWriteSec = TRUE;


	USHORT nTextLength = 8;	// CMT + CM + PM
	if (pbSendData != NULL
		&& bLength > 0)
		nTextLength += bLength;

	if (nTextLength == 8
		&& bLength == 0)
		nTextLength -= 1;

	if (nTextLength > MAX_MSG_LENGTH - 6)
		return F3_E_MESSAGE_TOO_LONG;

	m_MsgLength = 0;
	m_MsgBuff[m_MsgLength++] = STX;
	m_MsgBuff[m_MsgLength++] = m_Address;
	m_MsgBuff[m_MsgLength++] = (BYTE) HIBYTE(nTextLength);
	m_MsgBuff[m_MsgLength++] = (BYTE) LOBYTE(nTextLength);
	m_MsgBuff[m_MsgLength++] = CMT;
	m_MsgBuff[m_MsgLength++] = bCMDByte1;
	m_MsgBuff[m_MsgLength++] = bCMDByte2;

	m_MsgBuff[m_MsgLength++] = bCLA;
	m_MsgBuff[m_MsgLength++] = bINS;
	m_MsgBuff[m_MsgLength++] = bP1;
	m_MsgBuff[m_MsgLength++] = bP2;
	
	if (bLength > 0)
	{
		if ( bWriteSec )
			m_MsgBuff[m_MsgLength++] = 1;
		else
			m_MsgBuff[m_MsgLength++] = bLength;
	}

	if (nTextLength > 8)
	{
		memcpy(&m_MsgBuff[m_MsgLength], pbSendData, bLength);
		m_MsgLength += bLength;
	}

	m_MsgBuff[m_MsgLength++] = ETX;
	m_MsgLength++;	// + BCC
	m_MsgBuff[m_MsgLength - 1] = GetBCC(m_MsgBuff, m_MsgLength);

	return 0;
}


BOOL	CCardReader::WaitForBytesAvailable(DWORD dwMilliseconds)
{
	COMSTAT lResult;
	DWORD dwError;
	DWORD dwStartTime = GetTickCount();
	DWORD dwCurrTime;
	DWORD dwElapsedTime = 0;

	while ( TRUE )
	{
		ClearCommError(m_hDevice, &dwError, &lResult);
		if (lResult.cbInQue > 0)
			break;

		dwCurrTime = GetTickCount();
		if ( dwCurrTime >= dwStartTime )
		{
			dwElapsedTime += dwCurrTime - dwStartTime;
		}
		else
		{
			dwElapsedTime += 0xFFFFFFFF - dwStartTime;
			dwElapsedTime += dwCurrTime;
		}

		dwStartTime = dwCurrTime;

		if ( dwElapsedTime >= dwMilliseconds)
			return FALSE;

		Sleep(10);
	}

	return TRUE;
}


CCardReader* CCardReader::FromHandle(READERHANDLE hReader)
{
	if ( _objmgr.IsExist( (void*) hReader ))
		return (CCardReader*) hReader;

	return NULL;
}