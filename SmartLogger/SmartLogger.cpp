// SmartLogger.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SmartLogger.h"

#include "ServiceInstall.h"
#include "SmartWatcherService.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/SmartLoggerLogData.h"
#include "Log/SmartLoggerLogging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一のアプリケーション オブジェクトです。

CWinApp theApp;

using namespace std;

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
		nRetFunc = DispatchWcpa();
	}

	return nRetFunc;
}
