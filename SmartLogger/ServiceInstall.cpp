#include "StdAfx.h"
#include "Stdlib.h"
#include "windows.h"
#include "ServiceInstall.h"
#include "winsvc.h"

/**
 * @brief SMARTLOGGERサービスのインストールを行う
 *
 * @param	pModulePath		[in] サービスモジュールの実行パス
 * @return	処理の成功・失敗
 * @retval	0	正常に登録成功
 * @retval	0以外	エラー
 *            -  1:目的レジストリキーのオープンに失敗
 *            -  2:目的レジストリキーのデータ取得に失敗
 *            -  3:レジストリデータの書き換えに失敗
 *            - 11:pServiceName が不正(※ファイル名の長さ程度しか確認しません)
 *            - 12:SMC ハンドルのオープンに失敗
 *            - 13:サービスの追加に失敗
 *            - 14:サービス情報の整斉に失敗
 */
int CServiceInstall::install_service(TCHAR * pModulePath)
{
	int ret;

	// SCM へのサービス登録
	ret = register_service(pModulePath);
	if( ret != 0 )
	{
		// サービス登録に失敗
		return (ret + 10);
	}
	return 0;
}

/**
 * @brief SMARTLOGGERサービスのアンインストールを行う
 *
 * サービス起動順設定の解除（レジストリ操作）、サービス登録解除を行う。
 *
 * @return	処理の成功・失敗
 * @retval	0	正常に登録成功
 * @retval	0以外	エラー
 *            - 11:SCM ハンドルのオープンに失敗
 *            - 12:サービスハンドルのオープンに失敗
 *            - 13:サービスの登録解除に失敗
 */
int CServiceInstall::uninstall_service(void)
{
	int		ret;

	// SCM へのサービス登録解除
	ret = unregister_service();
	if( ret != 0 )
	{
		// サービス登録に失敗
		return (ret + 10);
	}
	return 0;
}

/**
 * @brief	サービスを SCM へ登録する
 * @param	pModulePath		[in] サービスモジュールの実行パス
 * @return	処理の成功・失敗
 * @retval	0	正常に登録成功
 * @retval	0以外	エラー
 *            - 1:pServiceName が不正(※ファイル名の長さ程度しか確認しません)
 *            - 2:SMC ハンドルのオープンに失敗
 *            - 3:サービスの追加に失敗
 *            - 4:サービス情報の整斉に失敗
 */
int CServiceInstall::register_service(TCHAR * pModulePath)
{
	/*
	 * 変数定義
	 */

	SC_HANDLE		hSCM;			// SCM ハンドル
	SC_HANDLE		hService;		// サービスハンドル
	BOOL			fRetFunc;		// 呼び出し関数の戻り値

	// インストールタイプの定義
	DWORD			DesiredAccess;	// サービスのアクセス権のタイプ
	DWORD			ServiceType;	// サービスタイプ
	DWORD			StartType;		// サービス開始のタイプ
	DWORD			ErrorControl;	// 障害が発生した場合に開始プログラムが実施するべき操作

	// 登録サービス情報
	SERVICE_DESCRIPTION		sd = {	// サービス説明
		SRV_DESC_INFO
	};



	/*
	 * 変数初期化
	 */

	// インストールタイプの設定
	DesiredAccess	= SERVICE_CHANGE_CONFIG;		// サービス構成の変更権限
	ServiceType		= SERVICE_WIN32_OWN_PROCESS;	// 専用のプロセス内で動作
	StartType		= SERVICE_AUTO_START;			// スタートアップの種類を「自動」に
	ErrorControl	= SERVICE_ERROR_IGNORE;			// エラー時も開始操作を続行する

	// サービスの開始コマンドの作成
	if( (wcslen(pModulePath) > _MAX_PATH) || (wcslen(pModulePath) == 0 ) )
	{
		// 明らかにサービスモジュールパスが不正と判断
		return 1;
	}

	/*
	 * SCM ハンドルのオープン
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if( hSCM == NULL )
	{
		// オープンに失敗
		return 2;
	}


	/*
	 * SCM データベースへのサービス登録の実行
	 */

	// サービスの追加
	hService = CreateService(hSCM, SERVICE_NAME, SERVICE_DISPLAY_NAME,
		DesiredAccess, ServiceType, StartType, ErrorControl,
		pModulePath, NULL, NULL, NULL, NULL, NULL);
	if( hService == NULL )
	{
		// 追加に失敗
		CloseServiceHandle(hSCM);
		return 3;
	}

	// サービス情報を変更
	fRetFunc = ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);
	if( fRetFunc == FALSE )
	{
		// 失敗
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService);
		return 4;
	}


	/*
	 * 正常終了
	 */

	// 終了処理
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return 0;
}

/**
 * @brief	サービスを SCM から登録解除する
 * @return	処理の成功・失敗
 * @retval	0	正常に登録解除成功
 * @retval	0以外	エラー
 *            - 1:SCM ハンドルのオープンに失敗
 *            - 2:サービスハンドルのオープンに失敗
 *            - 3:サービスの登録解除に失敗
 */
int CServiceInstall::unregister_service(void)
{
	/*
	 * 変数定義
	 */

	SC_HANDLE		hSCM;			// SCM ハンドル
	SC_HANDLE		hService;		// サービスハンドル
	BOOL			fRetFunc;		// 呼び出し関数の戻り値


	puts("called UninstallService()");

	/*
	 * 変数初期化
	 */

	/*
	 * サービス実行中の場合、停止する
	 * ※停止状態にできない場合でも、以降の処理を続行する。
	 */
	control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);


	/*
	 * SCM ハンドルのオープン
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if( hSCM == NULL )
	{
		// オープンに失敗
		return 1;
	}


	/*
	 * サービスハンドルのオープン
	 */
	hService = OpenService(hSCM, SERVICE_NAME, SERVICE_ALL_ACCESS);
	if( hService == NULL )
	{
		// オープンに失敗
		CloseServiceHandle(hSCM);
		return 2;
	}


	/*
	 * サービスの登録解除
	 */
	fRetFunc = DeleteService(hService);
	if( fRetFunc == FALSE )
	{
		// 登録解除に失敗
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService);
		return 3;
	}


	/*
	 * 終了処理
	 */
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);

	/*
	 * 正常終了
	 */
	return 0;
}

/**
 * @brief SMARTLOGGERサービスのコントロール関数
 *
 * サービスを指定された状態に変更する
 * 引数 dwControl には以下のものを指定する
 *   - SMARTLOGGER_CONTROL_START:	サービス開始
 *   - SMARTLOGGER_CONTROL_STOP:	サービス停止
 *
 * @param	dwControl	[in] 制御コード
 * @param	pService	[in] サービス名
 *
 * @return	処理の成功・失敗
 * @retval	0	正常に停止成功
 * @retval	0以外	エラー
 *			  - 1:指定した制御コードが不正
 *			  - 2:SCM ハンドルのオープンに失敗
 *			  - 3:サービスハンドルのオープンに失敗
 *			  - 4:サービス状態の取得に失敗
 *			  - 5:サービス状態操作に失敗
 */
int CServiceInstall::control_service(DWORD	dwControl,
					TCHAR	*pService)
{
	/*
	 * 変数定義
	 */
	SC_HANDLE		hSCM;			// SCM ハンドル
	SC_HANDLE		hService;		// サービスハンドル
	SERVICE_STATUS	serviceStatus;	// サービス状態取得用
	BOOL			fRetFunc;		// 呼び出し関数の戻り値

	/*
	 * 引数チェック
	 */
	if( (dwControl != SMARTLOGGER_CONTROL_START) &&
		(dwControl != SMARTLOGGER_CONTROL_STOP) )
	{
		// 不正な引数を与えられた
		return 1;
	}

	/*
	 * SCM ハンドルのオープン
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if( hSCM == NULL )
	{
		// オープンに失敗
		return 2;
	}

	/*
	 * サービスハンドルのオープン
	 */
	hService = OpenService(hSCM, pService, SERVICE_ALL_ACCESS);
	if( hService == NULL )
	{
		// オープンに失敗
		CloseServiceHandle(hSCM);
		return 3;
	}


	/*
	 * サービスの実行状態の取得
	 */
	fRetFunc = QueryServiceStatus(hService, &serviceStatus);
	if( fRetFunc == FALSE )
	{
		// 状態取得に失敗
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return 4;
	}

	switch( dwControl )
	{

	/*
	 * サービス開始
	 */

	case SMARTLOGGER_CONTROL_START:

		// 停止状態でない場合は何もしない
		if( serviceStatus.dwCurrentState != SERVICE_STOPPED )
		{
			break;
		}

		// サービスの開始
		fRetFunc = StartService(hService, 0, NULL);
		if( fRetFunc == FALSE )
		{
			// 開始に失敗
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return 5;
		}

		break;

	/*
	 * サービス停止
	 */

	case SMARTLOGGER_CONTROL_STOP:

		// はじめから停止状態の場合は何もしない
		if( serviceStatus.dwCurrentState == SERVICE_STOPPED )
		{
			break;
		}

		// サービスの停止
		fRetFunc = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		if( fRetFunc == FALSE )
		{
			// 停止に失敗
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return 5;
		}

		break;


	/*
	 * 引数が不正
	 */

	default:
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return 1;
	}


	/*
	 * 終了処理
	 */

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);


	/*
	 * 正常に処理終了
	 */

	return 0;
}
