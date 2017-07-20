
/**
 * @brief NULL 文字区切りの文字列コレクションから生成可能な CStringArray。
 * ::RegQueryValueEx() による REG_MULTI_SZ 文字列コレクションの取得、
 * ::GetPrivateProfileString() によるセクションやキーの列挙などで得られる
 * データの処理に有効。
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:新規作成。
 */
class AFX_EXT_CLASS CNsvStringArray
	: public CStringArray
{
private:
	static const char delimiter;

public:
	CNsvStringArray(TCHAR * buffer, UINT size);
};
