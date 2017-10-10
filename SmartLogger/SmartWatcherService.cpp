#include "StdAfx.h"
#include "Windows.h"
#include "winsvc.h"
#include "SmartWatcherService.h"
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

/*
 * グローバル変数の定義
 */
SERVICE_STATUS_HANDLE	g_statusService;			/**< サービスステータス取得用		*/
HANDLE					g_hEventTerminate = NULL;	/**< 終了シグナル受信用				*/
HANDLE					g_hThreadService = NULL;	/**< サービスメインスレッド作成用	*/
DWORD	g_dwCurrentState = SERVICE_START_PENDING;	/**< 現在のサービス状態	*/


/**
 * @brief	SCM から呼び出された場合、Wcpa サービスを開始させる
 *
 * SCM 以外からの呼び出しでは失敗する
 *
 * @param	なし
 * @return	処理の成功・失敗
 * @retval	0	正常に登録成功
 * @retval	0以外	エラー
 *            - 1:サービスメイン関数の呼び出しに失敗
 */
int DispatchWcpa(void)
{
	/*
	 * 変数定義
	 */
	BOOL	fRetFunc;			// 呼び出し関数の戻り値

	// サービステーブルの定義
	SERVICE_TABLE_ENTRY	service_table[] = {
		{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};


	/*
	 * 変数初期化
	 */

	g_hEventTerminate	= NULL;
	g_hThreadService	= NULL;


	/*
	 * サービスディスパッチの実行
	 *   サービステーブルに定義した ServiceMain 関数の呼び出しが行われる
	 */
	fRetFunc = StartServiceCtrlDispatcher(service_table);
	if( fRetFunc == FALSE )
	{
		// ディスパッチに失敗(サービスから呼び出されない場合は失敗する)
		return 1;
	}

	// 正常に処理終了
	return 0;
}


/**
 * @brief	サービスメイン関数
 * @param	argc		[in] 引数個数
 * @param	argv		[in] 引数配列
 * @return	なし
 */
VOID WINAPI ServiceMain(DWORD argc,	// 引数個数
						LPTSTR *argv)	// 引数配列
{
	/*
	 * 変数定義
	 */

	BOOL	fRetFunc;			// 呼び出し関数の戻り値
	DWORD	dwIdService;		// サービス本体スレッド ID

	/*
	 * サービスコントロールハンドラ関数の登録
	 */
	g_statusService = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if( g_statusService == NULL )
	{
		// サービスコントロールハンドラ関数の登録失敗 - 即座に終了
		// ログ出力
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("サービスコントロールハンドラ関数の登録")));
		return;
	}

	// SCM へ状況通知 -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
	if( fRetFunc == FALSE )
	{
		// 通知に失敗 - 即座に終了
		return;
	}


	/*
	 * サービス終了シグナルイベントの生成
	 */
	g_hEventTerminate = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( !g_hEventTerminate )
	{
		// 失敗 - 即座に終了
		// ログ出力
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("サービス終了シグナルイベントの生成")));
		return;
	}

	// SCM へ状況通知 -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
	if( fRetFunc == FALSE )
	{
		// 通知に失敗 - 即座に終了
		return;
	}


	/*
	 * サービス本体部分の実行 - サービス本体スレッドを作成
	 */
	g_hThreadService = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)smartLogger_server, NULL, NULL, &dwIdService);
	if( g_hThreadService == NULL )
	{
		// スレッド作成に失敗 - 即座に終了
		// ログ出力
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("サービス本体スレッドの生成")));
		return;
	}

	// SCM へ状況通知 -  SERVICE_RUNNING
	fRetFunc = SendStatusToSCM(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	if( fRetFunc == FALSE )
	{
		// 通知に失敗 - 即座に終了
		if( g_hThreadService )
		{
			CloseHandle(g_hEventTerminate);
		}
		return;
	}

	/*
	 * 終了シグナルの待ち受け
	 */
	WaitForSingleObject(g_hEventTerminate, INFINITE);

	// 終了シグナルを検知した場合、シグナル受信用ハンドルをクローズして終了
	if( g_hEventTerminate )
	{
		CloseHandle(g_hEventTerminate);
	}

	// SCM へ SERVICE_STOPPED を通知する。
	// ServiceCtrlHandler ではなく、終了シグナルを受け取った時に通知する
	SendStatusToSCM(SERVICE_STOPPED, NO_ERROR, 0, 0, 100);

	return;
}


/**
 * @brief	サービスコントロールハンドラ
 * @param	fdwControl	[in] 要求したい制御コード
 * @return	なし
 */
VOID WINAPI ServiceCtrlHandler(DWORD fdwControl		// 要求したい制御コード
							   )
{
	/*
	 * 変数定義
	 */
	BOOL	fRetFunc;			// 呼び出し関数の戻り値

	/*
	 * サービスコントロールコード受信時の各処理定義部
	 */
	switch( fdwControl )
	{
	/*
	 * サービス停止要求受信
	 *   （停止時＝シャットダウン時）
	 */
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:

		/*
		 * SCM へ状況通知 -  SERVICE_STOP_PENDING
		 */
		fRetFunc = SendStatusToSCM(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 100);
		if( fRetFunc == FALSE )
		{
			// 通知に成功しなかった場合は特に何もしない
			break;
		}

		/*
		 * サービス本体スレッドをクローズ
		 */
		if( g_hThreadService )
		{
			CloseHandle(g_hThreadService);
			g_hThreadService = NULL;
		}

		// SMARTLOGGERが停止したことを示すログを出力
		CSmartLoggerLogging::PutLog(&CStopServiceEvent());

		/*
		 * 終了シグナルの通知
		 */
		SetEvent(g_hEventTerminate);
		return;

	/*
	 * 状態の更新
	 */
	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}


	/*
	 * SCM へ状況を通知
	 *   このプログラムでは、実質 SERVICE_CONTROL_INTERROGATE ( + default ) のためだけにある
	 *   通知失敗時も特に処理を行わない
	 */
	fRetFunc = SendStatusToSCM(g_dwCurrentState, NO_ERROR, 0, 0, 0);
	return;
}


/**
 * @brief	サービス状況を SCM に通知する
 * @param	dwCurrentState				[in] サービスステータス
 * @param	dwWin32ExitCode				[in] エラーコード
 * @param	dwServiceSpecificExitCode	[in] サービス定義エラーコード
 * @param	dwCheckPoint				[in] サービス進捗指標
 * @param	dwWaitHint					[in] 待機時間(ms)
 * @return	処理の成功・失敗
 * @retval	TRUE	処理成功
 * @retval	FALSE	処理に失敗
 */
BOOL SendStatusToSCM(DWORD	dwCurrentState,				// サービスステータス
					 DWORD	dwWin32ExitCode,			// エラーコード
					 DWORD	dwServiceSpecificExitCode,	// サービス定義エラーコード
					 DWORD	dwCheckPoint,				// サービス進捗指標
					 DWORD	dwWaitHint					// 待機時間(ms)
					 )
{
	/*
	 * 変数定義
	 */
	SERVICE_STATUS	status;		// サービスステータス構造体
	BOOL			fRetFunc;	// 呼び出し関数の戻り値

	/*
	 * 変数初期化
	 */
	status.dwServiceType	= SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState	= dwCurrentState;
	status.dwCheckPoint		= dwCheckPoint;
	status.dwWaitHint		= dwWaitHint;

	/*
	 * サービス状況により、dwControlsAccepted を設定する
	 *
	 *   ・SERVICE_START_PENDING(何かの処理を実行中)の場合、
	 *       コントロールイベントは受け取らない
	 *   ・それ以外
	 *       サービス停止(+サービス開始)のみ通知を受け取る
	 */
	status.dwControlsAccepted = (dwCurrentState == SERVICE_START_PENDING) ?
		0 : (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);

	/*
	 * サービス定義エラーを指定された場合はそれを利用する
	 */
	status.dwWin32ExitCode = (dwServiceSpecificExitCode == 0) ?
		dwWin32ExitCode : ERROR_SERVICE_SPECIFIC_ERROR;


	/*
	 * 状態レコードを SCM へ渡す
	 */
	fRetFunc = SetServiceStatus(g_statusService, &status);
	if( fRetFunc == FALSE )
	{
		// サービス状態の設定に失敗
		// ログを出力
		CSmartLoggerLogging::PutLog(&CSendStatusToSCMErrorEvent(dwCurrentState, dwCheckPoint));
		return FALSE;
	}

	g_dwCurrentState = dwCurrentState;

	/*
	 * 正常終了
	 */
	return TRUE;
}

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

	int smartWatchInterval;
	if (smartWatchInterval2 != NULL)
	{
		smartWatchInterval = *smartWatchInterval2 * 1000;
	}
	else
	{
		smartWatchInterval = SMART_WATCH_INTERVAL;
	}

	CSmartLoggerLogging::PutLog(&CStartServiceEvent(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName, smartWatchInterval));

	GetAndWriteSmart(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName);

	DWORD last = ::GetTickCount();

	while (TRUE)
	{
		DWORD now = ::GetTickCount();

		if (last + smartWatchInterval < now)
		{
			GetAndWriteSmart(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName);

			last += smartWatchInterval;
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

void GetAndWriteSmart(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName)
{
	CSmartDevice device;

	IDENTIFY_DEVICE_OUTDATA * identify = NULL;
	SMART_READ_DATA_OUTDATA * smartValue = NULL;
	SMART_READ_DATA_OUTDATA * threshold = NULL;
	SMART_READ_LOG_OUTDATA * log = NULL;

	if (device.Open(0))
	{
		identify = device.ReadIdentify();
		smartValue = device.ReadSmart();
		threshold = device.ReadThreshold();
		log = device.ReadLog(0, 1);
		device.Close();

		if (identify != NULL ||
			smartValue != NULL ||
			threshold != NULL ||
			log != NULL)
		{
			// got something

			int blockCount = 0;
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

			CSmartLoggerLogging::PutLog(&CGetSmartErrorEvent());
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

			CSmartLoggerLogging::PutLog(&CSendtoFluentdEvent(sendRecv, jsonBuffer));
		}

		delete [] jsonBuffer;
	}
}
