#include "StdAfx.h"
#include "Str.h"
#include "StrArray.h"
#include "SmartLoggerLogData.h"
#include "SmartLoggerLogging.h"
#include "ReportEventData.h"
#include "EventReporter.h"
#include "SmartLoggerReportEventData.h"


/**
 * @brief  ���O���o��
 * logData �͖{�֐����ŉ�����邽�߁A�Ăяo�����ł̉���̕K�v�Ȃ��B
 *
 * @author kumagai
 *
 * @param  logData ���O�f�[�^
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
