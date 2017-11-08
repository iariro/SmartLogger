// SmartLogger.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "winsvc.h"
#include "SmartLogger.h"

#include "ServiceInstall.h"
#include "SmartLoggerService.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/ReportEventData.h"
#include "Log/SmartLoggerLogging.h"
#include "Log/SmartLoggerReportEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �B��̃A�v���P�[�V���� �I�u�W�F�N�g�ł��B

CWinApp theApp;

using namespace std;

/*
 * �O���[�o���ϐ��̒�`
 */
VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
HANDLE g_hEventTerminate;	/**< �I���V�O�i����M�p				*/
HANDLE g_hThreadService;	/**< �T�[�r�X���C���X���b�h�쐬�p	*/
DWORD g_dwCurrentState;	/**< ���݂̃T�[�r�X���	*/
SERVICE_STATUS_HANDLE g_statusService;			/**< �T�[�r�X�X�e�[�^�X�擾�p		*/

/**
 * @brief	SCM ����Ăяo���ꂽ�ꍇ�ASMARTLogger�T�[�r�X���J�n������
 *
 * SCM �ȊO����̌Ăяo���ł͎��s����
 *
 * @param	�Ȃ�
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^����
 * @retval	0�ȊO	�G���[
 *            - 1:�T�[�r�X���C���֐��̌Ăяo���Ɏ��s
 */
int DispatchService(void)
{
	/*
	 * �ϐ���`
	 */
	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l

	// �T�[�r�X�e�[�u���̒�`
	SERVICE_TABLE_ENTRY	service_table[] = {
		{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};


	/*
	 * �ϐ�������
	 */

	g_hEventTerminate	= NULL;
	g_hThreadService	= NULL;


	/*
	 * �T�[�r�X�f�B�X�p�b�`�̎��s
	 *   �T�[�r�X�e�[�u���ɒ�`���� ServiceMain �֐��̌Ăяo�����s����
	 */
	fRetFunc = StartServiceCtrlDispatcher(service_table);
	if( fRetFunc == FALSE )
	{
		// �f�B�X�p�b�`�Ɏ��s(�T�[�r�X����Ăяo����Ȃ��ꍇ�͎��s����)
		return 1;
	}

	// ����ɏ����I��
	return 0;
}

/**
 * @brief	�T�[�r�X���C���֐�
 * @param	argc		[in] ������
 * @param	argv		[in] �����z��
 * @return	�Ȃ�
 */
VOID WINAPI ServiceMain(DWORD argc,	// ������
						LPTSTR *argv)	// �����z��
{
	/*
	 * �ϐ���`
	 */

	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l
	DWORD	dwIdService;		// �T�[�r�X�{�̃X���b�h ID

	/*
	 * �T�[�r�X�R���g���[���n���h���֐��̓o�^
	 */
	g_statusService = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if( g_statusService == NULL )
	{
		// �T�[�r�X�R���g���[���n���h���֐��̓o�^���s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�R���g���[���n���h���֐��̓o�^")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		return;
	}


	/*
	 * �T�[�r�X�I���V�O�i���C�x���g�̐���
	 */
	g_hEventTerminate = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( !g_hEventTerminate )
	{
		// ���s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�I���V�O�i���C�x���g�̐���")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		return;
	}


	/*
	 * �T�[�r�X�{�̕����̎��s - �T�[�r�X�{�̃X���b�h���쐬
	 */
	g_hThreadService = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)smartLogger_server, NULL, NULL, &dwIdService);
	if( g_hThreadService == NULL )
	{
		// �X���b�h�쐬�Ɏ��s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�{�̃X���b�h�̐���")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_RUNNING
	fRetFunc = SendStatusToSCM(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		if( g_hThreadService )
		{
			CloseHandle(g_hEventTerminate);
		}
		return;
	}

	/*
	 * �I���V�O�i���̑҂���
	 */
	WaitForSingleObject(g_hEventTerminate, INFINITE);

	// �I���V�O�i�������m�����ꍇ�A�V�O�i����M�p�n���h�����N���[�Y���ďI��
	if( g_hEventTerminate )
	{
		CloseHandle(g_hEventTerminate);
	}

	// SCM �� SERVICE_STOPPED ��ʒm����B
	// ServiceCtrlHandler �ł͂Ȃ��A�I���V�O�i�����󂯎�������ɒʒm����
	SendStatusToSCM(SERVICE_STOPPED, NO_ERROR, 0, 0, 100);

	return;
}

/**
 * @brief	�T�[�r�X�R���g���[���n���h��
 * @param	fdwControl	[in] �v������������R�[�h
 * @return	�Ȃ�
 */
VOID WINAPI ServiceCtrlHandler(DWORD fdwControl		// �v������������R�[�h
							   )
{
	/*
	 * �ϐ���`
	 */
	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l

	/*
	 * �T�[�r�X�R���g���[���R�[�h��M���̊e������`��
	 */
	switch( fdwControl )
	{
	/*
	 * �T�[�r�X��~�v����M
	 *   �i��~�����V���b�g�_�E�����j
	 */
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:

		/*
		 * SCM �֏󋵒ʒm -  SERVICE_STOP_PENDING
		 */
		fRetFunc = SendStatusToSCM(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 100);
		if( fRetFunc == FALSE )
		{
			// �ʒm�ɐ������Ȃ������ꍇ�͓��ɉ������Ȃ�
			break;
		}

		/*
		 * �T�[�r�X�{�̃X���b�h���N���[�Y
		 */
		if( g_hThreadService )
		{
			CloseHandle(g_hThreadService);
			g_hThreadService = NULL;
		}

		// SMARTLOGGER����~�������Ƃ��������O���o��
		CSmartLoggerLogging::PutLog(&CStopServiceEvent());

		/*
		 * �I���V�O�i���̒ʒm
		 */
		SetEvent(g_hEventTerminate);
		return;

	/*
	 * ��Ԃ̍X�V
	 */
	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}


	/*
	 * SCM �֏󋵂�ʒm
	 *   ���̃v���O�����ł́A���� SERVICE_CONTROL_INTERROGATE ( + default ) �̂��߂����ɂ���
	 *   �ʒm���s�������ɏ������s��Ȃ�
	 */
	fRetFunc = SendStatusToSCM(g_dwCurrentState, NO_ERROR, 0, 0, 0);
	return;
}


/**
 * @brief	�T�[�r�X�󋵂� SCM �ɒʒm����
 * @param	dwCurrentState				[in] �T�[�r�X�X�e�[�^�X
 * @param	dwWin32ExitCode				[in] �G���[�R�[�h
 * @param	dwServiceSpecificExitCode	[in] �T�[�r�X��`�G���[�R�[�h
 * @param	dwCheckPoint				[in] �T�[�r�X�i���w�W
 * @param	dwWaitHint					[in] �ҋ@����(ms)
 * @return	�����̐����E���s
 * @retval	TRUE	��������
 * @retval	FALSE	�����Ɏ��s
 */
BOOL SendStatusToSCM(DWORD	dwCurrentState,				// �T�[�r�X�X�e�[�^�X
					 DWORD	dwWin32ExitCode,			// �G���[�R�[�h
					 DWORD	dwServiceSpecificExitCode,	// �T�[�r�X��`�G���[�R�[�h
					 DWORD	dwCheckPoint,				// �T�[�r�X�i���w�W
					 DWORD	dwWaitHint					// �ҋ@����(ms)
					 )
{
	/*
	 * �ϐ���`
	 */
	SERVICE_STATUS	status;		// �T�[�r�X�X�e�[�^�X�\����
	BOOL			fRetFunc;	// �Ăяo���֐��̖߂�l

	/*
	 * �ϐ�������
	 */
	status.dwServiceType	= SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState	= dwCurrentState;
	status.dwCheckPoint		= dwCheckPoint;
	status.dwWaitHint		= dwWaitHint;

	/*
	 * �T�[�r�X�󋵂ɂ��AdwControlsAccepted ��ݒ肷��
	 *
	 *   �ESERVICE_START_PENDING(�����̏��������s��)�̏ꍇ�A
	 *       �R���g���[���C�x���g�͎󂯎��Ȃ�
	 *   �E����ȊO
	 *       �T�[�r�X��~(+�T�[�r�X�J�n)�̂ݒʒm���󂯎��
	 */
	status.dwControlsAccepted = (dwCurrentState == SERVICE_START_PENDING) ?
		0 : (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);

	/*
	 * �T�[�r�X��`�G���[���w�肳�ꂽ�ꍇ�͂���𗘗p����
	 */
	status.dwWin32ExitCode = (dwServiceSpecificExitCode == 0) ?
		dwWin32ExitCode : ERROR_SERVICE_SPECIFIC_ERROR;


	/*
	 * ��ԃ��R�[�h�� SCM �֓n��
	 */
	fRetFunc = SetServiceStatus(g_statusService, &status);
	if( fRetFunc == FALSE )
	{
		// �T�[�r�X��Ԃ̐ݒ�Ɏ��s
		// ���O���o��
		CSmartLoggerLogging::PutLog(&CSendStatusToSCMErrorEvent(dwCurrentState, dwCheckPoint));
		return FALSE;
	}

	g_dwCurrentState = dwCurrentState;

	/*
	 * ����I��
	 */
	return TRUE;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// MFC �����������āA�G���[�̏ꍇ�͌��ʂ�������܂��B
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: �K�v�ɉ����ăG���[ �R�[�h��ύX���Ă��������B
		_tprintf(_T("�v���I�ȃG���[: MFC �̏��������ł��܂���ł����B\n"));
		return 1;
	}

	int			nRetFunc;				// �Ăяo���֐��̖߂�l
	
	if (argc > 1)
	{
		if( wcscmp(argv[1], OPTION_INSTALL) == 0 )
		{
			//
			// SCM �ւ̓o�^
			//
			puts("/install ���Ăяo����܂���");

			// �o�^���̃R�}���h���C���I�v�V�����ɂ̓T�[�r�X�̃p�X���K�v�ƂȂ�
			// �w�肳�ꂽ�t�@�C���̐������i���݂��邩�Ȃǁj�̓`�F�b�N���Ȃ�
			if(argc <= 2 )
			{
				puts("/install �I�v�V�����Ƃ��ĕs���i���s�t�@�C���p�X���Ȃ��j");
				return 1;
			}

			// �o�^�����֐��̌Ăяo��
			puts("�T�[�r�X�C���X�g�[�����s");
			nRetFunc = CServiceInstall::install_service(argv[2]);
			printf("install_service=%d\n", nRetFunc);
		}
		else if( wcscmp(argv[1], OPTION_UNINSTALL) == 0 )
		{
			//
			// SCM ����o�^����
			//

			puts("�T�[�r�X�A���C���X�g�[�����s");

			// �A���C���X�g�[���O�ɃT�[�r�X���~����
			CServiceInstall::control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);

			// �A���C���X�g�[�����s
			return CServiceInstall::uninstall_service();
		}
		else if( wcscmp(argv[1], OPTION_START) == 0)
		{
			//
			// �T�[�r�X�J�n
			//

			puts("/start ���Ăяo����܂���");
			return CServiceInstall::control_service(SMARTLOGGER_CONTROL_START, SERVICE_NAME);
		}
		else if(wcscmp(argv[1], OPTION_STOP) == 0)
		{
			//
			// �T�[�r�X��~
			//

			puts("/stop ���Ăяo����܂���");
			nRetFunc = CServiceInstall::control_service(SMARTLOGGER_CONTROL_STOP, SERVICE_NAME);
			printf("ControlWcpaService(SMARTLOGGER_CONTROL_STOP) ret = %d", nRetFunc);

			return nRetFunc;
		}
	}
	else
	{
		//
		// SCM ����Ăяo�����s����Ɛ���Ɏ��s�����
		//
		nRetFunc = DispatchService();
	}

	return nRetFunc;
}
