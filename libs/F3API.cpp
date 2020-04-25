// F3API.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "F3API.h"
#include "CardReader.h"

#define CHECKLENGTH(lResult, expr) \
	{ if (lResult == 0) \
	{ if ( !(expr)) return F3_E_COMM_ERROR; } \
	else return lResult; } 


static LONG ExecCommand(
	IN	READERHANDLE hReader,
	IN	BYTE	bCMDByte1,
	IN	BYTE	bCMDByte2,
	IN	PBYTE	pbCMPBuff,
	IN	USHORT	cbCMPLength,
	OUT	PBYTE	pbRDTBuff, 
	IN OUT	PDWORD	pcbRDTLength,
	IN	PCRSTATUS pStatus = NULL
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return F3_E_INVALID_HANDLE;

	return pReader->ExecCommand(bCMDByte1, bCMDByte2, pbCMPBuff, cbCMPLength, pbRDTBuff, pcbRDTLength, pStatus);
}

static LONG ExecApduCommand(
	IN	READERHANDLE hReader,
	IN	BYTE	bCMDByte1, 
	IN	BYTE	bCMDByte2, 
	IN	BYTE	bCLA,
	IN	BYTE	bINS, 
	IN	BYTE	bP1,
	IN	BYTE	bP2, 
	IN	BYTE	bLength, 
	IN	PBYTE	pbSendData,
	IN	PBYTE	pbRecvData, 
	IN	PDWORD	pcbRecvLength
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return F3_E_INVALID_HANDLE;

	return pReader->ExecApduCommand(bCMDByte1, bCMDByte2, bCLA, bINS, bP1, bP2, bLength, pbSendData, pbRecvData, pcbRecvLength);
}

static LONG ExecCommand(
	IN	READERHANDLE hReader,
	IN	BYTE	bCMDByte1,
	IN	BYTE	bCMDByte2,
	OUT	PBYTE	pbRDTBuff, 
	IN OUT	PDWORD	pcbRDTLength,
	IN	PCRSTATUS pStatus = NULL
	)
{
	return ExecCommand(hReader, bCMDByte1, bCMDByte2, NULL, 0, pbRDTBuff, pcbRDTLength, pStatus);
}


static LONG ExecCommand(
	IN	READERHANDLE	hReader,
	IN	BYTE	bCMDByte1,
	IN	BYTE	bCMDByte2,
	IN	PCRSTATUS pStatus = NULL
	)
{
	return ExecCommand(hReader, bCMDByte1, bCMDByte2, NULL, 0,  NULL, NULL, pStatus);
}


LONG WINAPI F3_Connect(
	IN	DWORD	dwPort,
	IN	DWORD	dwSpeed,
	IN	BYTE	bCRAddr,
	OUT	LPREADERHANDLE lphReader
	)
{
	if (lphReader == NULL)
		return F3_E_INVALID_PARAMETER;

	CCardReader *pReader = new CCardReader();
	if (pReader == NULL)
		return F3_E_NO_MEMORY;

	LONG lResult = pReader->Connect(dwPort, dwSpeed, bCRAddr);
	if (lResult == 0)
	{
		if ( pReader->ExecCommand(0x30, 0x33, NULL, 0, NULL, NULL) != 0)
		{
			pReader->Disconnect();
			delete pReader;
			return F3_E_DEV_NOT_RECOGNIZED;
		}

		*lphReader = (READERHANDLE) pReader;
	}
	else
	{
		delete pReader;
	}

	return lResult;
}


LONG WINAPI F3_Disconnect(
	IN	READERHANDLE hReader
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return F3_E_INVALID_HANDLE; 

	return pReader->Disconnect();
}


LONG
WINAPI
F3_SetCRAddress
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bCRAddr
	)
{
	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return F3_E_INVALID_HANDLE; 

	pReader->SetCRAddress(bCRAddr);
	return 0;
}


LONG WINAPI F3_GetCRStatus(
	IN	READERHANDLE hReader,
	OUT	PCRSTATUS lpStatus
	)
{
	if (lpStatus == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x31, 0x30, lpStatus);
}


LONG WINAPI F3_GetSenserDetail(
	IN	READERHANDLE hReader,
	OUT	BYTE	(&bStatus)[NUM_SENSORS]
	)
{
	DWORD Length = sizeof(bStatus);
	LONG lResult = ExecCommand(hReader, 0x31, 0x31, bStatus, &Length);
	CHECKLENGTH(lResult, Length == NUM_SENSORS);
	return 0;
}



LONG WINAPI F3_GetCRAddress(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCRAddr
	)
{
	if (pbCRAddr == NULL)
		return F3_E_INVALID_PARAMETER;

	CCardReader *pReader = CCardReader::FromHandle(hReader);
	if ( pReader == NULL )
		return F3_E_INVALID_HANDLE; 

	*pbCRAddr = pReader->GetCRAddress();

	return 0;
}


//LONG WINAPI F3_GetSerialNumber(
//	IN	READERHANDLE hReader,
//	OUT	LPSTR	pszSNBuff,
//	IN OUT	PDWORD	pcbSNLength
//	)
//{
//	BYTE RDTBuff[19];
//	DWORD RDTLength = sizeof(RDTBuff);
//
//	LONG lResult = ExecCommand(hReader, 0xA2, 0x30, RDTBuff, &RDTLength);
//	CHECKLENGTH(lResult, RDTLength > 1);
//
//	if (*pcbSNLength < RDTLength - 1)
//		return F3_E_BUFFER_TOO_SMALL;
//
//	*pcbSNLength = RDTLength - 1;
//	memcpy(pszSNBuff, &RDTBuff[1], *pcbSNLength);
//
//	return lResult;
//}
//
//
//LONG WINAPI F3_GetVersionInfo(
//	IN	READERHANDLE hReader,
//	IN	BYTE	bVerType,
//	OUT	LPSTR	pszVerBuff,
//	IN OUT	PDWORD	pcbVerLength
//	)
//{
//	return ExecCommand(hReader, 0xA4, bVerType, (PBYTE) pszVerBuff, pcbVerLength);
//}
//
//LONG WINAPI F3_GetCaptureCounter(
//	IN	READERHANDLE hReader,
//	OUT	PWORD	pwCounter
//	)
//{
//	BYTE RDTBuff[3];
//	DWORD RDTLength = sizeof(RDTBuff);
//
//	LONG lResult = ExecCommand(hReader, 0xA5, 0x30, RDTBuff, &RDTLength);
//	CHECKLENGTH(lResult, RDTLength == 3);
//
//	*pwCounter = ((RDTBuff[0] - 0x30) * 100) + ((RDTBuff[1] - 0x30) * 10) + (RDTBuff[0] - 0x30);
//
//	return lResult;
//}
//
//LONG WINAPI F3_SetCaptureCounter(
//	IN	READERHANDLE hReader,
//	IN	WORD	wCounter
//	)
//{
//	BYTE CmdData[3];
//	CmdData[0] = (wCounter / 100) + 0x30;
//	CmdData[1] = (wCounter / 10) % 10 + 0x30;
//	CmdData[2] = wCounter % 10 + 0x30;
//
//	return ExecCommand(hReader, 0xA5, 0x31, CmdData, sizeof(CmdData), NULL, NULL, NULL);
//}



LONG WINAPI F3_Initialize(
	IN	READERHANDLE hReader,
	IN	BYTE	bMode,
	IN	BOOL	fEnableCounter,
	OUT	PSTR	pszRevBuff,
	IN OUT	PDWORD	pcbRevLength
	)
{
	if (pszRevBuff != NULL
		&& pcbRevLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x30, fEnableCounter ? bMode + 4 : bMode, (PBYTE) pszRevBuff, pcbRevLength);
}



LONG WINAPI F3_MoveCard(
	IN	READERHANDLE hReader,
	IN	BYTE	bMode
	)
{
	return ExecCommand(hReader, 0x32, bMode);
}


LONG WINAPI F3_PermitInsertion(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x33, 0x30);
}


LONG WINAPI F3_DenieInsertion(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x33, 0x31);
}


LONG WINAPI F3_DetectIccType(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCardType
	)
{
	if (pbCardType == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[2];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x50, 0x30, Buff, &Length);
	CHECKLENGTH(lResult, Length == 2);

	*pbCardType = ICCTYPE_UNKNOWN;

	switch (Buff[0])
	{
	case '1':
		if (Buff[1] == '0')
			*pbCardType = ICCTYPE_TO_CPU;
		else if (Buff[1] == '1')
			*pbCardType = ICCTYPE_T1_CPU;
		break;
	case '2':
		if (Buff[1] == '0')
			*pbCardType = ICCTYPE_SLE4442;
		else
			*pbCardType = ICCTYPE_SLE4428;
		break;
	case '3':
		switch (Buff[1])
		{
		case '0': *pbCardType = ICCTYPE_AT24C01; break;
		case '1': *pbCardType = ICCTYPE_AT24C02; break;
		case '2': *pbCardType = ICCTYPE_AT24C04; break;
		case '3': *pbCardType = ICCTYPE_AT24C08; break;
		case '4': *pbCardType = ICCTYPE_AT24C16; break;
		case '5': *pbCardType = ICCTYPE_AT24C32; break;
		case '6': *pbCardType = ICCTYPE_AT24C64; break;
		case '7': *pbCardType = ICCTYPE_AT24C128; break;
		case '8': *pbCardType = ICCTYPE_AT24C256; break;
		}
		break;
	}

	return lResult;
}


LONG WINAPI F3_DetectRfcType(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCardType
	)
{
	if (pbCardType == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[2];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x50, 0x31, Buff, &Length);
	CHECKLENGTH(lResult, Length == 2);

	*pbCardType = RFCTYPE_UNKNOWN;

	switch (Buff[0])
	{
	case '1':
		if (Buff[1] == '0')
			*pbCardType = RFCTYPE_MIFARE_S50;
		else if (Buff[1] == '1')
			*pbCardType = RFCTYPE_MIFARE_S70;
		else if (Buff[1] == '2')
			*pbCardType = RFCTYPE_MIFARE_UL;
		break;
	case '2':
		if (Buff[1] == '0')
			*pbCardType = RFCTYPE_TYPEA_CPU;
		break;
	case '3':
		if (Buff[1] == '0')
			*pbCardType = RFCTYPE_TYPEB_CPU;
		break;
	}

	return lResult;
}


static LONG IccReset(
	IN	READERHANDLE hReader,
	IN	BYTE	bCMDByte1,
	IN	BYTE	bCMDByte2,
	IN	BYTE	bVCC,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	)
{
	if (pbATRBuff != NULL
		&& pcbATRLength == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[33];
	DWORD Length = sizeof(Buff);

	LONG lResult;
	if (bVCC >= 0x30)
		lResult = ExecCommand(hReader, bCMDByte1, bCMDByte2, &bVCC, 1, Buff, &Length);
	else
		lResult = ExecCommand(hReader, bCMDByte1, bCMDByte2, Buff, &Length);

	CHECKLENGTH(lResult, Length >= 1);

	if (Buff[0] == 0x30)
		*pbProtocol = ICC_PROTOCOL_T0;
	else if (Buff[0] == 0x31)
		*pbProtocol = ICC_PROTOCOL_T1;
	else
		return F3_E_COMM_ERROR;

	if (Length > 1)
	{
		if (pbATRBuff != NULL
			&& pcbATRLength != NULL)
		{
			if ( *pcbATRLength < Length)
				return F3_E_BUFFER_TOO_SMALL;

			*pcbATRLength = Length - 1;
			memcpy(pbATRBuff, &Buff[1], *pcbATRLength);
		}
	}

	return lResult;
}


LONG WINAPI F3_CpuActivate(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN OPTIONAL	BYTE	bVCC
	)
{
	return IccReset(hReader, 0x51, 0x30, bVCC, pbProtocol, pbATRBuff, pcbATRLength);
}


LONG WINAPI F3_CpuDeactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x51, 0x31);
}


LONG WINAPI F3_CpuGetStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatus
	)
{
	if (pbStatus == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[1];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x51, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 1);

	if (! (Buff[0] >= 0x30 && Buff[0] <= 0x32))
		return F3_E_COMM_ERROR;
	*pbStatus = Buff[0];

	return 0;
}


LONG WINAPI F3_CpuWarmReset(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	)
{
	return IccReset(hReader, 0x51, 0x38, 0, pbProtocol, pbATRBuff, pcbATRLength);
}


LONG WINAPI F3_CpuTransmit(
	IN	READERHANDLE hReader,
	IN	BYTE	bProtocol,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	if (pbSendBuff == NULL
		|| pbRecvBuff == NULL
		|| pcbRecvLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x51, bProtocol, pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength);
}


LONG
WINAPI
F3_CpuPPSExchange(
	IN READERHANDLE hReader,
	IN BYTE bProtocol
	)
{
	BYTE b;
	if (bProtocol == ICC_PROTOCOL_T0)
		b = 0;
	else if (bProtocol == ICC_PROTOCOL_T1)
		b = 1;
	else
		return F3_E_INVALID_PARAMETER;
	
	return ExecCommand(hReader, 0x51, 0x41, &b, 1, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////
// SAM ������


LONG WINAPI F3_SamActivate(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN OPTIONAL BYTE bVCC
	)
{
	return IccReset(hReader, 0x52, 0x30, bVCC, pbProtocol, pbATRBuff, pcbATRLength);
}


LONG WINAPI F3_SamDeactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x52, 0x31);
}


LONG WINAPI F3_SamGetStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatus,
	OUT	PBYTE	pbSAMNumber
	)
{
	if (pbStatus == NULL
		|| pbSAMNumber == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[2];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x52, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 2);

	if (! (Buff[0] >= 0x30 && Buff[0] <= 0x32))
		return F3_E_COMM_ERROR;
	*pbStatus = Buff[0];

	*pbSAMNumber = Buff[1] - 0x2f;

	return lResult;
}



LONG WINAPI F3_SamWarmReset(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	)
{
	return IccReset(hReader, 0x52, 0x38, 0, pbProtocol, pbATRBuff, pcbATRLength);
}


LONG WINAPI F3_SamTransmit(
	IN	READERHANDLE hReader,
	IN	BYTE	bProtocol,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	if (pbSendBuff == NULL
		|| pbRecvBuff == NULL
		|| pcbRecvLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x52, 0x33, pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength);
}


LONG WINAPI F3_SamSelect(
	IN	READERHANDLE hReader,
	IN	BYTE	bSAMNumber
	)
{
	BYTE b = bSAMNumber + 0x2f;
	return ExecCommand(hReader, 0x52, 0x40, &b, 1, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////
// SLE4442 ������

LONG WINAPI F3_Sle4442Activate(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	)
{
	if (pbATRBuff != NULL
		&& pcbATRLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x53, 0x30, pbATRBuff, pcbATRLength);
}



LONG WINAPI F3_Sle4442Deactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x53, 0x31);
}


LONG WINAPI F3_Sle4442GetStatus(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pfActivated
	)
{
	if (pfActivated == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[1];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x53, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 1);

	*pfActivated = Buff[0] == 0x31;

	return lResult;
}


LONG WINAPI F3_Sle4442ReadMainMemory(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer == NULL
		|| pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xB0, 0, bStartAddress, bBytesToRead, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_Sle4442UpdateMainMemory(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xD0, 0, bStartAddress, bBytesToWrite, pbBuffer,  NULL, NULL);
}


LONG WINAPI F3_Sle4442ReadProtectionMemory(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer == NULL
		|| pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xB0, 1, bStartAddress,  bBytesToRead, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_Sle4442WriteProtectionMemory(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xD0, 1, bStartAddress, bBytesToWrite, pbBuffer, NULL, NULL);
}


LONG WINAPI F3_Sle4442ReadSecurityMemory(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer == NULL
		|| pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xB0, 2, bStartAddress, bBytesToRead, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_Sle4442VerifyPSC(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[3]
	)
{
	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0x20, 3, 1, 3, bPSCBytes, NULL, NULL);
}


LONG WINAPI F3_Sle4442UpdatePSC(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[3]
	)
{
	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xD0, 2, 1, 3, bPSCBytes, NULL, NULL);
}


LONG WINAPI F3_Sle4442WriteErrorCounter(
	IN	READERHANDLE hReader,
	IN	BYTE	bValue
	)
{
	return ExecApduCommand(hReader, 0x53, 0x33, 0, 0xD0, 2, 0, 1, &bValue, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////
// SLE4428 ������

LONG WINAPI F3_Sle4428Activate(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	)
{
	return F3_Sle4442Activate(hReader, pbATRBuff, pcbATRLength);
}

LONG WINAPI F3_Sle4428Deactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x53, 0x31);
}

LONG WINAPI F3_Sle4428GetStatus(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pbActivated
	)
{
	if (pbActivated == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[1];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x53, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 1);

	*pbActivated = Buff[0] == 0x32;

	return lResult;
}


LONG WINAPI F3_Sle4428ReadMainMemory(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer != NULL
		&& pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0xb0, HIBYTE(wStartAddress) & 0xf, LOBYTE(wStartAddress), bBytesToRead, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_Sle4428ReadProtectionBits(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer != NULL
		&& pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0xb0, HIBYTE(wStartAddress) & 0x1f, LOBYTE(wStartAddress), bBytesToRead, NULL, pbBuffer, pcbLength);
}



LONG WINAPI F3_Sle4428WriteWithoutPB(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0xd0, HIBYTE(wStartAddress) & 0xf, LOBYTE(wStartAddress), bBytesToWrite, pbBuffer, NULL, NULL);
}


LONG WINAPI F3_Sle4428WriteWithPB(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0xd0, HIBYTE(wStartAddress) & 0x2f, LOBYTE(wStartAddress), bBytesToWrite, pbBuffer, NULL, NULL);
}


LONG WINAPI F3_Sle4428WriteWithDataComparison(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0xd0, HIBYTE(wStartAddress) & 0x1f, LOBYTE(wStartAddress), bBytesToWrite, pbBuffer, NULL, NULL);
}



LONG WINAPI F3_Sle4428VerifyPSC(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[2]
	)
{
	return ExecApduCommand(hReader, 0x53, 0x34, 0, 0x20, 0, 0, 2, bPSCBytes, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////
// I2C�ڴ濨����

LONG WINAPI F3_I2cActivate(
	IN	READERHANDLE hReader,
	IN	BYTE	bCardType
	)
{
	return ExecCommand(hReader, 0x54, 0x30, &bCardType, 1, NULL, NULL);
}


LONG WINAPI F3_I2cDeactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x54, 0x31);
}


LONG WINAPI F3_I2cGetStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbActivatedCard
	)
{
	if (pbActivatedCard == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[1];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x54, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 1);

	if ( !(Buff[0] >= 0x30 && Buff[0] <= 0x39))
		return F3_E_COMM_ERROR;

	*pbActivatedCard = Buff[0];

	return lResult;
}


LONG WINAPI F3_I2cReadMemory(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x54, 0x33, 0, 0xb0, HIBYTE(wStartAddress), LOBYTE(wStartAddress), bBytesToRead, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_I2cWriteMemory(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x54, 0x33, 0, 0xd0, HIBYTE(wStartAddress), LOBYTE(wStartAddress), bBytesToWrite, pbBuffer, NULL, NULL);
}

/////////////////////////////////////////////////////////////////////
// ��Ƶ����������

LONG WINAPI F3_RfcActivate(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN	BYTE	bFirstProtocol,
	IN	BYTE	bSecondProtocol
	)
{
	if (pbATRBuff != NULL
		&& pcbATRLength == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE CMPBuff[] = { bFirstProtocol, bSecondProtocol };

	return ExecCommand(hReader, 0x60, 0x30, CMPBuff, sizeof(CMPBuff), pbATRBuff, pcbATRLength);
}


LONG WINAPI F3_RfcDeactivate(
	IN	READERHANDLE hReader
	)
{
	return ExecCommand(hReader, 0x60, 0x31);
}


LONG WINAPI F3_RfcGetStatus(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbActivatedCard
	)
{
	if (pbActivatedCard == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE Buff[2];
	DWORD Length = sizeof(Buff);

	LONG lResult = ExecCommand(hReader, 0x60, 0x32, Buff, &Length);
	CHECKLENGTH(lResult, Length == 2);

	if (Buff[0] == '1' && Buff[1] == '0')
		*pbActivatedCard = RFCTYPE_MIFARE_S50;
	else if (Buff[0] == '1' && Buff[1] == '1')
		*pbActivatedCard = RFCTYPE_MIFARE_S70;
	else if (Buff[0] == '1' && Buff[1] == '2')
		*pbActivatedCard = RFCTYPE_MIFARE_UL;
	else if (Buff[0] == '2' && Buff[1] == '0')
		*pbActivatedCard = RFCTYPE_TYPEA_CPU;
	else if (Buff[0] == '2' && Buff[1] == '1')
		*pbActivatedCard = RFCTYPE_TYPEB_CPU;
	else
		*pbActivatedCard = RFCTYPE_UNKNOWN;

	return lResult;
}


/////////////////////////////////////////////////////////////////////
// S50/70 ������


LONG WINAPI F3_MfVerifyPassword(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA,
	IN	BYTE	(&bKeyBytes)[6]
	)
{
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0x20, fWithKeyA ? 0 : 1, bSectorNumber, sizeof(bKeyBytes), bKeyBytes, NULL, NULL);
}


LONG WINAPI F3_MfUpdatePassword(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	(&bKeyBytes)[6]
	)
{
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD5, 0, bSectorNumber, sizeof(bKeyBytes), bKeyBytes, NULL, NULL);
}


// ��EEPROM�����������벢У��
LONG WINAPI F3_MfLoadPassword(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA
	)
{
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0x21, fWithKeyA ? 0 : 1, bSectorNumber, 0, NULL, NULL, NULL);
}


// �����������뵽EEPROM
LONG WINAPI F3_MfDownloadPassword(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA,
	IN	BYTE	(&bKeyBytes)[6]
	)
{
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD0, fWithKeyA ? 0 : 1, bSectorNumber, sizeof(bKeyBytes), bKeyBytes, NULL, NULL);
}


LONG WINAPI F3_MfReadSector(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	)
{
	if (pbBuffer == NULL 
		|| pcbLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xb0, bSectorNumber, bBlockNumber, 1, NULL, pbBuffer, pcbLength);
}


LONG WINAPI F3_MfWriteSector(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bStartBlockNumber,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	)
{
	if (pbBuffer == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD1, bSectorNumber, bStartBlockNumber, bBytesToWrite, pbBuffer, NULL, NULL);
}


LONG WINAPI F3_MfInitializeValue(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	)
{
	BYTE bValueBytes[] = { (BYTE) iValue,  iValue >> 8, iValue >> 16, iValue >> 24  };
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD2, bSectorNumber, bBlockNumber, sizeof(bValueBytes), bValueBytes, NULL, NULL);
}


LONG WINAPI F3_MfReadValue(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	OUT	UINT32	*piValue
	)
{
	if (piValue == NULL)
		return F3_E_INVALID_PARAMETER;

	BYTE	RecvBuff[4];
	DWORD	RecvLength = sizeof(RecvBuff);
	LONG	lResult = ExecApduCommand(hReader, 0x60, 0x33, 0, 0xB1, bSectorNumber, bBlockNumber, 0, NULL, RecvBuff, &RecvLength);
	CHECKLENGTH(lResult, RecvLength == 4);

	*piValue  = (RecvBuff[3] << 24) | (RecvBuff[2] << 16) | (RecvBuff[1] << 8) | RecvBuff[0];

	return 0;
}


LONG WINAPI F3_MfIncrementValue(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	)
{
	BYTE bValueBytes[] = { (BYTE) iValue,  iValue >> 8, iValue >> 16, iValue >> 24  };
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD3, bSectorNumber, bBlockNumber, sizeof(bValueBytes), bValueBytes, NULL, NULL);
}


LONG WINAPI F3_MfDecrementValue(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	)
{
	BYTE bValueBytes[] = { (BYTE) iValue,  iValue >> 8, iValue >> 16, iValue >> 24  };
	return ExecApduCommand(hReader, 0x60, 0x33, 0, 0xD4, bSectorNumber, bBlockNumber, sizeof(bValueBytes), bValueBytes, NULL, NULL);
}


/////////////////////////////////////////////////////////////////////
// �ǽӴ�CPU������

LONG WINAPI F3_TypeACpuTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	if (pbSendBuff == NULL
		|| pbRecvBuff == NULL
		|| pcbRecvLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x60, 0x34, pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength);
}


LONG WINAPI F3_TypeBCpuTransmit(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	)
{
	if (pbSendBuff == NULL
		|| pbRecvBuff == NULL
		|| pcbRecvLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x60, 0x35, pbSendBuff, cbSendLength, pbRecvBuff, pcbRecvLength);
}


LONG
WINAPI
F3_ISO14443_3_Activate(
	IN  READERHANDLE hReader,
	OUT PBYTE pbRetBuff,
	IN OUT PDWORD pcbRetLength
	)
{
	if (pbRetBuff == NULL
		|| pcbRetLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x60, 0x40, pbRetBuff, pcbRetLength);
}


LONG
WINAPI
F3_ISO14443_4_Activate(
	IN  READERHANDLE hReader,
	OUT PBYTE pbATRBuff,
	IN OUT PDWORD pcbATRLength
	)
{
	if (pbATRBuff == NULL
		|| pcbATRLength == NULL)
		return F3_E_INVALID_PARAMETER;

	return ExecCommand(hReader, 0x60, 0x41, pbATRBuff, pcbATRLength);
}