
/**
 * @brief •¶Žš—ñ
 *
 * @author kumagai
 */
class CStr
{
private:
	TCHAR * string;

public:
	CStr(void);
	CStr(const TCHAR * string);
	~CStr(void);

	void Add(TCHAR * value);
	void AddChar(char value);
	TCHAR * GetCharPtr(void);
};
