// SmartLoggerMessage.h : SmartLoggerMessage.DLL �̃��C�� �w�b�_�[ �t�@�C��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��


// CSmartLoggerMessageApp
// ���̃N���X�̎����Ɋւ��Ă� SmartLoggerMessage.cpp ���Q�Ƃ��Ă��������B
//

class CSmartLoggerMessageApp : public CWinApp
{
public:
	CSmartLoggerMessageApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
