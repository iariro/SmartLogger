
#define SRV_DESC_INFO		_T("HDD��SMART�擾���s���܂��B")	/**< �T�[�r�X������				*/
#define SERVICE_NAME		_T("SmartLogger")		/**< �T�[�r�X��					*/
#define SERVICE_DISPLAY_NAME		_T("SMART Logger")		/**< �T�[�r�X��					*/

#define OPTION_INSTALL		_T("/install")				/**< �N���I�v�V����:SCM�ւ̓o�^					*/
#define OPTION_UNINSTALL	_T("/uninstall")			/**< �N���I�v�V����:SCM����̓o�^����			*/
#define OPTION_START		_T("/start")				/**< �N���I�v�V����:�T�[�r�X�J�n				*/
#define OPTION_STOP			_T("/stop")					/**< �N���I�v�V����:�T�[�r�X��~				*/

#define SMARTLOGGER_CONTROL_START		1					/**< �T�[�r�X�J�n����R�[�h(ControlWcpa�ŗ��p)	*/
#define SMARTLOGGER_CONTROL_STOP		2					/**< �T�[�r�X��~����R�[�h(ControlWcpa�ŗ��p)	*/

class CServiceInstall
{
public:
	static int install_service(TCHAR	*pModulePath);
	static int uninstall_service(void);
	static int register_service(TCHAR * pModulePath);
	static int unregister_service(void);
	static int control_service(DWORD dwControl, TCHAR * pService);
};
