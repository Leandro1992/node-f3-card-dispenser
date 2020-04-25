/*++

Module Name:

    F3API.H

Abstract:

    Master include file for applications that call 
    functions provided by F3API.DLL

Revision History:

    2013-12-30 : created

--*/


#ifndef _F3API_H
#define _F3API_H


#include <Windows.h>

// Return codes of the APIs
//
#define F3_S_SUCCESS			0
#define F3_E_PORT_UNAVAILABLE		0x3000
#define F3_E_DEV_NOT_RECOGNIZED		0x3002
#define F3_E_COMM_ERROR			0x3010
#define F3_E_COMM_TIMEOUT		0x3011
#define F3_E_UNKNOWN_ERROR		0x3021
#define F3_E_MESSAGE_TOO_LONG		0x4010
#define F3_E_NO_MEMORY			0x4020
#define F3_E_BUFFER_TOO_SMALL		0x4000
#define F3_E_INVALID_HANDLE		0x4001
#define F3_E_UNDEFINED_COMMAND		0x4E00
#define F3_E_INVALID_PARAMETER		0x4E01
#define F3_E_DISABLED_COMMAND		0x4E02
#define F3_E_UNSUPPORTED_COMMAND	0x4E03
#define F3_E_CONTACT_NO_RELEASE		0x4E05
#define F3_E_CARD_JAMMED		0x4E10
#define F3_E_SENSOR_ABNORMALITY		0x4E12
#define F3_E_TOO_LONG_CARD		0x4E13
#define F3_E_TOO_SHORT_CARD		0x4E14
#define F3_E_CARD_WITHDRAWN		0x4E40
#define F3_E_IC_SOLENOID_ERROR		0x4E41
#define F3_E_CANT_MOVED_TO_IC_POS	0x4E43
#define F3_E_CARD_POSITION_CHANGE	0x4E45
#define F3_E_COUNTER_OVERFLOW		0x4E50
#define F3_E_MOTOR_ABNORMALITY		0x4E51
#define F3_E_POWER_SHORT		0x4E60
#define F3_E_ICC_ACTIVATION_ERROR	0x4E61
#define F3_E_ICC_NOT_ACTIVATED		0x4E65
#define F3_E_UNSUPPORTED_ICC		0x4E66
#define F3_E_ICC_RECEPTION_ERROR	0x4E67
#define F3_E_ICC_COMM_TIMEOUT		0x4E68
#define F3_E_MISMATCH_EMV		0x4E69
#define F3_E_CARD_BOX_EMPTY		0x4EA1
#define F3_E_CAPTURE_BOX_FULL		0x4EA2
#define F3_E_WAITING_FOR_RESET		0x4EB0
#define F3_E_COMMAND_FAILURE		0x6F00
#define F3_E_DISAGREEMENT_OF_VC		0x6F01
#define F3_E_CARD_LOCKED		0x6F02
#define F3_E_ADDRESS_OVERFLOW		0x6B00
#define F3_E_LENGTH_OVERFLOW		0x6700


// Handle of the C/R
//
typedef ULONG_PTR READERHANDLE;
typedef READERHANDLE *LPREADERHANDLE;


// Basic Operations
//

LONG 
WINAPI 
F3_Connect
	(
	IN	DWORD	dwPort,
	IN	DWORD	dwSpeed,
	IN	BYTE	bCRAddr,
	OUT	LPREADERHANDLE lphReader
	);
LONG 
WINAPI
F3_Disconnect
	(
	IN	READERHANDLE hReader
	);
LONG
WINAPI
F3_SetCRAddress
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bCRAddr
	);

// Initialize Modes
//
#define INIT_RETURN_TO_FRONT	0x30
#define INIT_CAPTURE_TO_BOX	0x31
#define INIT_WITHOUT_MOVEMENT	0x33
LONG 
WINAPI 
F3_Initialize
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bMode,
	IN	BOOL	fEnableCounter,
	OUT	PSTR	pszRevBuff,
	IN OUT	PDWORD	pcbRevLength
	);

// Lane Status
//
#define LS_NO_CARD_IN		0x30
#define LS_CARD_AT_GATE_POS	0x31
#define LS_CARD_IN		0x32

// Card Box Status
//
#define CBS_EMPTY		0x30
#define CBS_INSUFFICIENT	0x31
#define CBS_ENOUGH		0x32

// C/R Status
//
typedef struct _CRSTATUS
{
	BYTE	bLaneStatus;
	BYTE	bCardBoxStatus;	
	BOOL	fCaptureBoxFull;
} CRSTATUS, *PCRSTATUS;

LONG 
WINAPI 
F3_GetCRStatus
	(
	IN	READERHANDLE hReader,
	OUT	PCRSTATUS lpStatus
	);

#define NUM_SENSORS	12

LONG 
WINAPI 
F3_GetSenserDetail
	(
	IN	READERHANDLE hReader,
	OUT	BYTE	(&bStatus)[NUM_SENSORS]
	);
LONG 
WINAPI 
F3_GetCRAddress
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCRAddr
	);

#define MM_RETURN_TO_FRONT	0x30
#define MM_RETURN_TO_IC_POS	0x31
#define MM_RETURN_TO_RF_POS	0x32
#define MM_CAPTURE_TO_BOX	0x33
#define MM_EJECT_TO_FRONT	0x39
LONG 
WINAPI
F3_MoveCard
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bMode
	);
LONG 
WINAPI 
F3_PermitInsertion
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_DenieInsertion
	(
	IN	READERHANDLE hReader
	);

// IC Card Type
//
#define ICCTYPE_UNKNOWN		0xFF
#define ICCTYPE_TO_CPU		0x10
#define ICCTYPE_T1_CPU		0x11
#define ICCTYPE_SLE4442		0x20
#define ICCTYPE_SLE4428		0x21
#define ICCTYPE_AT24C01		0x30
#define ICCTYPE_AT24C02		0x31
#define ICCTYPE_AT24C04		0x32
#define ICCTYPE_AT24C08		0x33
#define ICCTYPE_AT24C16		0x34
#define ICCTYPE_AT24C32		0x35
#define ICCTYPE_AT24C64		0x36
#define ICCTYPE_AT24C128	0x37
#define ICCTYPE_AT24C256	0x38
LONG 
WINAPI
F3_DetectIccType
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCardType
	);

// RF Card Type
//
#define RFCTYPE_UNKNOWN		0xFF
#define RFCTYPE_MIFARE_S50	0x10
#define RFCTYPE_MIFARE_S70	0x11
#define RFCTYPE_MIFARE_UL	0x12
#define RFCTYPE_TYPEA_CPU	0x20
#define RFCTYPE_TYPEB_CPU	0x30
LONG 
WINAPI 
F3_DetectRfcType
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbCardType
	);



// Supply voltage during activation
//
#define VCC_5V_EMV		0x30
#define VCC_5V_ISO7816		0x33
#define VCC_3V_ISO7816		0x35

// CPU/SAM Protocol Type
//
#define ICC_PROTOCOL_T0		0x33
#define ICC_PROTOCOL_T1		0x34
#define ICC_PROTOCOL_AUTO	0x39

// CPU/SAM Card Status
//
#define STATUS_DEACTIVATION	0x30
#define STATUS_CLKFREQ_3_57	0x31
#define STATUS_CLKFREQ_7_16	0x32

// CPU Card Operations
//

LONG 
WINAPI 
F3_CpuActivate
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN OPTIONAL	BYTE	bVCC = VCC_5V_EMV
	);
LONG 
WINAPI 
F3_CpuDeactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_CpuGetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatus
	);
LONG 
WINAPI 
F3_CpuWarmReset
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	);
LONG 
WINAPI 
F3_CpuTransmit
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bProtocol,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	);


// SAM Card Operations
//

LONG
WINAPI
F3_SamActivate
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN OPTIONAL BYTE bVCC = VCC_5V_EMV
	);
LONG 
WINAPI 
F3_SamDeactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_SamGetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbStatus,
	OUT	PBYTE	pbSAMNumber
	);
LONG 
WINAPI 
F3_SamWarmReset
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbProtocol,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	);
LONG 
WINAPI
F3_SamTransmit
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bProtocol,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	);
LONG 
WINAPI 
F3_SamSelect
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSAMNumber		// 1, 2, 3 ...
	);

// SLE4442 Operations
//

LONG 
WINAPI
F3_Sle4442Activate
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	);
LONG 
WINAPI 
F3_Sle4442Deactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_Sle4442GetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pfActivated
	);
LONG
WINAPI 
F3_Sle4442ReadMainMemory
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_Sle4442UpdateMainMemory
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	nBytesToWrite,
	IN	PBYTE	pbBuffer
	);
LONG 
WINAPI
F3_Sle4442ReadProtectionMemory
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_Sle4442WriteProtectionMemory
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	);
LONG 
WINAPI 
F3_Sle4442ReadSecurityMemory
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_Sle4442VerifyPSC
	(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[3]
	);
LONG 
WINAPI 
F3_Sle4442UpdatePSC
	(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[3]
	);
LONG 
WINAPI 
F3_Sle4442WriteErrorCounter
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bValue
	);


// SLE4428 Card Operations
//

LONG 
WINAPI 
F3_Sle4428Activate
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength
	);
LONG 
WINAPI 
F3_Sle4428Deactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_Sle4428GetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBOOL	pbActivated
	);
LONG 
WINAPI 
F3_Sle4428ReadMainMemory
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_Sle4428ReadProtectionBits
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_Sle4428WriteWithoutPB
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	);
LONG 
WINAPI
F3_Sle4428WriteWithPB
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	);
LONG 
WINAPI 
F3_Sle4428WriteWithDataComparison
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	);
LONG 
WINAPI 
F3_Sle4428VerifyPSC
	(
	IN	READERHANDLE hReader,
	IN	BYTE	(&bPSCBytes)[2]
	);

// I2C Operations
//

#define I2CTYPE_UNKNOWN		0x30
#define I2CTYPE_24C01		0x31
#define I2CTYPE_24C02		0x32
#define I2CTYPE_24C04		0x33
#define I2CTYPE_24C08		0x34
#define I2CTYPE_24C16		0x35
#define I2CTYPE_24C32		0x36
#define I2CTYPE_24C64		0x37
#define I2CTYPE_24C128		0x38
#define I2CTYPE_24C256		0x39
LONG 
WINAPI 
F3_I2cActivate
	(
	IN	READERHANDLE hReader,
	IN	BYTE	 bCardType
	);
LONG 
WINAPI
F3_I2cDeactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_I2cGetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbActivatedCard
	);
LONG 
WINAPI 
F3_I2cReadMemory
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI
F3_I2cWriteMemory
	(
	IN	READERHANDLE hReader,
	IN	WORD	wStartAddress,
	IN	BYTE	bBytesToWrite,
	IN	PBYTE	pbBuffer
	);

// RF Card Operations

#define RFC_PROTOCOL_NONE	0x30
#define RFC_PROTOCOL_TYPE_A	0x41
#define RFC_PROTOCOL_TYPE_B	0x42

LONG 
WINAPI 
F3_RfcActivate
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbATRBuff,
	IN OUT	PDWORD	pcbATRLength,
	IN	BYTE	bFirstProtocol = RFC_PROTOCOL_NONE,
	IN	BYTE	bSecondProtocol = RFC_PROTOCOL_NONE
	);
LONG 
WINAPI 
F3_RfcDeactivate
	(
	IN	READERHANDLE hReader
	);
LONG 
WINAPI 
F3_RfcGetStatus
	(
	IN	READERHANDLE hReader,
	OUT	PBYTE	pbActivatedCard
	);


// S50/S70 Card Operations
//

LONG 
WINAPI 
F3_MfVerifyPassword
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA,
	IN	BYTE	(&bKeyBytes)[6]
	);
LONG 
WINAPI 
F3_MfUpdatePassword
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	(&bKeyBytes)[6]
	);
LONG 
WINAPI 
F3_MfLoadPassword
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA
	);
LONG 
WINAPI
F3_MfDownloadPassword
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BOOL	fWithKeyA,
	IN	BYTE	(&bKeyBytes)[6]
	);
LONG 
WINAPI 
F3_MfReadSector
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bStartBlockNumber,
	IN	BYTE	bBlocksToRead,
	OUT	PBYTE	pbBuffer,
	IN OUT	PDWORD	pcbLength
	);
LONG 
WINAPI 
F3_MfWriteSector
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bStartBlockNumber,
	IN	BYTE	bBlocksToWrite,
	IN	PBYTE	pbBuffer
	);
LONG
WINAPI 
F3_MfInitializeValue
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	);
LONG
WINAPI 
F3_MfReadValue
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	OUT	UINT32	*piValue
	);
LONG 
WINAPI 
F3_MfIncrementValue
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	);
LONG 
WINAPI 
F3_MfDecrementValue
	(
	IN	READERHANDLE hReader,
	IN	BYTE	bSectorNumber,
	IN	BYTE	bBlockNumber,
	IN	UINT32	iValue
	);

// TypeA/B CPU Operations
//

LONG 
WINAPI 
F3_TypeACpuTransmit
	(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	);

LONG 
WINAPI 
F3_TypeBCpuTransmit
	(
	IN	READERHANDLE hReader,
	IN	PBYTE	pbSendBuff,
	IN	USHORT	cbSendLength,
	OUT	PBYTE	pbRecvBuff,
	IN OUT	PDWORD	pcbRecvLength
	);

#endif // _F3API_H