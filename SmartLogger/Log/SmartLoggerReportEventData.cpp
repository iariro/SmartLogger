#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerReportEventData.h"
#include "Str.h"

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ƀt�B�[���h�ւ̒l�̊��蓖�āC
 * �܂��f�[�^�̑g�ݗ��Ă��s��
 *
 * @param  type    �C�x���g�̎��
 * @param  eventId �C�x���g���ʎq
 * @param  code    ���O�̃R�[�h
 * @param  message ���b�Z�[�W
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
