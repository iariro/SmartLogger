#include "StdAfx.h"
#include "Str.h"

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��ɓ������f�[�^�̏��������s���B
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
CStr::CStr(void)
{
	string = new TCHAR [1];
	string[0] = '\0';
}

/**
 * @brief  �I�u�W�F�N�g�̍\�z�ƂƂ��ɓ������f�[�^�̏��������s���B
 *
 * @author kumagai
 *
 * @param  string �Z�b�g���镶����
 *
 * @date   2006/04/22
 *           - �V�K�쐬
 */
CStr::CStr(const TCHAR * string)
{
	int length = ::wcslen(string);
	this->string = new TCHAR [length + 1];
	::wcscpy_s(this->string, length, string);
}

/**
 * @brief  �I�u�W�F�N�g�̔j���ƂƂ��ɓ������f�[�^�̉�����s���B
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
CStr::~CStr(void)
{
	delete [] string;
}

/**
 * @brief  �������A������
 *
 * @author kumagai
 *
 * @param  value �A�����镶����
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
void CStr::Add(TCHAR * value)
{
	size_t oldSize, newSize;
	TCHAR * newString;

	oldSize = ::wcslen(string);
	newSize = oldSize + ::wcslen(value) + 1;
	newString = new TCHAR [newSize];

	::wcscpy_s(newString, oldSize, string);
	::wcscpy_s(newString + oldSize, newSize - oldSize - 1, value);

	delete [] string;
	string = newString;
}

/**
 * @brief  ������A������
 *
 * @author kumagai
 *
 * @param  value �A�����镶��
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
void CStr::AddChar(char value)
{
	size_t oldSize, newSize;
	TCHAR * newString;

	oldSize = ::wcslen(string);
	newSize = oldSize + 1 + 1;
	newString = new TCHAR [newSize];

	::wcscpy_s(newString, oldSize, string);
	newString[oldSize] = value;
	newString[oldSize + 1] = '\0';

	delete [] string;
	string = newString;
}

/**
 * @brief  ������|�C���^���擾
 *
 * @author kumagai
 *
 * @return ������|�C���^
 *
 * @date   2006/04/18
 *           - �V�K�쐬
 */
TCHAR * CStr::GetCharPtr(void)
{
	return string;
}
