
/**
 * @brief ���W�X�g���̒l�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
class CRegistryValue
	: public CBinaryBlock
{
public:
	BOOL success;
	DWORD type;

public:
	CRegistryValue(HKEY key, CString valueName);
};
