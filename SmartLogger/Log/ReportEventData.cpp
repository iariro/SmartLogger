#include "StdAfx.h"
#include "StrArray.h"
#include "ReportEventData.h"

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ƀt�B�[���h�ւ̒l�̊��蓖�Ă��s���B
 * �i�����̏��Ȃ��ȈՔŁj
 *
 * @author kumagai
 *
 * @param  type     �C�x���g�̎��
 * @param  category �C�x���g�̕���
 * @param  eventId  �C�x���g���ʎq
 */
CReportEventData::CReportEventData(WORD type, WORD category, DWORD eventId)
	:	type(type), category(0), eventId(eventId), userSid(0),
		binary(NULL), binarySize(0)
{
	strings = new CCharPtrArray();
}

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ƀt�B�[���h�ւ̒l�̊��蓖�Ă��s���B
 *
 * ������f�[�^�� strings �����o�ւ̒��ڃA�N�Z�X�ɂ��s���B
 *
 * @author kumagai
 *
 * @param  type       �C�x���g�̎��
 * @param  category   �C�x���g�̕���
 * @param  eventId    �C�x���g���ʎq
 * @param  userSid    ���[�U�[�Z�L�����e�B���ʎq
 * @param  binary     �o�C�i���f�[�^�̃A�h���X
 * @param  binarySize �o�C�i���f�[�^�̃T�C�Y�i�o�C�g���j
 */
CReportEventData::CReportEventData(WORD type, WORD category, DWORD eventId,
	PSID userSid, BYTE * binary, DWORD binarySize)
	:	type(type), category(0), eventId(eventId), userSid(0),
		binary(binary), binarySize(binarySize)
{
	strings = new CCharPtrArray();
}

/**
 * @brief  �I�u�W�F�N�g�̔j���ƂƂ��ɏW�񂵂Ă��� strings �̉�����s���B
 *
 * @author kumagai
 */
CReportEventData::~CReportEventData()
{
	delete strings;
}
