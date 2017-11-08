
extern int DispatchService(void);
extern void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
extern VOID WINAPI ServiceCtrlHandler(DWORD fdwControl);
extern BOOL SendStatusToSCM(DWORD	dwCurrentState,				// サービスステータス
					 DWORD	dwWin32ExitCode,			// エラーコード
					 DWORD	dwServiceSpecificExitCode,	// サービス定義エラーコード
					 DWORD	dwCheckPoint,				// サービス進捗指標
					 DWORD	dwWaitHint);				// 待機時間(ms)
extern DWORD WINAPI smartLogger_server(LPVOID lpParameter);

int GetAndWriteSmart(CString fluentCatPath, CString logFilePath, CString fluentdSensorName, CString fluentdHostName);
