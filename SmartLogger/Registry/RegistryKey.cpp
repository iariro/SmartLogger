#include "StdAfx.h"
#include "../Utility/BinaryBlock.h"
#include "RegistryValue.h"
#include "RegistryKey.h"
#include "../Utility/NsvStringArray.h"

/**
 * @brief レジストリパスのデリミタ。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
const char CRegistryKey::delimiter = '\\';

/**
 * @brief レジストリフルパスからルート以下のサブキーパスを取得する。
 *
 * @author kumagai
 *
 * @param fullPath レジストリのフルパス
 *
 * @return サブキーパス
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CString CRegistryKey::GetSubKeyPath(CString fullPath)
{
	return fullPath.Mid(fullPath.Find(delimiter) + 1);
}

/**
 * @brief サブクラスのデストラクタ仮想関数実行のためのデストラクタ。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryKey::~CRegistryKey()
{
	// 何もしない。
}

/**
 * @brief キーは有効化を取得。
 *
 * @author kumagai
 *
 * @return TRUE=有効/FALSE=無効
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::IsValid(void)
{
	return key != NULL;
}

/**
 * @brief サブキーを読み取りモードでオープンする。
 *
 * @author kumagai
 *
 * @param subKey サブキー名
 *
 * @return サブキーオブジェクト
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryKey * CRegistryKey::OpenForRead(CString subKey)
{
	LONG recv;
	HKEY key;
	CRegistryKey * ret;

	// オープン。
	recv = ::RegOpenKeyEx(this->key, subKey, 0, KEY_READ, &key);

	ret = NULL;
	if(recv == ERROR_SUCCESS)
	{
		// オープン成功。

		ret = new CRegistrySubKey(key);
	}

	return ret;
}

/**
 * @brief サブキーを書き込みモードでオープンする。
 *
 * @author kumagai
 *
 * @param subKey サブキー名
 *
 * @return サブキーオブジェクト
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryKey * CRegistryKey::OpenForWrite(CString subKey)
{
	LONG recv;
	HKEY key;
	CRegistryKey * ret;

	// オープン。
	recv = ::RegOpenKeyEx(this->key, subKey, 0, KEY_WRITE, &key);

	ret = NULL;
	if(recv == ERROR_SUCCESS)
	{
		// オープン成功。

		ret = new CRegistrySubKey(key);
	}

	return ret;
}

/**
 * @brief サブキーを作成しオープンする。
 *
 * @author kumagai
 *
 * @param subKeyName 作成するサブキーの名前
 *
 * @return 作成したサブキー／NULL=作成失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryKey * CRegistryKey::CreateSubKeyForWrite(CString subKeyName)
{
	LONG recv;
	HKEY subKeyHandle;
	CRegistryKey * subKey;

	recv =
		::RegCreateKeyEx(
			key,
			subKeyName,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&subKeyHandle,
			NULL);

	if(recv == ERROR_SUCCESS)
	{
		// 作成成功。

		subKey = new CRegistrySubKey(subKeyHandle);
	}
	else
	{
		// 作成失敗。

		subKey = NULL;
	}

	return subKey;
}

/**
 * @brief 値をバイナリの形で取得。
 *
 * @author kumagai
 *
 * @param name 値名
 *
 * @return 値オブジェクト
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryValue * CRegistryKey::GetValue(CString name)
{
	return new CRegistryValue(key, name);
}

/**
 * @brief REG_SZ の値を取得する。
 *
 * @author kumagai
 *
 * @param name 値の名前
 *
 * @return 取得した値へのポインタ/NULL=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CString * CRegistryKey::GetStringValue(CString name)
{
	CString * ret;

	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// 成功。

		if(value.type == REG_SZ)
		{
			// 型は正しい。

			if(value.size > 0)
			{
				// １バイトでもある。

				ret = new CString((TCHAR *)value.data);
			}
			else
			{
				// ０バイト。

				ret = new CString();
			}
		}
	}

	return ret;
}

/**
 * @brief REG_MULTI_SZ の値を取得する。
 *
 * @author kumagai
 *
 * @param name 値の名前
 *
 * @return 取得した値へのポインタ/NULL=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CStringArray * CRegistryKey::GetStringArray(CString name)
{
	CStringArray * ret;
	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// 成功。

		if(value.type == REG_MULTI_SZ)
		{
			// 型は正しい。

			if(value.size > 0)
			{
				// １バイトでもある。

				ret = new CNsvStringArray((TCHAR *)value.data, value.size);
			}
			else
			{
				// ０バイト。

				ret = new CStringArray();
			}
		}
	}

	return ret;
}

/**
 * @brief REG_DWORD の値を取得する。
 *
 * @author kumagai
 *
 * @param name 値の名前
 *
 * @return 取得した値へのポインタ/NULL=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
DWORD * CRegistryKey::GetDWordValue(CString name)
{
	DWORD * ret;

	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// 成功。

		if(value.type == REG_DWORD)
		{
			// 型は正しい。

			if(value.size == sizeof(DWORD))
			{
				// バイト数は正しい。

				ret = new DWORD;
				::CopyMemory(ret, value.data, sizeof(DWORD));
			}
		}
	}

	return ret;
}

/**
 * @brief 文字列の値を書き込む。
 *
 * @author kumagai
 *
 * @param name 値の名前
 * @param value 値
 *
 * @return TRUE=成功/FALSE=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::SetStringValue(CString name, CString value)
{
	LONG recv;

	recv =
		::RegSetValueEx(
			key,
			name,
			0,
			REG_SZ,
			(BYTE *)value.GetBuffer(),
			sizeof(TCHAR) * value.GetLength());

	return recv == ERROR_SUCCESS;
}

/**
 * @brief 複数行文字列値の書き込み。
 *
 * @author kumagai
 *
 * @param name データ名
 * @param values 値
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::SetStringArray(CString name, CStringArray * values)
{
	int i, j;
	int offset;
	LONG recv;
	TCHAR * buffer;
	DWORD size;
	CString string;

	// バッファサイズ計算。
	size = 0;
	for(i=0 ; i<values->GetSize() ; i++)
	{
		string = values->GetAt(i);
		size += string.GetLength() + 1;
	}
	size += 1;

	// バッファ確保及びデータ設置。
	offset = 0;
	buffer = new TCHAR [size];
	for(i=0 ; i<values->GetSize() ; i++)
	{
		string = values->GetAt(i);

		for(j=0 ; j<string.GetLength() ; j++)
		{
			buffer[offset + j] = string[j];
		}

		offset += string.GetLength();
		buffer[offset] = '\0';
		offset++;
	}

	buffer[offset] = '\0';

	// 格納実行。
	recv =
		::RegSetValueEx
			(key, name, 0, REG_MULTI_SZ, (BYTE *)buffer, sizeof(TCHAR) * size);

	delete [] buffer;

	return recv == ERROR_SUCCESS;
}

/**
 * @brief 文字列の値を書き込む。
 *
 * @author kumagai
 *
 * @param name 値の名前
 * @param value 値
 *
 * @return TRUE=成功/FALSE=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::SetDWordValue(CString name, DWORD value)
{
	LONG recv;

	recv =
		::RegSetValueEx(
			key,
			name,
			0,
			REG_DWORD,
			(BYTE *)&value,
			sizeof(DWORD));

	return recv == ERROR_SUCCESS;
}

/**
 * @brief キーの削除。
 *
 * @author kumagai
 *
 * @param keyName キー名
 *
 * @return TRUE=成功/FALSE=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::DeleteKey(CString keyName)
{
	LONG recv;

	recv = ::RegDeleteKey(key, keyName);

	return recv == ERROR_SUCCESS;
}

/**
 * @brief 値の削除。
 *
 * @author kumagai
 *
 * @param name 値名
 *
 * @return TRUE=成功/FALSE=失敗
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
BOOL CRegistryKey::DeleteValue(CString name)
{
	LONG recv;

	recv = ::RegDeleteValue(key, name);

	return recv == ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに、キー値の割り当てを行う。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
CRegistryClassesRootKey::CRegistryClassesRootKey(void)
{
	key = HKEY_CLASSES_ROOT;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに、キー値の割り当てを行う。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
CRegistryCurrentUserRootKey::CRegistryCurrentUserRootKey(void)
{
	key = HKEY_CURRENT_USER;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに、キー値の割り当てを行う。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
CRegistryLocalMachineRootKey::CRegistryLocalMachineRootKey(void)
{
	key = HKEY_LOCAL_MACHINE;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief レジストリルートキーの名前と値の対。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
const struct RootKeyNameAndValue
{
	char * name;
	HKEY value;
}
CRegistryRootKey::rootKeyNameAndValue [] =
{
	{"HKEY_CLASSES_ROOT",			HKEY_CLASSES_ROOT		},
	{"HKEY_CURRENT_USER",			HKEY_CURRENT_USER		},
	{"HKEY_LOCAL_MACHINE",			HKEY_LOCAL_MACHINE		},
	{"HKEY_USERS",					HKEY_USERS				},
	{"HKEY_PERFORMANCE_DATA",		HKEY_PERFORMANCE_DATA	},
	{"HKEY_PERFORMANCE_TEXT",		HKEY_PERFORMANCE_TEXT	},
	{"HKEY_PERFORMANCE_NLSTEXT",	HKEY_PERFORMANCE_NLSTEXT},
	{"HKEY_CURRENT_CONFIG",			HKEY_CURRENT_CONFIG		},
	{"HKEY_DYN_DATA",				HKEY_DYN_DATA			}
};

/**
 * @brief オブジェクトの構築とともに、指定レジストリパスのルートキーのキーオブ
 * ジェクト化を行う。
 *
 * @author kumagai
 *
 * @param path レジストリパス
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistryRootKey::CRegistryRootKey(CString path)
{
	int i;
	CString root;

	if(path.Find(delimiter) >= 0)
	{
		// デリミタを含む。フルパスとみなす。

		root = path.Left(path.Find(delimiter));
	}
	else
	{
		// デリミタを含まない。ルートキーのみを表す文字列とみなす。

		root = path;
	}

	this->key = NULL;
	for(i=0 ; i<sizeof(rootKeyNameAndValue)/sizeof(RootKeyNameAndValue) ; i++)
	{
		if(root == rootKeyNameAndValue[i].name)
		{
			// ルートキー名は一致する。

			this->key = rootKeyNameAndValue[i].value;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief オブジェクトの構築とともに、指定キーハンドルの集約を行う。
 *
 * @author kumagai
 *
 * @param key レジストリキー
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistrySubKey::CRegistrySubKey(HKEY key)
{
	this->key = key;
}

/**
 * @brief オブジェクトの破棄とともに、集約していたレジストリキーのクローズを行う。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
CRegistrySubKey::~CRegistrySubKey()
{
	LONG recv;

	recv = ::RegCloseKey(key);

	ASSERT(recv == ERROR_SUCCESS);
}
