
/**
 * @brief SMART Logger用イベントレポートデータ
 *
 * @author kumagai
 */
class CSmartLoggerReportEventData
	: public CReportEventData
{
public:
	CSmartLoggerReportEventData(WORD type, DWORD eventId, UINT code, TCHAR * message);
};
