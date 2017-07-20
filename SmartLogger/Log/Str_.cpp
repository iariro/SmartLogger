#include "StdAfx.h"
#include "Str.h"

/**
 * @brief  オブジェクトの構築とともに内部実データの初期化を行う。
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - 新規作成
 */
CStr::CStr(void)
{
	string = new TCHAR [1];
	string[0] = '\0';
}

/**
 * @brief  オブジェクトの構築とともに内部実データの初期化を行う。
 *
 * @author kumagai
 *
 * @param  string セットする文字列
 *
 * @date   2006/04/22
 *           - 新規作成
 */
CStr::CStr(const TCHAR * string)
{
	int length = ::wcslen(string);
	this->string = new TCHAR [length + 1];
	::wcscpy_s(this->string, length, string);
}

/**
 * @brief  オブジェクトの破棄とともに内部実データの解放を行う。
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - 新規作成
 */
CStr::~CStr(void)
{
	delete [] string;
}

/**
 * @brief  文字列を連結する
 *
 * @author kumagai
 *
 * @param  value 連結する文字列
 *
 * @date   2006/04/18
 *           - 新規作成
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
 * @brief  文字を連結する
 *
 * @author kumagai
 *
 * @param  value 連結する文字
 *
 * @date   2006/04/18
 *           - 新規作成
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
 * @brief  文字列ポインタを取得
 *
 * @author kumagai
 *
 * @return 文字列ポインタ
 *
 * @date   2006/04/18
 *           - 新規作成
 */
TCHAR * CStr::GetCharPtr(void)
{
	return string;
}
