#include "StdAfx.h"
#include "Str.h"
#include "SmartLoggerLogdata.h"
#include "../../SmartLoggerMessage/Message/SmartLoggerMessage.h"

/**
 * @brief  �C�x���g�̃\�[�X�̖��O�B
 *
 * @author kumagai
 */
const TCHAR * CSmartLoggerLogData::sourceName = _T("SMART Logger");

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ƀt�B�[���h�ւ̒l�̊��蓖�Ă��s���B
 *
 * @author kumagai
 *
 * @param  type �C�x���g�̎��
 * @param  code ���O�R�[�h
 */
CSmartLoggerLogData::CSmartLoggerLogData(UINT type, UINT code)
	:	source(sourceName), type(type), eventId(SMARTLOGGER_MESSAGE), code(code),
		message(NULL)
{
	// �������Ȃ��B
}

/**
 * @brief  �I�u�W�F�N�g�̔j���ƂƂ��ɏW�񂵂Ă������b�Z�[�W�f�[�^�̉�����s���B
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
 * @brief  ���b�Z�[�W�f�[�^���Z�b�g�B
 *
 * @author kumagai
 *
 * @param  message ���b�Z�[�W
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
 * �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏������C���b�Z�[�W�̑g�ݗ��Ă��s���B
 *
 * @author kumagai
 */
CStartServiceLog::CStartServiceLog(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 100)
{
	CStr * str;

	str = new CStr(_T("SMART Logger�T�[�r�X�̉ғ����J�n���܂����B\r\n"));
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
 * �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏������C���b�Z�[�W�̑g�ݗ��Ă��s���B
 *
 * @author kumagai
 */
CStopServiceLog::CStopServiceLog(void)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 110)
{
	CStr * str;

	str = new CStr(_T("SMART Logger�T�[�r�X�̉ғ����~���܂����B"));

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param  info �G���[�������̏������e
 */
CStartServiceErrorLog::CStartServiceErrorLog(TCHAR * info)
	: CSmartLoggerLogData(EVENTLOG_ERROR_TYPE, 101)
{
	CStr * str;

	str = new CStr();
	str->Add(_T("�T�[�r�X�J�n�������s���Ɂu"));
	str->Add(info);
	str->Add(_T("�v�ɂăG���[���������܂����B"));

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param error �G���[�R�[�h
 */
CFileWriteErrorLog::CFileWriteErrorLog(CString path)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 130)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	wsprintf(errorString, _T("�t�@�C���u%s�v�̍쐬�Ɏ��s"), path);

	str = new CStr();
	str->Add(errorString);

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param error �G���[�R�[�h
 */
CSendtoFluentdLog1::CSendtoFluentdLog1(CString commandLine)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 131)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	// �X�e�[�^�X�A�i���w�W�𐔒l���當����ɕϊ�
	wsprintf(errorString, _T("(%s)"), commandLine);

	str = new CStr();
	str->Add(_T("fluent-cat���s"));
	str->Add(errorString);

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param error �G���[�R�[�h
 */
CSendtoFluentdLog::CSendtoFluentdLog(int error, char * json)
	: CSmartLoggerLogData(EVENTLOG_INFORMATION_TYPE, 132)
{
	CStr * str;

	TCHAR errorString [BUFSIZ];

	// �X�e�[�^�X�A�i���w�W�𐔒l���當����ɕϊ�
	wsprintf(errorString, _T("(%d)"), error);

	CStringA jsonStringA(json);
	CString jsonString;
	jsonString =jsonStringA;

	str = new CStr();
	str->Add(_T("fluent-cat���s"));
	str->Add(errorString);
	str->Add(jsonString.GetBuffer());

	SetMessage(str);
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɋ��N���X�̏��������s���B
 *
 * @param  status �G���[�������̒ʒm�X�e�[�^�X
 * @param  checkPoint �G���[�������̃X�e�[�^�X�i���w�W
 */
CSendStatusToSCMErrorLog::CSendStatusToSCMErrorLog(DWORD status, DWORD checkPoint)
	: CSmartLoggerLogData(EVENTLOG_ERROR_TYPE, 140)
{
	CStr * str;
	TCHAR szIntegerStatus[32], szIntegerCheckPoint[32];

	// �X�e�[�^�X�A�i���w�W�𐔒l���當����ɕϊ�
	wsprintf(szIntegerStatus, _T("%d"), status);
	wsprintf(szIntegerCheckPoint, _T("%d"), checkPoint);

	str = new CStr();
	str->Add(_T("SCM�ւ̃T�[�r�X�󋵁i�X�e�[�^�X["));
	str->Add(szIntegerStatus);
	str->Add(_T("], �i���w�W["));
	str->Add(szIntegerCheckPoint);
	str->Add(_T("]�j�̒ʒm�Ɏ��s���܂����B"));
	SetMessage(str);
}
