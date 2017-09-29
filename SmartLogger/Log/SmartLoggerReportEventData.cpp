#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerReportEventData.h"
#include "Str.h"
#include "../../SmartLoggerMessage/Message/SmartLoggerMessage.h"

/**
 * @brief
 * オブジェクトの構築とともに基底クラスの初期化，メッセージの組み立てを行う。
 *
 * @author kumagai
 */
CStartServiceEvent::CStartServiceEvent(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName, int smartWatchInterval)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_START_SERVICE)
{
	TCHAR intervalString [BUFSIZ];
	wsprintf(intervalString, _T("%d"), smartWatchInterval);

	strings = new CCharPtrArray();
	strings->Add(fluentCatPath.GetBuffer());
	strings->Add(logFilePath.GetBuffer());
	strings->Add(fluentdSensorName.GetBuffer());
	strings->Add(fluentdHostName.GetBuffer());
	strings->Add(intervalString);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief
 * オブジェクトの構築とともに基底クラスの初期化，メッセージの組み立てを行う。
 *
 * @author kumagai
 */
CStopServiceEvent::CStopServiceEvent(void)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_STOP_SERVICE)
{
	// 何もしない
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param  info エラー発生時の処理内容
 */
CStartServiceErrorEvent::CStartServiceErrorEvent(TCHAR * info)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_START_SERVICE_ERROR)
{
	strings = new CCharPtrArray();
	strings->Add(info);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CFileWriteErrorEvent::CFileWriteErrorEvent(CString path)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_FILE_WRITE_ERROR)
{
	strings = new CCharPtrArray();
	strings->Add(path.GetBuffer());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CSendtoFluentdEvent1::CSendtoFluentdEvent1(CString commandLine)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_SENDTO_FLUENTD_EVENT1)
{
	strings->Add(commandLine.GetBuffer());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param error エラーコード
 */
CSendtoFluentdEvent::CSendtoFluentdEvent(int error, char * json)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_SENDTO_FLUENTD_EVENT)
{
	TCHAR errorString [BUFSIZ];

	// ステータス、進捗指標を数値から文字列に変換
	wsprintf(errorString, _T("%d"), error);

	CStringA jsonStringA(json);
	CString jsonString;
	jsonString =jsonStringA;

	strings = new CCharPtrArray();
	strings->Add(errorString);
	strings->Add(jsonString.GetBuffer());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに基底クラスの初期化を行う。
 *
 * @param  status エラー発生時の通知ステータス
 * @param  checkPoint エラー発生時のステータス進捗指標
 */
CSendStatusToSCMErrorEvent::CSendStatusToSCMErrorEvent(DWORD status, DWORD checkPoint)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_SENDSTATUSTOSCM_ERROR)
{
	TCHAR szIntegerStatus[32], szIntegerCheckPoint[32];

	// ステータス、進捗指標を数値から文字列に変換
	wsprintf(szIntegerStatus, _T("%d"), status);
	wsprintf(szIntegerCheckPoint, _T("%d"), checkPoint);

	strings = new CCharPtrArray();
	strings->Add(szIntegerStatus);
	strings->Add(szIntegerCheckPoint);
}
