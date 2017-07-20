
/**
 * @brief NULL ������؂�̕�����R���N�V�������琶���\�� CStringArray�B
 * ::RegQueryValueEx() �ɂ�� REG_MULTI_SZ ������R���N�V�����̎擾�A
 * ::GetPrivateProfileString() �ɂ��Z�N�V������L�[�̗񋓂Ȃǂœ�����
 * �f�[�^�̏����ɗL���B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
class AFX_EXT_CLASS CNsvStringArray
	: public CStringArray
{
private:
	static const char delimiter;

public:
	CNsvStringArray(TCHAR * buffer, UINT size);
};
