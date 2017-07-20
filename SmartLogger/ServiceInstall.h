
#define SRV_DESC_INFO		_T("HDDのSMART取得を行います。")	/**< サービス説明文				*/
#define SERVICE_NAME		_T("SmartLogger")		/**< サービス名					*/
#define SERVICE_DISPLAY_NAME		_T("SMART Logger")		/**< サービス名					*/

#define OPTION_INSTALL		_T("/install")				/**< 起動オプション:SCMへの登録					*/
#define OPTION_UNINSTALL	_T("/uninstall")			/**< 起動オプション:SCMからの登録解除			*/
#define OPTION_START		_T("/start")				/**< 起動オプション:サービス開始				*/
#define OPTION_STOP			_T("/stop")					/**< 起動オプション:サービス停止				*/

#define SMARTLOGGER_CONTROL_START		1					/**< サービス開始制御コード(ControlWcpaで利用)	*/
#define SMARTLOGGER_CONTROL_STOP		2					/**< サービス停止制御コード(ControlWcpaで利用)	*/

class CServiceInstall
{
public:
	static int install_service(TCHAR	*pModulePath);
	static int uninstall_service(void);
	static int register_service(TCHAR * pModulePath);
	static int unregister_service(void);
	static int control_service(DWORD dwControl, TCHAR * pService);
};
