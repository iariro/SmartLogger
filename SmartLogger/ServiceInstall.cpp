#include "StdAfx.h"
#include "Stdlib.h"
#include "windows.h"
#include "ServiceInstall.h"
#include "winsvc.h"

/**
 * @brief SMARTLOGGER�T�[�r�X�̃C���X�g�[�����s��
 *
 * @param	pModulePath		[in] �T�[�r�X���W���[���̎��s�p�X
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^����
 * @retval	0�ȊO	�G���[
 *            -  1:�ړI���W�X�g���L�[�̃I�[�v���Ɏ��s
 *            -  2:�ړI���W�X�g���L�[�̃f�[�^�擾�Ɏ��s
 *            -  3:���W�X�g���f�[�^�̏��������Ɏ��s
 *            - 11:pServiceName ���s��(���t�@�C�����̒������x�����m�F���܂���)
 *            - 12:SMC �n���h���̃I�[�v���Ɏ��s
 *            - 13:�T�[�r�X�̒ǉ��Ɏ��s
 *            - 14:�T�[�r�X���̐��ĂɎ��s
 */
int CServiceInstall::install_service(TCHAR * pModulePath)
{
	int ret;

	// SCM �ւ̃T�[�r�X�o�^
	ret = register_service(pModulePath);
	if( ret != 0 )
	{
		// �T�[�r�X�o�^�Ɏ��s
		return (ret + 10);
	}
	return 0;
}

/**
 * @brief SMARTLOGGER�T�[�r�X�̃A���C���X�g�[�����s��
 *
 * �T�[�r�X�N�����ݒ�̉����i���W�X�g������j�A�T�[�r�X�o�^�������s���B
 *
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^����
 * @retval	0�ȊO	�G���[
 *            - 11:SCM �n���h���̃I�[�v���Ɏ��s
 *            - 12:�T�[�r�X�n���h���̃I�[�v���Ɏ��s
 *            - 13:�T�[�r�X�̓o�^�����Ɏ��s
 */
int CServiceInstall::uninstall_service(void)
{
	int		ret;

	// SCM �ւ̃T�[�r�X�o�^����
	ret = unregister_service();
	if( ret != 0 )
	{
		// �T�[�r�X�o�^�Ɏ��s
		return (ret + 10);
	}
	return 0;
}

/**
 * @brief	�T�[�r�X�� SCM �֓o�^����
 * @param	pModulePath		[in] �T�[�r�X���W���[���̎��s�p�X
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^����
 * @retval	0�ȊO	�G���[
 *            - 1:pServiceName ���s��(���t�@�C�����̒������x�����m�F���܂���)
 *            - 2:SMC �n���h���̃I�[�v���Ɏ��s
 *            - 3:�T�[�r�X�̒ǉ��Ɏ��s
 *            - 4:�T�[�r�X���̐��ĂɎ��s
 */
int CServiceInstall::register_service(TCHAR * pModulePath)
{
	/*
	 * �ϐ���`
	 */

	SC_HANDLE		hSCM;			// SCM �n���h��
	SC_HANDLE		hService;		// �T�[�r�X�n���h��
	BOOL			fRetFunc;		// �Ăяo���֐��̖߂�l

	// �C���X�g�[���^�C�v�̒�`
	DWORD			DesiredAccess;	// �T�[�r�X�̃A�N�Z�X���̃^�C�v
	DWORD			ServiceType;	// �T�[�r�X�^�C�v
	DWORD			StartType;		// �T�[�r�X�J�n�̃^�C�v
	DWORD			ErrorControl;	// ��Q�����������ꍇ�ɊJ�n�v���O���������{����ׂ�����

	// �o�^�T�[�r�X���
	SERVICE_DESCRIPTION		sd = {	// �T�[�r�X����
		SRV_DESC_INFO
	};



	/*
	 * �ϐ�������
	 */

	// �C���X�g�[���^�C�v�̐ݒ�
	DesiredAccess	= SERVICE_CHANGE_CONFIG;		// �T�[�r�X�\���̕ύX����
	ServiceType		= SERVICE_WIN32_OWN_PROCESS;	// ��p�̃v���Z�X���œ���
	StartType		= SERVICE_AUTO_START;			// �X�^�[�g�A�b�v�̎�ނ��u�����v��
	ErrorControl	= SERVICE_ERROR_IGNORE;			// �G���[�����J�n����𑱍s����

	// �T�[�r�X�̊J�n�R�}���h�̍쐬
	if( (wcslen(pModulePath) > _MAX_PATH) || (wcslen(pModulePath) == 0 ) )
	{
		// ���炩�ɃT�[�r�X���W���[���p�X���s���Ɣ��f
		return 1;
	}

	/*
	 * SCM �n���h���̃I�[�v��
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if( hSCM == NULL )
	{
		// �I�[�v���Ɏ��s
		return 2;
	}


	/*
	 * SCM �f�[�^�x�[�X�ւ̃T�[�r�X�o�^�̎��s
	 */

	// �T�[�r�X�̒ǉ�
	hService = CreateService(hSCM, SERVICE_NAME, SERVICE_DISPLAY_NAME,
		DesiredAccess, ServiceType, StartType, ErrorControl,
		pModulePath, NULL, NULL, NULL, NULL, NULL);
	if( hService == NULL )
	{
		// �ǉ��Ɏ��s
		CloseServiceHandle(hSCM);
		return 3;
	}

	// �T�[�r�X����ύX
	fRetFunc = ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &sd);
	if( fRetFunc == FALSE )
	{
		// ���s
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService);
		return 4;
	}


	/*
	 * ����I��
	 */

	// �I������
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);

	return 0;
}

/**
 * @brief	�T�[�r�X�� SCM ����o�^��������
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^��������
 * @retval	0�ȊO	�G���[
 *            - 1:SCM �n���h���̃I�[�v���Ɏ��s
 *            - 2:�T�[�r�X�n���h���̃I�[�v���Ɏ��s
 *            - 3:�T�[�r�X�̓o�^�����Ɏ��s
 */
int CServiceInstall::unregister_service(void)
{
	/*
	 * �ϐ���`
	 */

	SC_HANDLE		hSCM;			// SCM �n���h��
	SC_HANDLE		hService;		// �T�[�r�X�n���h��
	BOOL			fRetFunc;		// �Ăяo���֐��̖߂�l


	puts("called UninstallService()");

	/*
	 * �ϐ�������
	 */

	/*
	 * �T�[�r�X���s���̏ꍇ�A��~����
	 * ����~��Ԃɂł��Ȃ��ꍇ�ł��A�ȍ~�̏����𑱍s����B
	 */
	control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);


	/*
	 * SCM �n���h���̃I�[�v��
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if( hSCM == NULL )
	{
		// �I�[�v���Ɏ��s
		return 1;
	}


	/*
	 * �T�[�r�X�n���h���̃I�[�v��
	 */
	hService = OpenService(hSCM, SERVICE_NAME, SERVICE_ALL_ACCESS);
	if( hService == NULL )
	{
		// �I�[�v���Ɏ��s
		CloseServiceHandle(hSCM);
		return 2;
	}


	/*
	 * �T�[�r�X�̓o�^����
	 */
	fRetFunc = DeleteService(hService);
	if( fRetFunc == FALSE )
	{
		// �o�^�����Ɏ��s
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService);
		return 3;
	}


	/*
	 * �I������
	 */
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService);

	/*
	 * ����I��
	 */
	return 0;
}

/**
 * @brief SMARTLOGGER�T�[�r�X�̃R���g���[���֐�
 *
 * �T�[�r�X���w�肳�ꂽ��ԂɕύX����
 * ���� dwControl �ɂ͈ȉ��̂��̂��w�肷��
 *   - SMARTLOGGER_CONTROL_START:	�T�[�r�X�J�n
 *   - SMARTLOGGER_CONTROL_STOP:	�T�[�r�X��~
 *
 * @param	dwControl	[in] ����R�[�h
 * @param	pService	[in] �T�[�r�X��
 *
 * @return	�����̐����E���s
 * @retval	0	����ɒ�~����
 * @retval	0�ȊO	�G���[
 *			  - 1:�w�肵������R�[�h���s��
 *			  - 2:SCM �n���h���̃I�[�v���Ɏ��s
 *			  - 3:�T�[�r�X�n���h���̃I�[�v���Ɏ��s
 *			  - 4:�T�[�r�X��Ԃ̎擾�Ɏ��s
 *			  - 5:�T�[�r�X��ԑ���Ɏ��s
 */
int CServiceInstall::control_service(DWORD	dwControl,
					TCHAR	*pService)
{
	/*
	 * �ϐ���`
	 */
	SC_HANDLE		hSCM;			// SCM �n���h��
	SC_HANDLE		hService;		// �T�[�r�X�n���h��
	SERVICE_STATUS	serviceStatus;	// �T�[�r�X��Ԏ擾�p
	BOOL			fRetFunc;		// �Ăяo���֐��̖߂�l

	/*
	 * �����`�F�b�N
	 */
	if( (dwControl != SMARTLOGGER_CONTROL_START) &&
		(dwControl != SMARTLOGGER_CONTROL_STOP) )
	{
		// �s���Ȉ�����^����ꂽ
		return 1;
	}

	/*
	 * SCM �n���h���̃I�[�v��
	 */
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if( hSCM == NULL )
	{
		// �I�[�v���Ɏ��s
		return 2;
	}

	/*
	 * �T�[�r�X�n���h���̃I�[�v��
	 */
	hService = OpenService(hSCM, pService, SERVICE_ALL_ACCESS);
	if( hService == NULL )
	{
		// �I�[�v���Ɏ��s
		CloseServiceHandle(hSCM);
		return 3;
	}


	/*
	 * �T�[�r�X�̎��s��Ԃ̎擾
	 */
	fRetFunc = QueryServiceStatus(hService, &serviceStatus);
	if( fRetFunc == FALSE )
	{
		// ��Ԏ擾�Ɏ��s
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return 4;
	}

	switch( dwControl )
	{

	/*
	 * �T�[�r�X�J�n
	 */

	case SMARTLOGGER_CONTROL_START:

		// ��~��ԂłȂ��ꍇ�͉������Ȃ�
		if( serviceStatus.dwCurrentState != SERVICE_STOPPED )
		{
			break;
		}

		// �T�[�r�X�̊J�n
		fRetFunc = StartService(hService, 0, NULL);
		if( fRetFunc == FALSE )
		{
			// �J�n�Ɏ��s
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return 5;
		}

		break;

	/*
	 * �T�[�r�X��~
	 */

	case SMARTLOGGER_CONTROL_STOP:

		// �͂��߂����~��Ԃ̏ꍇ�͉������Ȃ�
		if( serviceStatus.dwCurrentState == SERVICE_STOPPED )
		{
			break;
		}

		// �T�[�r�X�̒�~
		fRetFunc = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		if( fRetFunc == FALSE )
		{
			// ��~�Ɏ��s
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return 5;
		}

		break;


	/*
	 * �������s��
	 */

	default:
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return 1;
	}


	/*
	 * �I������
	 */

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCM);


	/*
	 * ����ɏ����I��
	 */

	return 0;
}
