// SmartLogger.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "SmartLogger.h"

#include "ServiceInstall.h"
#include "SmartWatcherService.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/SmartLoggerLogData.h"
#include "Log/SmartLoggerLogging.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �B��̃A�v���P�[�V���� �I�u�W�F�N�g�ł��B

CWinApp theApp;

using namespace std;

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
		nRetFunc = DispatchWcpa();
	}

	return nRetFunc;
}
