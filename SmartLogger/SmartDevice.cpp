#include "StdAfx.h"
#include "Windows.h"
#include "winioctl.h"
#include <ntddscsi.h>
#include "SmartDevice.h"

/**
 * @brief Open device
 *
 * @param deviceNo device no
 *
 * @return TRUE=successÅ^FALSE=error
 */
BOOL CSmartDevice::Open(BYTE deviceNo)
{
	TCHAR strDevice [MAX_PATH];

	this->deviceNo = deviceNo;
	swprintf(strDevice, MAX_PATH, _T("\\\\.\\PhysicalDrive%d"), deviceNo);
	handler =
		::CreateFile(
			strDevice,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

	return handler != INVALID_HANDLE_VALUE;
}

/**
 * @brief Close device
 *
 * @return TRUE=successÅ^FALSE=error
 */
BOOL CSmartDevice::Close()
{
	return TRUE;
}

/**
 * @brief Execute command. Note It's necessary to free output data.
 *
 * @param feature feature ID
 * @param command command ID
 * @param outLength output length
 * @param bufferSize buffer size
 *
 * @return output dataÅ^NULL=error
 */
BYTE * CSmartDevice::CommandInterface
	(BYTE feature, BYTE command, int outLength, int bufferSize)
{
	BYTE * sendCmdOutParam = new BYTE [outLength];
	::ZeroMemory(sendCmdOutParam, outLength);

	SENDCMDINPARAMS sSendCmd;
	::ZeroMemory(&sSendCmd,sizeof(SENDCMDINPARAMS));
	sSendCmd.irDriveRegs.bFeaturesReg = feature;
	sSendCmd.irDriveRegs.bCylLowReg = 0x4f;
	sSendCmd.irDriveRegs.bCylHighReg = 0xc2;
	sSendCmd.irDriveRegs.bDriveHeadReg = 0xA0 | ((deviceNo & 1) << 4); // drive no
	sSendCmd.irDriveRegs.bCommandReg = command; //SMART READ DATA command no
	sSendCmd.cBufferSize = bufferSize;
	sSendCmd.bDriveNumber = deviceNo; // drive no

	// execute comand
	BOOL	bRet;
	DWORD	dwReturned;
	bRet =
		::DeviceIoControl(
			handler,
			DFP_RECEIVE_DRIVE_DATA,
			&sSendCmd,
			sizeof(SENDCMDINPARAMS),
			sendCmdOutParam,
			outLength,
			&dwReturned,
			NULL);

	if (!bRet || dwReturned != sizeof(SMART_READ_DATA_OUTDATA))
	{
		// execute error or return size error

		delete [] sendCmdOutParam;

		return NULL;
	}

	return sendCmdOutParam;
}

/**
 * @brief get IDENTIFY
 *
 * @return IDENTIFY
 */
IDENTIFY_DEVICE_OUTDATA * CSmartDevice::ReadIdentify()
{
	return
		(IDENTIFY_DEVICE_OUTDATA *)CommandInterface(
			0,
			ATAPI_IDENTIFY_DEVICE,
			sizeof(IDENTIFY_DEVICE_OUTDATA),
			IDENTIFY_BUFFER_SIZE);
}

/**
 * @brief Get S.M.A.R.T. value
 *
 * @return S.M.A.R.T. value
 */
SMART_READ_DATA_OUTDATA * CSmartDevice::ReadSmart()
{
	return
		(SMART_READ_DATA_OUTDATA *)CommandInterface(
			ATA_SMART_READ_VALUES,
			ATAPI_SMART_READ_DATA,
			sizeof(SMART_READ_DATA_OUTDATA),
			READ_ATTRIBUTE_BUFFER_SIZE);
}

/**
 * @brief Get threshold value
 *
 * @return threshold value
 */
SMART_READ_DATA_OUTDATA * CSmartDevice::ReadThreshold()
{
	return
		(SMART_READ_DATA_OUTDATA *)CommandInterface(
			ATA_SMART_READ_THRESHOLDS,
			ATAPI_SMART_READ_DATA,
			sizeof(SMART_READ_DATA_OUTDATA),
			READ_THRESHOLD_BUFFER_SIZE);
}

/**
 * @brief Read log
 *
 * @return SMART log
 *
 * @param logAddress log address
 * @param sectorCount sector count
 */
SMART_READ_LOG_OUTDATA * CSmartDevice::ReadLog(int logAddress, int sectorCount)
{
	int outLength = 0x20 + SMART_LOG_SECTOR_SIZE * sectorCount;
	BYTE * sendCmdOutParam = new BYTE [outLength];
	::ZeroMemory(sendCmdOutParam, outLength);

	SENDCMDINPARAMS sSendCmd;
	::ZeroMemory(&sSendCmd, sizeof(SENDCMDINPARAMS));
	sSendCmd.irDriveRegs.bFeaturesReg	= ATA_SMART_READ_LOG_SECTOR;
	sSendCmd.irDriveRegs.bSectorCountReg  = (BYTE)sectorCount;
	sSendCmd.irDriveRegs.bSectorNumberReg = (BYTE)logAddress;
	sSendCmd.irDriveRegs.bCylLowReg		= 0x4f;
	sSendCmd.irDriveRegs.bCylHighReg	= 0xc2;
	sSendCmd.irDriveRegs.bDriveHeadReg	= 0xA0 | ((deviceNo & 1) << 4); // drive no
	sSendCmd.irDriveRegs.bCommandReg	= ATAPI_SMART_READ_DATA; // SMART READ DATA command

	sSendCmd.cBufferSize	= SMART_LOG_SECTOR_SIZE * sectorCount;
	sSendCmd.bDriveNumber	= deviceNo;

	BOOL	bRet;
	DWORD	dwReturned;
	bRet =
		::DeviceIoControl(
			handler,
			DFP_RECEIVE_DRIVE_DATA,
			&sSendCmd,
			sizeof(SENDCMDINPARAMS),
			sendCmdOutParam,
			outLength,
			&dwReturned,
			NULL);

	if (!bRet)
	{
		delete sendCmdOutParam;

		return NULL;
	}

	return (SMART_READ_LOG_OUTDATA *)sendCmdOutParam;
}

/**
 * @brief Read extended log - P194
 *
 * @return SMART extended log
 *
 * @param logAddress log address
 * @param sectorCount sector count
 */
BYTE * CSmartDevice::ReadLogExt(int logAddress, int sectorCount)
{
	int datasize = SMART_LOG_SECTOR_SIZE * sectorCount;

	// Set IDEREGS
	IDEREGS regs, prev_regs;

	regs.bFeaturesReg     = 0;
	regs.bSectorCountReg  = (BYTE)sectorCount;
	regs.bSectorNumberReg = (BYTE)logAddress;
	regs.bCylLowReg       = 0;
	regs.bCylHighReg      = 0;
	regs.bDriveHeadReg    = 0;
	regs.bCommandReg      = ATA_READ_LOG_EXT;
	regs.bReserved        = 0;

	prev_regs.bFeaturesReg     = 0;
	prev_regs.bSectorCountReg  = 0;
	prev_regs.bSectorNumberReg = 0;
	prev_regs.bCylLowReg       = 0;
	prev_regs.bCylHighReg      = 0;
	prev_regs.bDriveHeadReg    = 0;
	prev_regs.bCommandReg      = 0;
	prev_regs.bReserved        = 0;

	typedef struct
	{
		ATA_PASS_THROUGH_EX apt;
		ULONG Filler;
		UCHAR ucDataBuf[32 * 512];
	} ATA_PASS_THROUGH_EX_WITH_BUFFERS;

	const unsigned char magic = 0xcf;

	ATA_PASS_THROUGH_EX_WITH_BUFFERS passThrough;
	::ZeroMemory(&passThrough, sizeof(passThrough));
	passThrough.apt.Length = sizeof(ATA_PASS_THROUGH_EX);
	passThrough.apt.TimeOutValue = 10;
	unsigned size = offsetof(ATA_PASS_THROUGH_EX_WITH_BUFFERS, ucDataBuf);
	passThrough.apt.DataBufferOffset = size;

	passThrough.apt.AtaFlags = ATA_FLAGS_DATA_IN;
	passThrough.apt.DataTransferLength = datasize;
	size += datasize;
	passThrough.ucDataBuf[0] = magic;

	IDEREGS * ctfregs = (IDEREGS *)passThrough.apt.CurrentTaskFile;
	IDEREGS * ptfregs = (IDEREGS *)passThrough.apt.PreviousTaskFile;

	*ctfregs = regs;
	*ptfregs = prev_regs;

	passThrough.apt.AtaFlags |= ATA_FLAGS_48BIT_COMMAND;

	DWORD num_out;
	BOOL recv =
		::DeviceIoControl(
			handler,
			IOCTL_ATA_PASS_THROUGH,
			&passThrough,
			size,
			&passThrough,
			size,
			&num_out,
			NULL);

	if (recv)
	{
		BYTE * data = new BYTE [datasize];

		::CopyMemory(data, passThrough.ucDataBuf, datasize);

		return data;
	}
	else
	{
		return NULL;
	}
}
