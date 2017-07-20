
/**
 * @brief �C�x���g���|�[�g�̃f�[�^�BCEventReporter::ReportEvent() �ɂĎw�肷��B
 *
 * @author kumagai
 */
class CReportEventData
{
public:
	WORD type;
	WORD category;
	DWORD eventId;
	PSID userSid;
	WORD stringsNum;
	CCharPtrArray * strings;
	BYTE * binary;
	DWORD binarySize;

public:
	CReportEventData(WORD type, WORD category, DWORD eventId);
	CReportEventData(WORD type, WORD category, DWORD eventId, PSID userSid,
		BYTE * binary, DWORD binarySize);
	virtual ~CReportEventData();
};
