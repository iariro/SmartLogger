#include "Stdafx.h"
// main
#include "NsvStringArray.h"

/**
 * @brief �f���~�^�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
const char CNsvStringArray::delimiter = '\0';

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�o�C�g��`���̃f�[�^��ϊ���
 * CStringArray �̗v�f�Ƃ��Ď������g�ɒǉ����Ă����B
 *
 * @author kumagai
 *
 * @param buffer �o�b�t�@�ւ̃|�C���^
 * @param size �o�b�t�@�T�C�Y
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CNsvStringArray::CNsvStringArray(TCHAR * buffer, UINT size)
{
	UINT offset;
	CString * name;

	for(offset=0 ; offset<size && buffer[offset] != delimiter ; offset++)
	{
		// �f���~�^�� NULL ('0') �𗘗p���ĂP�g�[�N������ CString �I�u�W�F�N�g
		// ���B
		name = new CString(buffer + offset);
		Add(*name);

		// Add() �ł͎��̃R�s�[���s���邽�߁Aname �͂����Ŕj���B
		delete name;

		// �e������̌�̂P�� NULL �����܂� offset ���X�L�b�v�B
		for( ; buffer[offset] != delimiter ; offset++)
		{
		}
	}
}
