
/**
 * @brief レジストリの値。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
class CRegistryValue
	: public CBinaryBlock
{
public:
	BOOL success;
	DWORD type;

public:
	CRegistryValue(HKEY key, CString valueName);
};
