
/**
 * @brief ReportEvent() �ɂ��C�x���g���|�[�g�������J�v�Z����
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
