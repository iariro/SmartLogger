#include "StdAfx.h"
#include "../Utility/BinaryBlock.h"
#include "RegistryValue.h"

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA���W�X�g���̒l���o�C�i���f�[�^�Ƃ��Ď擾�E
 * �ێ�����B
 *
 * @author kumagai
 *
 * @param key ���W�X�g���L�[�n���h��
 * @param name ���W�X�g���̒l�̖��O
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryValue::CRegistryValue(HKEY key, CString name)
{
	LONG recv;
	BOOL loop;

	success = FALSE;
	loop = TRUE;
	size = 0;
	do
	{
		data = new BYTE [size];
		recv =
			::RegQueryValueEx
				(key, name, NULL, &type, data, (LPDWORD)&size);

		if(recv == ERROR_SUCCESS)
		{
			// �擾�����B

			success = TRUE;
			loop = FALSE;
		}
		else
		{
			// �擾���s�B

			if(recv != ERROR_MORE_DATA)
			{
				// �o�b�t�@�s���ł��Ȃ��B

				loop = FALSE;
			}

			delete [] data;
			data = NULL;
		}
	}
	while(loop);
}
