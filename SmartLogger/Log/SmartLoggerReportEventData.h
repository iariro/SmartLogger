
/**
 * @brief SMARTLoggerサービス開始イベント
 */
class CStartServiceEvent
	: public CReportEventData
{
public:
	CStartServiceEvent(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName, int smartWatchInterval);
};

/**
 * @brief   SMART Logger停止についてのログ
 * @ingroup LogData
 */
class CStopServiceEvent
	: public CReportEventData
{
public:
	CStopServiceEvent(void);
};

/**
 * @brief  サービス開始時のエラーについてのログ
 * @ingroup LogData
 */
class CStartServiceErrorEvent
	: public CReportEventData
{
public:
	CStartServiceErrorEvent(TCHAR * info);
};

/**
 * @brief SMART取得エラーについてのログ
 * @ingroup LogData
 */
class CGetSmartSuccessEvent
	: public CReportEventData
{
public:
	CGetSmartSuccessEvent(CString filename, int blockCount);
};

/**
 * @brief SMART取得エラーについてのログ
 * @ingroup LogData
 */
class CGetSmartErrorEvent
	: public CReportEventData
{
public:
	CGetSmartErrorEvent(DWORD readIdentifyError, DWORD readSmartError, DWORD readThresholdError, DWORD readLogError);
};

/**
 * @brief ファイル出力エラーについてのログ
 * @ingroup LogData
 */
class CFileWriteErrorEvent
	: public CReportEventData
{
public:
	CFileWriteErrorEvent(CString path);
};

/**
 * @brief  サービス開始時のエラーについてのログ
 * @ingroup LogData
 */
class CSendtoFluentdEvent1
	: public CReportEventData
{
public:
	CSendtoFluentdEvent1(CString commandLine);
};

/**
 * @brief  サービス開始時のエラーについてのログ
 * @ingroup LogData
 */
class CSendtoFluentdEvent
	: public CReportEventData
{
public:
	CSendtoFluentdEvent(int error);
};

/**
 * @brief  SCMへのサービス状態通知でのエラーについてのログ
 * @ingroup LogData
 */
class CSendStatusToSCMErrorEvent
	: public CReportEventData
{
public:
	CSendStatusToSCMErrorEvent(DWORD status, DWORD checkPoint);
};
