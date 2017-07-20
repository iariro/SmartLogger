#include "Stdafx.h"
// main
#include "NsvStringArray.h"

/**
 * @brief デリミタ。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
const char CNsvStringArray::delimiter = '\0';

/**
 * @brief オブジェクトの構築とともに、バイト列形式のデータを変換し
 * CStringArray の要素として自分自身に追加していく。
 *
 * @author kumagai
 *
 * @param buffer バッファへのポインタ
 * @param size バッファサイズ
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CNsvStringArray::CNsvStringArray(TCHAR * buffer, UINT size)
{
	UINT offset;
	CString * name;

	for(offset=0 ; offset<size && buffer[offset] != delimiter ; offset++)
	{
		// デリミタの NULL ('0') を利用して１トークンごと CString オブジェクト
		// 化。
		name = new CString(buffer + offset);
		Add(*name);

		// Add() では実体コピーが行われるため、name はここで破棄。
		delete name;

		// 各文字列の後の１個の NULL 文字まで offset をスキップ。
		for( ; buffer[offset] != delimiter ; offset++)
		{
		}
	}
}
