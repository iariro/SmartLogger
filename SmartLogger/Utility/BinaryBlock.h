
/**
 * @brief �o�C�i���f�[�^���o�C�g�z��Ƃ��Ĉ����f�[�^�^�B
 * CObArray �Ɋi�[���邽�� CObject ���p������B
 *
 * @author kumagai
 *
 * @date 2005/04/13-20
 *	- kumagai:(2.0.1.0 �� 2.0.2.0)CBinaryBlock �̃R���X�g���N�^�ɂ����āA�w��T
 *		�C�Y 0 �̏ꍇ�o�C�g��̊m�ۂ��s��Ȃ��悤�ύX�B
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
class AFX_EXT_CLASS CBinaryBlock
	: public CObject
{
public:
	BYTE * data;
	UINT size;

public:
	CBinaryBlock(void);
	CBinaryBlock(UINT size);
	CBinaryBlock(CBinaryBlock * sourceBinaryBlock);
	CBinaryBlock(BYTE * buffer, UINT offset, UINT size);
	virtual ~CBinaryBlock();

	CString DumpForLog(void);
	CString Dump(void);
};

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ������ CBinaryBlock �^�̃f�[�^���܂Ƃ߂�^�B�P�� CBinaryBlock �𐶐�
 * ����B
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 * @date 2008/03/19
 *	- kumagai:GetBinaryBlock() ���� Concatenate() �ɉ����B
 */
class AFX_EXT_CLASS CBinaryBlockCollection
	: public CObArray
{
public:
	~CBinaryBlockCollection();
	CBinaryBlock * Concatenate(void);
};
