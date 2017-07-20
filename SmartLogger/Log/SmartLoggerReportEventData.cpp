#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerReportEventData.h"
#include "Str.h"

/**
 * @brief  オブジェクトの構築とともにフィールドへの値の割り当て，
 * またデータの組み立てを行う
 *
 * @param  type    イベントの種類
 * @param  eventId イベント識別子
 * @param  code    ログのコード
 * @param  message メッセージ
 */
CSmartLoggerReportEventData::CSmartLoggerReportEventData
	(WORD type, DWORD eventId, UINT code, TCHAR * message)
	: CReportEventData(type, 0, eventId)
{
	CStr typeString;
	CStr codeString;

	if (type == EVENTLOG_INFORMATION_TYPE)
	{
		typeString.AddChar('I');
	}
	else if (type == EVENTLOG_WARNING_TYPE)
	{
		typeString.AddChar('W');
	}
	else if (type == EVENTLOG_ERROR_TYPE)
	{
		typeString.AddChar('E');
	}

	codeString.AddChar('0' + code / 100);
	codeString.AddChar('0' + (code / 10) % 10);
	codeString.AddChar('0' + code % 10);

	strings->Add(message);
	strings->Add(typeString.GetCharPtr());
	strings->Add(codeString.GetCharPtr());
}
