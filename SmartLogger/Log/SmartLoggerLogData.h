
/**
 * @defgroup LogData ���ۂ�PutLog���s�����O�f�[�^
 */

/**
 * @brief SMART Logger�p���O���f�[�^
 *
 * @author kumagai
 */
class CSmartLoggerLogData
{
private:
	static const TCHAR * sourceName;

public:
	const TCHAR * source;
	UINT type;
	UINT eventId;
	UINT code;
	CStr * message;

public:
	CSmartLoggerLogData(UINT type, UINT code);
	virtual ~CSmartLoggerLogData();

	void SetMessage(CStr * message);
};

/**
 * @brief   SMART Logger�J�n�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author  kumagai
 */
class CStartServiceLog
	: public CSmartLoggerLogData
{
public:
	CStartServiceLog(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName);
};

/**
 * @brief   SMART Logger��~�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author  kumagai
 */
class CStopServiceLog
	: public CSmartLoggerLogData
{
public:
	CStopServiceLog(void);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author abem
 */
class CStartServiceErrorLog
	: public CSmartLoggerLogData
{
public:
	CStartServiceErrorLog(TCHAR * info);
};

/**
 * @brief �t�@�C���o�̓G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CFileWriteErrorLog
	: public CSmartLoggerLogData
{
public:
	CFileWriteErrorLog(CString path);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CSendtoFluentdLog1
	: public CSmartLoggerLogData
{
public:
	CSendtoFluentdLog1(CString commandLine);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CSendtoFluentdLog
	: public CSmartLoggerLogData
{
public:
	CSendtoFluentdLog(int error, char * json);
};

/**
 * @brief  SCM�ւ̃T�[�r�X��Ԓʒm�ł̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author abem
 */
class CSendStatusToSCMErrorLog
	: public CSmartLoggerLogData
{
public:
	CSendStatusToSCMErrorLog(DWORD status, DWORD checkPoint);
};
