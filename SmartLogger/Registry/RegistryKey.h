
/**
 * @brief ���W�X�g���L�[�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
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
 * @brief �L�[HKEY_CLASSES_ROOT�B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
class AFX_EXT_CLASS CRegistryClassesRootKey
	: public CRegistryKey
{
public:
	CRegistryClassesRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �L�[HKEY_CURRENT_USER�B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
class AFX_EXT_CLASS CRegistryCurrentUserRootKey
	: public CRegistryKey
{
public:
	CRegistryCurrentUserRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �L�[HKEY_LOCAL_MACHINE�B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
class AFX_EXT_CLASS CRegistryLocalMachineRootKey
	: public CRegistryKey
{
public:
	CRegistryLocalMachineRootKey(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ���[�g�L�[�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
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
 * @brief �T�u�L�[�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
class CRegistrySubKey
	: public CRegistryKey
{
public:
	CRegistrySubKey(HKEY key);
	~CRegistrySubKey();
};
