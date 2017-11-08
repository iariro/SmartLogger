#include "StdAfx.h"
#include "Windows.h"
#include "winsvc.h"
#include "SmartLoggerService.h"
#include "ServiceInstall.h"

#include "winioctl.h"
#include <ntddscsi.h>
#include "SmartDevice.h"
#include "SmartAttribute.h"
#include "FluentdCall.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/ReportEventData.h"
#include "Log/SmartLoggerLogging.h"
#include "Log/SmartLoggerReportEventData.h"

#include "Utility/BinaryBlock.h"
#include "Registry/RegistryValue.h"
#include "Registry/RegistryKey.h"

int SMART_WATCH_INTERVAL = (1000 * 3600);

const CString registryPath =
	_T("HKEY_LOCAL_MACHINE\\Software\\TOPS\\SmartLogger");

DWORD WINAPI smartLogger_server(LPVOID lpParameter)
{
	CString * logFilePath = NULL;
	CString * fluentcatPath = NULL;
	CString * fluentdSensorName = NULL;
	CString * fluentdHostName = NULL;
	DWORD * smartWatchInterval2 = NULL;

	CRegistryRootKey root(registryPath);

	if(root.IsValid())
	{
		// ルートキーは有効。

		CString subKeyPath = CRegistryKey::GetSubKeyPath(registryPath);
		CRegistryKey * subKey = root.OpenForRead(subKeyPath);
		if (subKey != NULL)
		{
			// サブキーオープン成功。

			logFilePath = subKey->GetStringValue(_T("LogFilePath"));
			fluentcatPath = subKey->GetStringValue(_T("FluentcatPath"));
			fluentdSensorName = subKey->GetStringValue(_T("FluentdSensorName"));
			fluentdHostName = subKey->GetStringValue(_T("FluentdHostName"));
			smartWatchInterval2 = subKey->GetDWordValue(_T("SmartWatchInterval"));

			delete subKey;
		}
	}

	if (logFilePath == NULL ||
		fluentcatPath == NULL ||
		fluentdSensorName == NULL ||
		fluentdHostName == NULL)
	{
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("設定不足")));
		return 0;
	}

	DWORD smartWatchInterval;
	if (smartWatchInterval2 != NULL)
	{
		smartWatchInterval = *smartWatchInterval2 * 1000;
	}
	else
	{
		smartWatchInterval = SMART_WATCH_INTERVAL;
	}

	CSmartLoggerLogging::PutLog(&CStartServiceEvent(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName, smartWatchInterval));

	int retryCount = 0;
	BOOL lastSuccess = FALSE;
	DWORD lastSuccessTime;

	while (TRUE)
	{
		DWORD now = ::GetTickCount();

		if (!lastSuccess || (now - lastSuccessTime > smartWatchInterval))
		{
			// first time or interval time has passed.

			int blockCount =
				GetAndWriteSmart(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName);

			if (blockCount > 0)
			{
				// success

				lastSuccessTime = now;
				lastSuccess = TRUE;
			}
			else
			{
				// error

				if (retryCount <= 0)
				{
					// first error

					retryCount = 1;
				}
				else
				{
					// retrying

					retryCount--;
					if (retryCount <= 0)
					{
						// retry limit

						lastSuccessTime = now;
						lastSuccess = TRUE;
					}
				}
			}
		}

		::Sleep(1000);
	}

	if (logFilePath != NULL)
	{
		delete logFilePath;
	}
	if (fluentcatPath != NULL)
	{
		delete fluentcatPath;
	}
	if (fluentdSensorName != NULL)
	{
		delete fluentdSensorName;
	}
	if (fluentdHostName != NULL)
	{
		delete fluentdHostName;
	}
	if (smartWatchInterval2 != NULL)
	{
		delete smartWatchInterval2;
	}

	return 0;
}

const int JSON_BUFFER_SIZE = 10000;

/**
 * @brief Get drive size and build binary list or output Json.
 *
 * @param outMode OUT_FILE or OUT_STDOUT_JSON
 * @param list2 binary list of body part
 * @param blockCount reference of block count
 * @param nRetCode reference of return code
 * @param allocSuccess reference of Success and failure
 *
 * @date 2016/09/20
 *	- kumagai : new
 */
void GetDriveSize(char * jsonBuffer, int & jsonBufferLength, int & count)
{
	DWORD dwDrive;
	char pszDrive[16];
	UINT DriveType;
	BOOL fResult;
	unsigned __int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

	dwDrive = GetLogicalDrives();
	for (int nDrive = 0; nDrive < 26; nDrive++)
	{
		if (dwDrive & (1 << nDrive))
		{
			sprintf_s(pszDrive, sizeof(pszDrive), "%c:\\", nDrive + 'A');

			DriveType = GetDriveTypeA(pszDrive);
			if (DriveType == DRIVE_FIXED)
			{
				fResult =
					::GetDiskFreeSpaceExA(
						pszDrive,
						(PULARGE_INTEGER)&i64FreeBytesToCaller,
						(PULARGE_INTEGER)&i64TotalBytes,
						(PULARGE_INTEGER)&i64FreeBytes);

				if (count > 0)
				{
					jsonBuffer[jsonBufferLength] = ',';
					jsonBufferLength++;
				}

				jsonBufferLength += sprintf_s(
					jsonBuffer + jsonBufferLength,
					JSON_BUFFER_SIZE - jsonBufferLength,
					"\"Drive%c_TotalSize\": \"%I64u\","
					"\"Drive%c_FreeSize\": \"%I64u\"",
					'A' + nDrive,
					i64TotalBytes,
					'A' + nDrive,
					i64FreeBytes);
				count++;
			}
		}
	}
}

char * CreateSmartJson(BYTE * smart, BYTE * log)
{
	int count = 0;
	char * jsonBuffer = new char [JSON_BUFFER_SIZE];
	int jsonBufferLength = 0;

	jsonBuffer[jsonBufferLength] = '{';
	jsonBufferLength++;

	if (smart != NULL)
	{
		for (int i=1 ; i<512 ; i+=12)
		{
			int id = smart[i];
			int value = smart[i + 3];
			int raw = smart[i + 5] + (smart[i + 6] << 8);

			if (id == 0)
			{
				break;
			}

			char * name = NULL;
			for (int j=0 ; j<GetSmartAttributeCount() ; j++)
			{
				if (attributeIdAndName[j].id == id)
				{
					name = attributeIdAndName[j].name;
					break;
				}
			}

			if (count > 0)
			{
				jsonBuffer[jsonBufferLength] = ',';
				jsonBufferLength++;
			}

			jsonBufferLength += sprintf_s(
				jsonBuffer + jsonBufferLength,
				JSON_BUFFER_SIZE - jsonBufferLength,
				"\"%s_Value\":\"%d\",\"%s_Raw\":\"%d\"",
				name,
				value,
				name,
				raw);
			count++;
		}
	}

	if (log != NULL)
	{
		int errorLogCount = 0;
		errorLogCount = log[452] + (log[453] << 8);

		if (count > 0)
		{
			jsonBuffer[jsonBufferLength] = ',';
			jsonBufferLength++;
		}

		jsonBufferLength += sprintf_s(
			jsonBuffer + jsonBufferLength,
			JSON_BUFFER_SIZE - jsonBufferLength,
			"\"ErrorLogCount\":\"%d\"",
			errorLogCount);
	}

	GetDriveSize(jsonBuffer, jsonBufferLength, count);

	jsonBuffer[jsonBufferLength] = '}';
	jsonBufferLength++;
	jsonBuffer[jsonBufferLength] = '\0';
	jsonBufferLength++;

	return jsonBuffer;
}

int GetAndWriteSmart(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName)
{
	int blockCount = 0;
	CSmartDevice device;

	IDENTIFY_DEVICE_OUTDATA * identify = NULL;
	SMART_READ_DATA_OUTDATA * smartValue = NULL;
	SMART_READ_DATA_OUTDATA * threshold = NULL;
	SMART_READ_LOG_OUTDATA * log = NULL;

	if (device.Open(0))
	{
		DWORD readIdentifyError;
		DWORD readSmartError;
		DWORD readThresholdError;
		DWORD readLogError;

		identify = device.ReadIdentify(&readIdentifyError);
		smartValue = device.ReadSmart(&readSmartError);
		threshold = device.ReadThreshold(&readThresholdError);
		log = device.ReadLog(0, 1, &readLogError);
		device.Close();

		if (identify != NULL ||
			smartValue != NULL ||
			threshold != NULL ||
			log != NULL)
		{
			// got something

			if (identify != NULL)
			{
				blockCount++;
			}
			if (smartValue != NULL)
			{
				blockCount++;
			}
			if (threshold != NULL)
			{
				blockCount++;
			}
			if (log != NULL)
			{
				blockCount++;
			}

			if (! logFilePath.IsEmpty())
			{
				// ログ出力パスの指定あり

				CTime time = CTime::GetCurrentTime();
				char header [68];
				::ZeroMemory(header, 68);
				strcpy_s(header, "SMARTCrawlResult");

				sprintf_s(
					header + 48,
					16,
					"%04d%02d%02d%02d%02d%02d",
					time.GetYear(),
					time.GetMonth(),
					time.GetDay(),
					time.GetHour(),
					time.GetMinute(),
					time.GetSecond());

				memcpy(header + 64, &blockCount, sizeof(int));

				CFile file;

				CString filename;

				filename.Format(
					_T("%s\\smart_%04d%02d%02d"),
					logFilePath,
					time.GetYear(),
					time.GetMonth(),
					time.GetDay());

				if (file.Open(filename, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::typeBinary))
				{
					file.SeekToEnd();
					file.Write(header, sizeof(header));
					BYTE blockHeader [12];
					::ZeroMemory(blockHeader, sizeof(blockHeader));
					blockHeader[0] = 10;

					if (identify != NULL)
					{
						blockHeader[5] = 0xec;
						blockHeader[6] = 0x00;
						blockHeader[8] = IDENTIFY_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = IDENTIFY_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)identify->sSendCmdOutParam.bBuffer, IDENTIFY_BUFFER_SIZE);
					}
					if (smartValue != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd0;
						blockHeader[8] = READ_ATTRIBUTE_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = READ_ATTRIBUTE_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)smartValue->sSendCmdOutParam.bBuffer, READ_ATTRIBUTE_BUFFER_SIZE);
					}
					if (threshold != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd1;
						blockHeader[8] = READ_THRESHOLD_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = READ_THRESHOLD_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)threshold->sSendCmdOutParam.bBuffer, READ_THRESHOLD_BUFFER_SIZE);
					}
					if (log != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd5;
						blockHeader[7] = 0x00;
						blockHeader[8] = sizeof(ata_smart_exterrlog) % 0x100; // size
						blockHeader[9] = sizeof(ata_smart_exterrlog) / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)log->sSendCmdOutParam.bBuffer, sizeof(ata_smart_exterrlog));
					}
					file.Close();
					CSmartLoggerLogging::PutLog(&CGetSmartSuccessEvent(filename, blockCount));
				}
				else
				{
					CSmartLoggerLogging::PutLog(&CFileWriteErrorEvent(filename));
				}
			}
		}
		else
		{
			// all null

			CSmartLoggerLogging::PutLog(&CGetSmartErrorEvent(readIdentifyError, readSmartError, readThresholdError, readLogError));
		}
	}
	else
	{
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("CSmartDevice::open")));
	}

	char * jsonBuffer =
		CreateSmartJson(
			smartValue != NULL ? smartValue->pData : NULL,
			log != NULL ? log->pData : NULL);

	if (identify != NULL)
	{
		delete identify;
	}
	if (smartValue != NULL)
	{
		delete smartValue;
	}
	if (threshold != NULL)
	{
		delete threshold;
	}
	if (log != NULL)
	{
		delete log;
	}

	if (jsonBuffer != NULL)
	{
		if (! fluentCatPath.IsEmpty() && ! fluentdSensorName.IsEmpty() && ! fluentdHostName.IsEmpty())
		{
			// fluentcat呼び出し用パラメータの指定あり

			int sendRecv =
				SendtoFluentCat(
					fluentCatPath,
					fluentdSensorName,
					fluentdHostName,
					jsonBuffer);

			CSmartLoggerLogging::PutLog(&CSendtoFluentdEvent(sendRecv));
		}

		delete [] jsonBuffer;
	}

	return blockCount;
}
