
/**
 * @brief レジストリキー。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CRegistryKey
{
protected:
	static const char delimiter;
public:
	static CString GetSubKeyPath(CString fullPath);

protected:
	HKEY key;

public:
	virtual ~CRegistryKey();

	BOOL IsValid(void);
	CRegistryKey * OpenForRead(CString subKey);
	CRegistryKey * OpenForWrite(CString subKey);
	CRegistryKey * CreateSubKeyForWrite(CString subKeyName);

	CRegistryValue * GetValue(CString name);

	CString * GetStringValue(CString name);
	CStringArray * GetStringArray(CString name);
	DWORD * GetDWordValue(CString name);
	BOOL SetStringValue(CString name, CString value);
	BOOL SetStringArray(CString name, CStringArray * values);
	BOOL SetDWordValue(CString name, DWORD value);

	BOOL DeleteKey(CString keyName);
	BOOL DeleteValue(CString name);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief キーHKEY_CLASSES_ROOT。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CRegistryClassesRootKey
	: public CRegistryKey
{
public:
	CRegistryClassesRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief キーHKEY_CURRENT_USER。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CRegistryCurrentUserRootKey
	: public CRegistryKey
{
public:
	CRegistryCurrentUserRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief キーHKEY_LOCAL_MACHINE。
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CRegistryLocalMachineRootKey
	: public CRegistryKey
{
public:
	CRegistryLocalMachineRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ルートキー。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CRegistryRootKey
	: public CRegistryKey
{
private:
	static const struct RootKeyNameAndValue rootKeyNameAndValue [];

public:
	CRegistryRootKey(CString path);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief サブキー。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
class CRegistrySubKey
	: public CRegistryKey
{
public:
	CRegistrySubKey(HKEY key);
	~CRegistrySubKey();
};
