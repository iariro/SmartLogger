
/**
 * @brief ログ出力を行う
 *
 * @author kumagai
 */
class CSmartLoggerLogging
{
private:
	static TCHAR * source;

public:
	static BOOL PutLog(CReportEventData * data);
};
