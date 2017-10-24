#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"
#include "SmartLoggerReportEventData.h"
#include "Str.h"
#include "../../SmartLoggerMessage/Message/SmartLoggerMessage.h"

/**
 * @brief
 * �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏������C���b�Z�[�W�̑g�ݗ��Ă��s���B
 *
 * @param fluentCatPath fluent-cat.exe�̃p�X
 * @param logFilePath SMART�t�@�C���o�̓p�X
 * @param fluentdSensorName fluentd�Z���T�[��
 * @param fluentdHostName fluentd�z�X�g��
 * @param smartWatchInterval SMART�擾�C���^�[�o��
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
 * �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏������C���b�Z�[�W�̑g�ݗ��Ă��s���B
 *
 * @author kumagai
 */
CStopServiceEvent::CStopServiceEvent(void)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_STOP_SERVICE)
{
	// �������Ȃ�
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param  info �G���[�������̏������e
 */
CStartServiceErrorEvent::CStartServiceErrorEvent(TCHAR * info)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_START_SERVICE_ERROR)
{
	strings = new CCharPtrArray();
	strings->Add(info);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 */
CGetSmartErrorEvent::CGetSmartErrorEvent(DWORD readIdentifyError, DWORD readSmartError, DWORD readThresholdError, DWORD readLogError)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_GET_SMART_ERROR)
{
	TCHAR readIdentifyErrorString [BUFSIZ];
	TCHAR readSmartErrorString [BUFSIZ];
	TCHAR readThresholdErrorString [BUFSIZ];
	TCHAR readLogErrorString [BUFSIZ];

	wsprintf(readIdentifyErrorString, _T("%d"), readIdentifyError);
	wsprintf(readSmartErrorString, _T("%d"), readSmartError);
	wsprintf(readThresholdErrorString, _T("%d"), readThresholdError);
	wsprintf(readLogErrorString, _T("%d"), readLogError);

	strings = new CCharPtrArray();
	strings->Add(readIdentifyErrorString);
	strings->Add(readSmartErrorString);
	strings->Add(readThresholdErrorString);
	strings->Add(readLogErrorString);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param path �t�@�C���p�X
 */
CFileWriteErrorEvent::CFileWriteErrorEvent(CString path)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_FILE_WRITE_ERROR)
{
	strings = new CCharPtrArray();
	strings->Add(path.GetBuffer());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param commandLine fluent-cat�Ăяo���R�}���h���C��
 */
CSendtoFluentdEvent1::CSendtoFluentdEvent1(CString commandLine)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_SENDTO_FLUENTD_EVENT1)
{
	strings->Add(commandLine.GetBuffer());
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param error �G���[�R�[�h
 * @param json ���MJSON������
 */
CSendtoFluentdEvent::CSendtoFluentdEvent(int error, char * json)
	: CReportEventData(EVENTLOG_INFORMATION_TYPE, 0, EVENT_SENDTO_FLUENTD_EVENT)
{
	TCHAR errorString [BUFSIZ];

	// �X�e�[�^�X�A�i���w�W�𐔒l���當����ɕϊ�
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
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param status �G���[�������̒ʒm�X�e�[�^�X
 * @param checkPoint �G���[�������̃X�e�[�^�X�i���w�W
 */
CSendStatusToSCMErrorEvent::CSendStatusToSCMErrorEvent(DWORD status, DWORD checkPoint)
	: CReportEventData(EVENTLOG_ERROR_TYPE, 0, EVENT_SENDSTATUSTOSCM_ERROR)
{
	TCHAR szIntegerStatus[32], szIntegerCheckPoint[32];

	// �X�e�[�^�X�A�i���w�W�𐔒l���當����ɕϊ�
	wsprintf(szIntegerStatus, _T("%d"), status);
	wsprintf(szIntegerCheckPoint, _T("%d"), checkPoint);

	strings = new CCharPtrArray();
	strings->Add(szIntegerStatus);
	strings->Add(szIntegerCheckPoint);
}
