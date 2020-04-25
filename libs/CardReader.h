#pragma once

class CCardReader
{
public:
	CCardReader();
	virtual ~CCardReader();

	LONG	Connect(DWORD dwPort, DWORD dwBaudRate, BYTE bAddress = 0);
	LONG	Disconnect();

	LONG	ExecCommand(BYTE bCMDByte1, BYTE bCMDByte2, PBYTE pbCMPBuff, USHORT cbCMPLength, PBYTE pbRDTBuff, PDWORD pcbRDTLength, PCRSTATUS pStatus = NULL);
	LONG	ExecApduCommand(BYTE bCMDByte1, BYTE bCMDByte2, BYTE bCLA, BYTE bINS, BYTE bP1, BYTE bP2, BYTE bLength, PBYTE pbSendData, PBYTE pbRecvData, PDWORD pcbRecvLength);

	static	CCardReader* FromHandle(READERHANDLE hReader);

	inline	BYTE GetCRAddress() { return m_Address; }
	inline  void SetCRAddress(BYTE bAddress) { m_Address = bAddress; }

private:
	LONG	TransmissionControl(PVOID pvParam);
	LONG	ProcessResponseData(BOOL bHasSW, PBYTE pbRecvData, PDWORD pcbRecvLength, PCRSTATUS pStatus);

	inline	LONG SendCommand();
	inline  LONG SendENQ();
	inline	LONG WaitForACK();
	inline	LONG WaitForResponse();
	inline	LONG RecvResponse();

	BOOL	SendData(PBYTE pbBuffer, DWORD dwBytesToWrite, PBOOL pbTimeOut);
	BOOL	RecvData(PBYTE pbBuffer, DWORD dwBytesToRead, PBOOL pbTimeOut);

	LONG	InitialPort(DWORD dwPort, DWORD dwBaudRate);
	LONG	MakeMessage(BYTE bCMDByte1, BYTE bCMDByte2, PBYTE pbCMPBuff, USHORT cbCMPLength);
	LONG	MakeMessage(BYTE bCMDByte1, BYTE bCMDByte2, BYTE bCLA, BYTE bINS, BYTE bP1, BYTE bP2, BYTE bLength, PBYTE pbSendData);

	BOOL	WaitForBytesAvailable(DWORD dwMilliseconds);

private:
	enum { MAX_MSG_LENGTH = 1024 };

	CRITICAL_SECTION m_csLock;

	HANDLE	m_hDevice;

	BYTE	m_MsgBuff[MAX_MSG_LENGTH];
	UINT	m_MsgLength;
	BYTE	m_Address;
};

