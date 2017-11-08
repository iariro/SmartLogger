
/**
 * @brief size info of one drive
 */
class CDriveSize
	: public CObject
{
public:
	char driveLetter;
	unsigned __int64 i64FreeBytesToCaller;
	unsigned __int64 i64TotalBytes;
	unsigned __int64 i64FreeBytes;

public:
	CDriveSize(char driveLetter);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief array of CDriveSize
 */
class CDriveSizeArray
	: public CObArray
{
public:
	CDriveSizeArray();
	~CDriveSizeArray();

	CDriveSize * GetAt(int i);
};
