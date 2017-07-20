
/**
 * @brief ������z��
 *
 * @author kumagai
 */
class CCharPtrArray
{
private:
	TCHAR ** pointers;
	int size;

public:
	CCharPtrArray(void);
	~CCharPtrArray(void);

	void Add(TCHAR * pointer);
	int GetSize(void);
	TCHAR * GetAt(int index);
	TCHAR ** GetPointers(void);
};
