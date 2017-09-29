#include "StdAfx.h"
#include "Str.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerLogging.h"
#include "EventReporter.h"
#include "SmartLoggerReportEventData.h"

TCHAR * CSmartLoggerLogging::source = _T("SMART Logger");

/**
 * @brief  ���O���o��
 * logData �͖{�֐����ŉ�����邽�߁A�Ăяo�����ł̉���̕K�v�Ȃ��B
 *
 * @author kumagai
 *
 * @param  logData ���O�f�[�^
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
