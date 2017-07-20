#include "StdAfx.h"
#include "StrArray.h"

/**
 * @brief  オブジェクトの構築とともに実データの初期化を行う。
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - 新規作成
 */
CCharPtrArray::CCharPtrArray(void)
	: size(0)
{
	pointers = new TCHAR * [size];
}

/**
 * @brief  オブジェクトの破棄とともに実データの後始末を行う。
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - 新規作成
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
 * @brief  文字列値を追加
 *
 * @author kumagai
 *
 * @param  pointer 追加する文字列
 *
 * @date   2006/04/18
 *           - 新規作成
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
 * @brief 内容の個数を取得
 *
 * @author kumagai
 *
 * @return 内容の個数
 *
 * @date   2006/04/18
 *           - 新規作成
 */
int CCharPtrArray::GetSize(void)
{
	return size;
}

/**
 * @brief  文字列のポインタを取得
 *
 * @author kumagai
 *
 * @param  index 取得する要素のインデックス値
 *
 * @return 内容１つ
 *
 * @date   2006/04/18
 *           - 新規作成
 */
TCHAR * CCharPtrArray::GetAt(int index)
{
	return pointers[index];
}

/**
 * @brief  文字列ポインタの配列を返却
 *
 * @author kumagai
 *
 * @date   2006/04/18
 *           - 新規作成
 */
TCHAR ** CCharPtrArray::GetPointers(void)
{
	return pointers;
}
