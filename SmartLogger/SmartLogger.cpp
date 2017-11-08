// SmartLogger.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "winsvc.h"
#include "SmartLogger.h"

#include "ServiceInstall.h"
#include "SmartLoggerService.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/ReportEventData.h"
#include "Log/SmartLoggerLogging.h"
#include "Log/SmartLoggerReportEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一のアプリケーション オブジェクトです。

CWinApp theApp;

using namespace std;

/*
 * グローバル変数の定義
 */
VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
HANDLE g_hEventTerminate;	/**< 終了シグナル受信用				*/
HANDLE g_hThreadService;	/**< サービスメインスレッド作成用	*/
DWORD g_dwCurrentState;	/**< 現在のサービス状態	*/
SERVICE_STATUS_HANDLE g_statusService;			/**< サービスステータス取得用		*/

/**
 * @brief	SCM から呼び出された場合、SMARTLoggerサービスを開始させる
 *
 * SCM 以外からの呼び出しでは失敗する
 *
 * @param	なし
 * @return	処理の成功・失敗
 * @retval	0	正常に登録成功
 * @retval	0以外	エラー
 *            - 1:サービスメイン関数の呼び出しに失敗
 */
int DispatchService(void)
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


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// MFC を初期化して、エラーの場合は結果を印刷します。
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 必要に応じてエラー コードを変更してください。
		_tprintf(_T("致命的なエラー: MFC の初期化ができませんでした。\n"));
		return 1;
	}

	int			nRetFunc;				// 呼び出し関数の戻り値
	
	if (argc > 1)
	{
		if( wcscmp(argv[1], OPTION_INSTALL) == 0 )
		{
			//
			// SCM への登録
			//
			puts("/install が呼び出されました");

			// 登録時のコマンドラインオプションにはサービスのパスも必要となる
			// 指定されたファイルの正当性（存在するかなど）はチェックしない
			if(argc <= 2 )
			{
				puts("/install オプションとして不正（実行ファイルパスがない）");
				return 1;
			}

			// 登録処理関数の呼び出し
			puts("サービスインストール実行");
			nRetFunc = CServiceInstall::install_service(argv[2]);
			printf("install_service=%d\n", nRetFunc);
		}
		else if( wcscmp(argv[1], OPTION_UNINSTALL) == 0 )
		{
			//
			// SCM から登録解除
			//

			puts("サービスアンインストール実行");

			// アンインストール前にサービスを停止する
			CServiceInstall::control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);

			// アンインストール実行
			return CServiceInstall::uninstall_service();
		}
		else if( wcscmp(argv[1], OPTION_START) == 0)
		{
			//
			// サービス開始
			//

			puts("/start が呼び出されました");
			return CServiceInstall::control_service(SMARTLOGGER_CONTROL_START, SERVICE_NAME);
		}
		else if(wcscmp(argv[1], OPTION_STOP) == 0)
		{
			//
			// サービス停止
			//

			puts("/stop が呼び出されました");
			nRetFunc = CServiceInstall::control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);
			printf("ControlWcpaService(SMARTLOGGER_CONTROL_STOP) ret = %d", nRetFunc);

			return nRetFunc;
		}
	}
	else
	{
		//
		// SCM から呼び出しが行われると正常に実行される
		//
		nRetFunc = DispatchService();
	}

	return nRetFunc;
}
