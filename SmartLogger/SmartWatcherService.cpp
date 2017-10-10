#include "StdAfx.h"
#include "Windows.h"
#include "winsvc.h"
#include "SmartWatcherService.h"
#include "ServiceInstall.h"

#include "winioctl.h"
#include <ntddscsi.h>
#include "SmartDevice.h"
#include "SmartAttribute.h"
#include "FluentdCall.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/ReportEventData.h"
#include "Log/SmartLoggerLogging.h"
#include "Log/SmartLoggerReportEventData.h"

#include "Utility/BinaryBlock.h"
#include "Registry/RegistryValue.h"
#include "Registry/RegistryKey.h"

/*
 * �O���[�o���ϐ��̒�`
 */
SERVICE_STATUS_HANDLE	g_statusService;			/**< �T�[�r�X�X�e�[�^�X�擾�p		*/
HANDLE					g_hEventTerminate = NULL;	/**< �I���V�O�i����M�p				*/
HANDLE					g_hThreadService = NULL;	/**< �T�[�r�X���C���X���b�h�쐬�p	*/
DWORD	g_dwCurrentState = SERVICE_START_PENDING;	/**< ���݂̃T�[�r�X���	*/


/**
 * @brief	SCM ����Ăяo���ꂽ�ꍇ�AWcpa �T�[�r�X���J�n������
 *
 * SCM �ȊO����̌Ăяo���ł͎��s����
 *
 * @param	�Ȃ�
 * @return	�����̐����E���s
 * @retval	0	����ɓo�^����
 * @retval	0�ȊO	�G���[
 *            - 1:�T�[�r�X���C���֐��̌Ăяo���Ɏ��s
 */
int DispatchWcpa(void)
{
	/*
	 * �ϐ���`
	 */
	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l

	// �T�[�r�X�e�[�u���̒�`
	SERVICE_TABLE_ENTRY	service_table[] = {
		{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};


	/*
	 * �ϐ�������
	 */

	g_hEventTerminate	= NULL;
	g_hThreadService	= NULL;


	/*
	 * �T�[�r�X�f�B�X�p�b�`�̎��s
	 *   �T�[�r�X�e�[�u���ɒ�`���� ServiceMain �֐��̌Ăяo�����s����
	 */
	fRetFunc = StartServiceCtrlDispatcher(service_table);
	if( fRetFunc == FALSE )
	{
		// �f�B�X�p�b�`�Ɏ��s(�T�[�r�X����Ăяo����Ȃ��ꍇ�͎��s����)
		return 1;
	}

	// ����ɏ����I��
	return 0;
}


/**
 * @brief	�T�[�r�X���C���֐�
 * @param	argc		[in] ������
 * @param	argv		[in] �����z��
 * @return	�Ȃ�
 */
VOID WINAPI ServiceMain(DWORD argc,	// ������
						LPTSTR *argv)	// �����z��
{
	/*
	 * �ϐ���`
	 */

	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l
	DWORD	dwIdService;		// �T�[�r�X�{�̃X���b�h ID

	/*
	 * �T�[�r�X�R���g���[���n���h���֐��̓o�^
	 */
	g_statusService = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if( g_statusService == NULL )
	{
		// �T�[�r�X�R���g���[���n���h���֐��̓o�^���s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�R���g���[���n���h���֐��̓o�^")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 1, 5000);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		return;
	}


	/*
	 * �T�[�r�X�I���V�O�i���C�x���g�̐���
	 */
	g_hEventTerminate = CreateEvent(NULL, TRUE, FALSE, NULL);
	if( !g_hEventTerminate )
	{
		// ���s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�I���V�O�i���C�x���g�̐���")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_START_PENDING
	fRetFunc = SendStatusToSCM(SERVICE_START_PENDING, NO_ERROR, 0, 2, 1000);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		return;
	}


	/*
	 * �T�[�r�X�{�̕����̎��s - �T�[�r�X�{�̃X���b�h���쐬
	 */
	g_hThreadService = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)smartLogger_server, NULL, NULL, &dwIdService);
	if( g_hThreadService == NULL )
	{
		// �X���b�h�쐬�Ɏ��s - �����ɏI��
		// ���O�o��
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�T�[�r�X�{�̃X���b�h�̐���")));
		return;
	}

	// SCM �֏󋵒ʒm -  SERVICE_RUNNING
	fRetFunc = SendStatusToSCM(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	if( fRetFunc == FALSE )
	{
		// �ʒm�Ɏ��s - �����ɏI��
		if( g_hThreadService )
		{
			CloseHandle(g_hEventTerminate);
		}
		return;
	}

	/*
	 * �I���V�O�i���̑҂���
	 */
	WaitForSingleObject(g_hEventTerminate, INFINITE);

	// �I���V�O�i�������m�����ꍇ�A�V�O�i����M�p�n���h�����N���[�Y���ďI��
	if( g_hEventTerminate )
	{
		CloseHandle(g_hEventTerminate);
	}

	// SCM �� SERVICE_STOPPED ��ʒm����B
	// ServiceCtrlHandler �ł͂Ȃ��A�I���V�O�i�����󂯎�������ɒʒm����
	SendStatusToSCM(SERVICE_STOPPED, NO_ERROR, 0, 0, 100);

	return;
}


/**
 * @brief	�T�[�r�X�R���g���[���n���h��
 * @param	fdwControl	[in] �v������������R�[�h
 * @return	�Ȃ�
 */
VOID WINAPI ServiceCtrlHandler(DWORD fdwControl		// �v������������R�[�h
							   )
{
	/*
	 * �ϐ���`
	 */
	BOOL	fRetFunc;			// �Ăяo���֐��̖߂�l

	/*
	 * �T�[�r�X�R���g���[���R�[�h��M���̊e������`��
	 */
	switch( fdwControl )
	{
	/*
	 * �T�[�r�X��~�v����M
	 *   �i��~�����V���b�g�_�E�����j
	 */
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:

		/*
		 * SCM �֏󋵒ʒm -  SERVICE_STOP_PENDING
		 */
		fRetFunc = SendStatusToSCM(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 100);
		if( fRetFunc == FALSE )
		{
			// �ʒm�ɐ������Ȃ������ꍇ�͓��ɉ������Ȃ�
			break;
		}

		/*
		 * �T�[�r�X�{�̃X���b�h���N���[�Y
		 */
		if( g_hThreadService )
		{
			CloseHandle(g_hThreadService);
			g_hThreadService = NULL;
		}

		// SMARTLOGGER����~�������Ƃ��������O���o��
		CSmartLoggerLogging::PutLog(&CStopServiceEvent());

		/*
		 * �I���V�O�i���̒ʒm
		 */
		SetEvent(g_hEventTerminate);
		return;

	/*
	 * ��Ԃ̍X�V
	 */
	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}


	/*
	 * SCM �֏󋵂�ʒm
	 *   ���̃v���O�����ł́A���� SERVICE_CONTROL_INTERROGATE ( + default ) �̂��߂����ɂ���
	 *   �ʒm���s�������ɏ������s��Ȃ�
	 */
	fRetFunc = SendStatusToSCM(g_dwCurrentState, NO_ERROR, 0, 0, 0);
	return;
}


/**
 * @brief	�T�[�r�X�󋵂� SCM �ɒʒm����
 * @param	dwCurrentState				[in] �T�[�r�X�X�e�[�^�X
 * @param	dwWin32ExitCode				[in] �G���[�R�[�h
 * @param	dwServiceSpecificExitCode	[in] �T�[�r�X��`�G���[�R�[�h
 * @param	dwCheckPoint				[in] �T�[�r�X�i���w�W
 * @param	dwWaitHint					[in] �ҋ@����(ms)
 * @return	�����̐����E���s
 * @retval	TRUE	��������
 * @retval	FALSE	�����Ɏ��s
 */
BOOL SendStatusToSCM(DWORD	dwCurrentState,				// �T�[�r�X�X�e�[�^�X
					 DWORD	dwWin32ExitCode,			// �G���[�R�[�h
					 DWORD	dwServiceSpecificExitCode,	// �T�[�r�X��`�G���[�R�[�h
					 DWORD	dwCheckPoint,				// �T�[�r�X�i���w�W
					 DWORD	dwWaitHint					// �ҋ@����(ms)
					 )
{
	/*
	 * �ϐ���`
	 */
	SERVICE_STATUS	status;		// �T�[�r�X�X�e�[�^�X�\����
	BOOL			fRetFunc;	// �Ăяo���֐��̖߂�l

	/*
	 * �ϐ�������
	 */
	status.dwServiceType	= SERVICE_WIN32_OWN_PROCESS;
	status.dwCurrentState	= dwCurrentState;
	status.dwCheckPoint		= dwCheckPoint;
	status.dwWaitHint		= dwWaitHint;

	/*
	 * �T�[�r�X�󋵂ɂ��AdwControlsAccepted ��ݒ肷��
	 *
	 *   �ESERVICE_START_PENDING(�����̏��������s��)�̏ꍇ�A
	 *       �R���g���[���C�x���g�͎󂯎��Ȃ�
	 *   �E����ȊO
	 *       �T�[�r�X��~(+�T�[�r�X�J�n)�̂ݒʒm���󂯎��
	 */
	status.dwControlsAccepted = (dwCurrentState == SERVICE_START_PENDING) ?
		0 : (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);

	/*
	 * �T�[�r�X��`�G���[���w�肳�ꂽ�ꍇ�͂���𗘗p����
	 */
	status.dwWin32ExitCode = (dwServiceSpecificExitCode == 0) ?
		dwWin32ExitCode : ERROR_SERVICE_SPECIFIC_ERROR;


	/*
	 * ��ԃ��R�[�h�� SCM �֓n��
	 */
	fRetFunc = SetServiceStatus(g_statusService, &status);
	if( fRetFunc == FALSE )
	{
		// �T�[�r�X��Ԃ̐ݒ�Ɏ��s
		// ���O���o��
		CSmartLoggerLogging::PutLog(&CSendStatusToSCMErrorEvent(dwCurrentState, dwCheckPoint));
		return FALSE;
	}

	g_dwCurrentState = dwCurrentState;

	/*
	 * ����I��
	 */
	return TRUE;
}

int SMART_WATCH_INTERVAL = (1000 * 3600);

const CString registryPath =
	_T("HKEY_LOCAL_MACHINE\\Software\\TOPS\\SmartLogger");

DWORD WINAPI smartLogger_server(LPVOID lpParameter)
{
	CString * logFilePath = NULL;
	CString * fluentcatPath = NULL;
	CString * fluentdSensorName = NULL;
	CString * fluentdHostName = NULL;
	DWORD * smartWatchInterval2 = NULL;

	CRegistryRootKey root(registryPath);

	if(root.IsValid())
	{
		// ���[�g�L�[�͗L���B

		CString subKeyPath = CRegistryKey::GetSubKeyPath(registryPath);
		CRegistryKey * subKey = root.OpenForRead(subKeyPath);
		if (subKey != NULL)
		{
			// �T�u�L�[�I�[�v�������B

			logFilePath = subKey->GetStringValue(_T("LogFilePath"));
			fluentcatPath = subKey->GetStringValue(_T("FluentcatPath"));
			fluentdSensorName = subKey->GetStringValue(_T("FluentdSensorName"));
			fluentdHostName = subKey->GetStringValue(_T("FluentdHostName"));
			smartWatchInterval2 = subKey->GetDWordValue(_T("SmartWatchInterval"));

			delete subKey;
		}
	}

	if (logFilePath == NULL ||
		fluentcatPath == NULL ||
		fluentdSensorName == NULL ||
		fluentdHostName == NULL)
	{
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("�ݒ�s��")));
		return 0;
	}

	int smartWatchInterval;
	if (smartWatchInterval2 != NULL)
	{
		smartWatchInterval = *smartWatchInterval2 * 1000;
	}
	else
	{
		smartWatchInterval = SMART_WATCH_INTERVAL;
	}

	CSmartLoggerLogging::PutLog(&CStartServiceEvent(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName, smartWatchInterval));

	GetAndWriteSmart(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName);

	DWORD last = ::GetTickCount();

	while (TRUE)
	{
		DWORD now = ::GetTickCount();

		if (last + smartWatchInterval < now)
		{
			GetAndWriteSmart(*fluentcatPath, *logFilePath, *fluentdSensorName, *fluentdHostName);

			last += smartWatchInterval;
		}

		::Sleep(1000);
	}

	if (logFilePath != NULL)
	{
		delete logFilePath;
	}
	if (fluentcatPath != NULL)
	{
		delete fluentcatPath;
	}
	if (fluentdSensorName != NULL)
	{
		delete fluentdSensorName;
	}
	if (fluentdHostName != NULL)
	{
		delete fluentdHostName;
	}
	if (smartWatchInterval2 != NULL)
	{
		delete smartWatchInterval2;
	}

	return 0;
}

const int JSON_BUFFER_SIZE = 10000;

/**
 * @brief Get drive size and build binary list or output Json.
 *
 * @param outMode OUT_FILE or OUT_STDOUT_JSON
 * @param list2 binary list of body part
 * @param blockCount reference of block count
 * @param nRetCode reference of return code
 * @param allocSuccess reference of Success and failure
 *
 * @date 2016/09/20
 *	- kumagai : new
 */
void GetDriveSize(char * jsonBuffer, int & jsonBufferLength, int & count)
{
	DWORD dwDrive;
	char pszDrive[16];
	UINT DriveType;
	BOOL fResult;
	unsigned __int64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

	dwDrive = GetLogicalDrives();
	for (int nDrive = 0; nDrive < 26; nDrive++)
	{
		if (dwDrive & (1 << nDrive))
		{
			sprintf_s(pszDrive, sizeof(pszDrive), "%c:\\", nDrive + 'A');

			DriveType = GetDriveTypeA(pszDrive);
			if (DriveType == DRIVE_FIXED)
			{
				fResult =
					::GetDiskFreeSpaceExA(
						pszDrive,
						(PULARGE_INTEGER)&i64FreeBytesToCaller,
						(PULARGE_INTEGER)&i64TotalBytes,
						(PULARGE_INTEGER)&i64FreeBytes);

				if (count > 0)
				{
					jsonBuffer[jsonBufferLength] = ',';
					jsonBufferLength++;
				}

				jsonBufferLength += sprintf_s(
					jsonBuffer + jsonBufferLength,
					JSON_BUFFER_SIZE - jsonBufferLength,
					"\"Drive%c_TotalSize\": \"%I64u\","
					"\"Drive%c_FreeSize\": \"%I64u\"",
					'A' + nDrive,
					i64TotalBytes,
					'A' + nDrive,
					i64FreeBytes);
				count++;
			}
		}
	}
}

char * CreateSmartJson(BYTE * smart, BYTE * log)
{
	int count = 0;
	char * jsonBuffer = new char [JSON_BUFFER_SIZE];
	int jsonBufferLength = 0;

	jsonBuffer[jsonBufferLength] = '{';
	jsonBufferLength++;

	if (smart != NULL)
	{
		for (int i=1 ; i<512 ; i+=12)
		{
			int id = smart[i];
			int value = smart[i + 3];
			int raw = smart[i + 5] + (smart[i + 6] << 8);

			if (id == 0)
			{
				break;
			}

			char * name = NULL;
			for (int j=0 ; j<GetSmartAttributeCount() ; j++)
			{
				if (attributeIdAndName[j].id == id)
				{
					name = attributeIdAndName[j].name;
					break;
				}
			}

			if (count > 0)
			{
				jsonBuffer[jsonBufferLength] = ',';
				jsonBufferLength++;
			}

			jsonBufferLength += sprintf_s(
				jsonBuffer + jsonBufferLength,
				JSON_BUFFER_SIZE - jsonBufferLength,
				"\"%s_Value\":\"%d\",\"%s_Raw\":\"%d\"",
				name,
				value,
				name,
				raw);
			count++;
		}
	}

	if (log != NULL)
	{
		int errorLogCount = 0;
		errorLogCount = log[452] + (log[453] << 8);

		if (count > 0)
		{
			jsonBuffer[jsonBufferLength] = ',';
			jsonBufferLength++;
		}

		jsonBufferLength += sprintf_s(
			jsonBuffer + jsonBufferLength,
			JSON_BUFFER_SIZE - jsonBufferLength,
			"\"ErrorLogCount\":\"%d\"",
			errorLogCount);
	}

	GetDriveSize(jsonBuffer, jsonBufferLength, count);

	jsonBuffer[jsonBufferLength] = '}';
	jsonBufferLength++;
	jsonBuffer[jsonBufferLength] = '\0';
	jsonBufferLength++;

	return jsonBuffer;
}

void GetAndWriteSmart(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName)
{
	CSmartDevice device;

	IDENTIFY_DEVICE_OUTDATA * identify = NULL;
	SMART_READ_DATA_OUTDATA * smartValue = NULL;
	SMART_READ_DATA_OUTDATA * threshold = NULL;
	SMART_READ_LOG_OUTDATA * log = NULL;

	if (device.Open(0))
	{
		identify = device.ReadIdentify();
		smartValue = device.ReadSmart();
		threshold = device.ReadThreshold();
		log = device.ReadLog(0, 1);
		device.Close();

		if (identify != NULL ||
			smartValue != NULL ||
			threshold != NULL ||
			log != NULL)
		{
			// got something

			int blockCount = 0;
			if (identify != NULL)
			{
				blockCount++;
			}
			if (smartValue != NULL)
			{
				blockCount++;
			}
			if (threshold != NULL)
			{
				blockCount++;
			}
			if (log != NULL)
			{
				blockCount++;
			}

			if (! logFilePath.IsEmpty())
			{
				// ���O�o�̓p�X�̎w�肠��

				CTime time = CTime::GetCurrentTime();
				char header [68];
				::ZeroMemory(header, 68);
				strcpy_s(header, "SMARTCrawlResult");

				sprintf_s(
					header + 48,
					16,
					"%04d%02d%02d%02d%02d%02d",
					time.GetYear(),
					time.GetMonth(),
					time.GetDay(),
					time.GetHour(),
					time.GetMinute(),
					time.GetSecond());

				memcpy(header + 64, &blockCount, sizeof(int));

				CFile file;

				CString filename;

				filename.Format(
					_T("%s\\smart_%04d%02d%02d"),
					logFilePath,
					time.GetYear(),
					time.GetMonth(),
					time.GetDay());

				if (file.Open(filename, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::typeBinary))
				{
					file.SeekToEnd();
					file.Write(header, sizeof(header));
					BYTE blockHeader [12];
					::ZeroMemory(blockHeader, sizeof(blockHeader));
					blockHeader[0] = 10;

					if (identify != NULL)
					{
						blockHeader[5] = 0xec;
						blockHeader[6] = 0x00;
						blockHeader[8] = IDENTIFY_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = IDENTIFY_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)identify->sSendCmdOutParam.bBuffer, IDENTIFY_BUFFER_SIZE);
					}
					if (smartValue != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd0;
						blockHeader[8] = READ_ATTRIBUTE_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = READ_ATTRIBUTE_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)smartValue->sSendCmdOutParam.bBuffer, READ_ATTRIBUTE_BUFFER_SIZE);
					}
					if (threshold != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd1;
						blockHeader[8] = READ_THRESHOLD_BUFFER_SIZE % 0x100; // size
						blockHeader[9] = READ_THRESHOLD_BUFFER_SIZE / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)threshold->sSendCmdOutParam.bBuffer, READ_THRESHOLD_BUFFER_SIZE);
					}
					if (log != NULL)
					{
						blockHeader[5] = 0xb0;
						blockHeader[6] = 0xd5;
						blockHeader[7] = 0x00;
						blockHeader[8] = sizeof(ata_smart_exterrlog) % 0x100; // size
						blockHeader[9] = sizeof(ata_smart_exterrlog) / 0x100; // size
						blockHeader[10] = 0x00;
						blockHeader[11] = 0x00;
						file.Write(blockHeader, 12);
						file.Write((BYTE *)log->sSendCmdOutParam.bBuffer, sizeof(ata_smart_exterrlog));
					}
					file.Close();
				}
				else
				{
					CSmartLoggerLogging::PutLog(&CFileWriteErrorEvent(filename));
				}
			}
		}
		else
		{
			// all null

			CSmartLoggerLogging::PutLog(&CGetSmartErrorEvent());
		}
	}
	else
	{
		CSmartLoggerLogging::PutLog(&CStartServiceErrorEvent(_T("CSmartDevice::open")));
	}

	char * jsonBuffer =
		CreateSmartJson(
			smartValue != NULL ? smartValue->pData : NULL,
			log != NULL ? log->pData : NULL);

	if (identify != NULL)
	{
		delete identify;
	}
	if (smartValue != NULL)
	{
		delete smartValue;
	}
	if (threshold != NULL)
	{
		delete threshold;
	}
	if (log != NULL)
	{
		delete log;
	}

	if (jsonBuffer != NULL)
	{
		if (! fluentCatPath.IsEmpty() && ! fluentdSensorName.IsEmpty() && ! fluentdHostName.IsEmpty())
		{
			// fluentcat�Ăяo���p�p�����[�^�̎w�肠��

			int sendRecv =
				SendtoFluentCat(
					fluentCatPath,
					fluentdSensorName,
					fluentdHostName,
					jsonBuffer);

			CSmartLoggerLogging::PutLog(&CSendtoFluentdEvent(sendRecv, jsonBuffer));
		}

		delete [] jsonBuffer;
	}
}
