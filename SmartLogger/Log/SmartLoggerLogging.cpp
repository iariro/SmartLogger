#include "StdAfx.h"
#include "Str.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerLogging.h"
#include "EventReporter.h"
#include "SmartLoggerReportEventData.h"

TCHAR * CSmartLoggerLogging::source = _T("SMART Logger");

/**
 * @brief  ログ情報出力
 * logData は本関数内で解放するため、呼び出し側での解放の必要なし。
 *
 * @author kumagai
 *
 * @param  logData ログデータ
 */
BOOL CSmartLoggerLogging::PutLog(CReportEventData * data)
{
	BOOL recv = FALSE;
	CEventReporter reporter(source);

	if (reporter.Open())
	{
		recv = reporter.ReportEvent(data);
		reporter.Close();
	}

	return recv;
}
