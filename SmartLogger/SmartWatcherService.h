
extern int DispatchService(void);
extern void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
extern VOID WINAPI ServiceCtrlHandler(DWORD fdwControl);
extern BOOL SendStatusToSCM(DWORD	dwCurrentState,				// �T�[�r�X�X�e�[�^�X
					 DWORD	dwWin32ExitCode,			// �G���[�R�[�h
					 DWORD	dwServiceSpecificExitCode,	// �T�[�r�X��`�G���[�R�[�h
					 DWORD	dwCheckPoint,				// �T�[�r�X�i���w�W
					 DWORD	dwWaitHint);				// �ҋ@����(ms)
extern DWORD WINAPI smartLogger_server(LPVOID lpParameter);

int GetAndWriteSmart(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName);
