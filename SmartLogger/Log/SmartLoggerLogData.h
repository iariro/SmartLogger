
/**
 * @defgroup LogData 実際にPutLogを行うログデータ
 */

/**
 * @brief SMART Logger用ログ情報データ
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
 * @brief   SMART Logger開始についてのログ
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
 * @brief   SMART Logger停止についてのログ
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
 * @brief  サービス開始時のエラーについてのログ
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
 * @brief ファイル出力エラーについてのログ
 * @ingroup LogData
 */
class CFileWriteErrorLog
	: public CSmartLoggerLogData
{
public:
	CFileWriteErrorLog(CString path);
};

/**
 * @brief  サービス開始時のエラーについてのログ
 * @ingroup LogData
 */
class CSendtoFluentdLog1
	: public CSmartLoggerLogData
{
public:
	CSendtoFluentdLog1(CString commandLine);
};

/**
 * @brief  サービス開始時のエラーについてのログ
 * @ingroup LogData
 */
class CSendtoFluentdLog
	: public CSmartLoggerLogData
{
public:
	CSendtoFluentdLog(int error, char * json);
};

/**
 * @brief  SCMへのサービス状態通知でのエラーについてのログ
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
