#include "StdAfx.h"
#include "Str.h"
#include "StrArray.h"
#include "SmartLoggerLogData.h"
#include "SmartLoggerLogging.h"
#include "ReportEventData.h"
#include "EventReporter.h"
#include "SmartLoggerReportEventData.h"


/**
 * @brief  ログ情報出力
 * logData は本関数内で解放するため、呼び出し側での解放の必要なし。
 *
 * @author kumagai
 *
 * @param  logData ログデータ
 */
void CSmartLoggerLogging::PutLog(CSmartLoggerLogData * logData)
{
	BOOL recv;
	CEventReporter * reporter;
	CReportEventData * data;

	reporter = new CEventReporter(logData->source);

	recv = reporter->Open();
	if (recv)
	{
		data =
			new CSmartLoggerReportEventData(
				(WORD)logData->type,
				(DWORD)logData->eventId,
				logData->code,
				logData->message->GetCharPtr());

		recv = reporter->ReportEvent(data);
		reporter->Close();
	}
	delete reporter;

	delete logData;
}
