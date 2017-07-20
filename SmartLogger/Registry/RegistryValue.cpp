#include "StdAfx.h"
#include "../Utility/BinaryBlock.h"
#include "RegistryValue.h"

/**
 * @brief オブジェクトの構築とともに、レジストリの値をバイナリデータとして取得・
 * 保持する。
 *
 * @author kumagai
 *
 * @param key レジストリキーハンドル
 * @param name レジストリの値の名前
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
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
			// 取得成功。

			success = TRUE;
			loop = FALSE;
		}
		else
		{
			// 取得失敗。

			if(recv != ERROR_MORE_DATA)
			{
				// バッファ不足でもない。

				loop = FALSE;
			}

			delete [] data;
			data = NULL;
		}
	}
	while(loop);
}
