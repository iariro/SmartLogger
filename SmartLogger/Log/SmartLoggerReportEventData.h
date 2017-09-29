
/**
 * @brief 
 *
 * @author kumagai
 */
class CStartServiceEvent
	: public CReportEventData
{
public:
	CStartServiceEvent(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName, int smartWatchInterval);
};

/**
 * @brief   SMART Logger��~�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author  kumagai
 */
class CStopServiceEvent
	: public CReportEventData
{
public:
	CStopServiceEvent(void);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author abem
 */
class CStartServiceErrorEvent
	: public CReportEventData
{
public:
	CStartServiceErrorEvent(TCHAR * info);
};

/**
 * @brief �t�@�C���o�̓G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CFileWriteErrorEvent
	: public CReportEventData
{
public:
	CFileWriteErrorEvent(CString path);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CSendtoFluentdEvent1
	: public CReportEventData
{
public:
	CSendtoFluentdEvent1(CString commandLine);
};

/**
 * @brief  �T�[�r�X�J�n���̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 */
class CSendtoFluentdEvent
	: public CReportEventData
{
public:
	CSendtoFluentdEvent(int error, char * json);
};

/**
 * @brief  SCM�ւ̃T�[�r�X��Ԓʒm�ł̃G���[�ɂ��Ẵ��O
 * @ingroup LogData
 *
 * @author abem
 */
class CSendStatusToSCMErrorEvent
	: public CReportEventData
{
public:
	CSendStatusToSCMErrorEvent(DWORD status, DWORD checkPoint);
};
