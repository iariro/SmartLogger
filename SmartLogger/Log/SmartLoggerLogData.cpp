#include "StdAfx.h"
#include "Str.h"
#include "SmartLoggerLogdata.h"
#include "../../SmartLoggerMessage/Message/SmartLoggerMessage.h"

/**
 * @brief  イベントのソースの名前。
 *
 * @author kumagai
 */
const TCHAR * CSmartLoggerLogData::sourceName = _T("SMART Logger");

/**
 * @brief  オブジェクトの構築とともにフィールドへの値の割り当てを行う。
 *
 * @author kumagai
 *
 * @param  type イベントの種類
 * @param  code ログコード
 */
CSmartLoggerLogData::CSmartLoggerLogData(UINT type, UINT code)
	:	source(sourceName), type(type), eventId(SMARTLOGGER_MESSAGE), code(code),
		message(NULL)
{
	// 何もしない。
}

/**
 * @brief  オブジェクトの破棄とともに集約していたメッセージデータの解放を行う。
 *
 * @author kumagai
 */
CSmartLoggerLogData::~CSmartLoggerLogData()
{
	if (message != NULL)
	{
		delete message;
	}
}

/**
 * @brief  メッセージデータをセット。
 *
 * @author kumagai
 *
 * @param  message メッセージ
 */
void CSmartLoggerLogData::SetMessage(CStr * message)
{
	if(this->message != NULL)
	{
		delete message;
	}

	this->message = message;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 * オブジェクトの構築とともに基底クラスの初期化，メッセージの組み立てを行う。
 *
 * @author kumagai
 */
CStartServiceLog::CStartServiceLog(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 100)
{
	CStr * str;

	str = new CStr(_T("SMART Loggerサービスの稼動を開始しました。\r\n"));
	str->Add(_T("\r\nFluentCatPath="));
	str->Add(fluentCatPath.GetBuffer());
	str->Add(_T("\r\nLogFilePath="));
	str->Add(logFilePath.GetBuffer());
	str->Add(_T("\r\nFluentdSensorName="));
	str->Add(fluentdSensorName.GetBuffer());
	str->Add(_T("\r\nFluentdHostName="));
	str->Add(fluentdHostName.GetBuffer());

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 * オブジェクトの構築とともに基底クラスの初期化，メッセージの組み立てを行う。
 *
 * @author kumagai
 */
CStopServiceLog::CStopServiceLog(void)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 110)
{
	CStr * str;

	str = new CStr(_T("SMART Loggerサービスの稼動を停止しました。"));

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param  info エラー発生時の処理内容
 */
CStartServiceErrorLog::CStartServiceErrorLog(TCHAR * info)
	: CSmartLoggerLogData(EVENTLOG_ERROR_TYPE, 101)
{
	CStr * str;

	str = new CStr();
	str->Add(_T("サービス開始処理実行中に「"));
	str->Add(info);
	str->Add(_T("」にてエラーが発生しました。"));

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CFileWriteErrorLog::CFileWriteErrorLog(CString path)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 130)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	wsprintf(errorString, _T("ファイル「%s」の作成に失敗"), path);

	str = new CStr();
	str->Add(errorString);

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CSendtoFluentdLog1::CSendtoFluentdLog1(CString commandLine)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 131)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	// ステータス、進捗指標を数値から文字列に変換
	wsprintf(errorString, _T("(%s)"), commandLine);

	str = new CStr();
	str->Add(_T("fluent-cat実行"));
	str->Add(errorString);

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CSendtoFluentdLog::CSendtoFluentdLog(int error, char * json)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 132)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	// ステータス、進捗指標を数値から文字列に変換
	wsprintf(errorString, _T("(%d)"), error);

	CStringA jsonStringA(json);
	CString jsonString;
	jsonString =jsonStringA;

	str = new CStr();
	str->Add(_T("fluent-cat実行"));
	str->Add(errorString);
	str->Add(jsonString.GetBuffer());

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param  status エラー発生時の通知ステータス
 * @param  checkPoint エラー発生時のステータス進捗指標
 */
CSendStatusToSCMErrorLog::CSendStatusToSCMErrorLog(DWORD status, DWORD checkPoint)
	: CSmartLoggerLogData(EVENTLOG_ERROR_TYPE, 140)
{
	CStr * str;
	TCHAR szIntegerStatus[32], szIntegerCheckPoint[32];

	// ステータス、進捗指標を数値から文字列に変換
	wsprintf(szIntegerStatus, _T("%d"), status);
	wsprintf(szIntegerCheckPoint, _T("%d"), checkPoint);

	str = new CStr();
	str->Add(_T("SCMへのサービス状況（ステータス["));
	str->Add(szIntegerStatus);
	str->Add(_T("], 進捗指標["));
	str->Add(szIntegerCheckPoint);
	str->Add(_T("]）の通知に失敗しました。"));
	SetMessage(str);
}
