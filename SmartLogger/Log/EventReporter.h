
/**
 * @brief ReportEvent() によるイベントレポート処理をカプセル化
 *
 * @author kumagai
 */
class CEventReporter
{
private:
	HANDLE handle;
	const TCHAR * sourceName;

public:
	CEventReporter(const TCHAR * sourceName);

	BOOL Open(void);
	BOOL Close(void);
	BOOL ReportEvent(CReportEventData * data);
};
