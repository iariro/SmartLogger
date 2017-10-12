#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "EventReporter.h"

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ƀt�B�[���h�ւ̒l�̊��蓖�Ă��s��
 *
 * @author kumagai
 *
 * @param  sourceName �C�x���g���O�̃C�x���g�\�[�X�̖��O
 */
CEventReporter::CEventReporter(const TCHAR * sourceName)
	: sourceName(sourceName)
{
	// �������Ȃ��B
}

/**
 * @brief  �C�x���g���O�g�p�������s��
 *
 * @author kumagai
 *
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CEventReporter::Open(void)
{
	handle = ::RegisterEventSource(NULL, sourceName);

	return handle != NULL;
}

/**
 * @brief  �C�x���g���O�g�p�I���������s��
 *
 * @author kumagai
 *
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CEventReporter::Close(void)
{
	return ::DeregisterEventSource(handle);
}

/**
 * @brief  �C�x���g�����|�[�g����
 *
 * data �͖{�֐����ŉ�����邽�߁A�Ăяo�����ł̉���̕K�v�Ȃ��B
 *
 * @author kumagai
 *
 * @param  data �Ώۃf�[�^
 *
 * @retval TRUE ����
 * @retval FALSE ���s
 */
BOOL CEventReporter::ReportEvent(CReportEventData * data)
{
	return
		::ReportEvent(
			handle,
			data->type,
			data->category,
			data->eventId,
			data->userSid,
			(WORD)data->strings->GetSize(),
			(DWORD)data->binarySize,
			(LPCTSTR *)data->strings->GetPointers(),
			(LPVOID)data->binary);
}
