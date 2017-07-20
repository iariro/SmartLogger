
/**
 * @brief バイナリデータをバイト配列として扱うデータ型。
 * CObArray に格納するため CObject を継承する。
 *
 * @author kumagai
 *
 * @date 2005/04/13-20
 *	- kumagai:(2.0.1.0 → 2.0.2.0)CBinaryBlock のコンストラクタにおいて、指定サ
 *		イズ 0 の場合バイト列の確保を行わないよう変更。
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
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
 * @brief 複数の CBinaryBlock 型のデータをまとめる型。１つの CBinaryBlock を生成
 * する。
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 * @date 2008/03/19
 *	- kumagai:GetBinaryBlock() から Concatenate() に改名。
 */
class AFX_EXT_CLASS CBinaryBlockCollection
	: public CObArray
{
public:
	~CBinaryBlockCollection();
	CBinaryBlock * Concatenate(void);
};
