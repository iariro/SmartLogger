#include "StdAfx.h"
#include "StrArray.h"

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��Ɏ��f�[�^�̏��������s���B
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
CCharPtrArray::CCharPtrArray(void)
	: size(0)
{
	pointers = new TCHAR * [size];
}

/**
 * @brief  �I�u�W�F�N�g�̔j���ƂƂ��Ɏ��f�[�^�̌�n�����s���B
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
CCharPtrArray::~CCharPtrArray(void)
{
	int i;

	for(i=0 ; i<size ; i++)
	{
		delete [] pointers[i];
	}

	delete [] pointers;
}

/**
 * @brief  ������l��ǉ�
 *
 * @author kumagai
 *
 * @param  pointer �ǉ����镶����
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
void CCharPtrArray::Add(TCHAR * pointer)
{
	int i;
	TCHAR ** newPointers;

	newPointers = new TCHAR * [size + 1];
	for(i=0 ; i<size ; i++)
	{
		newPointers[i] = pointers[i];
	}
	newPointers[size] = new TCHAR [::wcslen(pointer) + 1];
	::wcscpy(newPointers[size], pointer);

	delete [] pointers;
	pointers = newPointers;

	size++;
}

/**
 * @brief ���e�̌����擾
 *
 * @author kumagai
 *
 * @return ���e�̌�
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
int CCharPtrArray::GetSize(void)
{
	return size;
}

/**
 * @brief  ������̃|�C���^���擾
 *
 * @author kumagai
 *
 * @param  index �擾����v�f�̃C���f�b�N�X�l
 *
 * @return ���e�P��
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
TCHAR * CCharPtrArray::GetAt(int index)
{
	return pointers[index];
}

/**
 * @brief  ������|�C���^�̔z���ԋp
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
TCHAR ** CCharPtrArray::GetPointers(void)
{
	return pointers;
}
