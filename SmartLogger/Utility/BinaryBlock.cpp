#include "StdAfx.h"
#include "BinaryBlock.h"

////////////////////////////////////////////////////////////////////////////////
// CBinaryBlock

/**
 * @brief オブジェクトの構築のみで、バッファの確保は行わない。
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlock::CBinaryBlock(void)
{
	// 何もしない。
}

/**
 * @brief オブジェクトの構築とともに指定サイズ分のバッファ確保を行う。
 *
 * @author kumagai
 *
 * @param size 確保するバッファのバイト数
 *
 * @date 2005/04/13
 *	- kumagai:(2.0.1.0 → 2.0.2.0)指定サイズ 0 の場合バイト列の確保を行わないよ
 *		う変更。
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlock::CBinaryBlock(UINT size)
{
	this->size = size;
	if(this->size > 0)
	{
		// 内容あり。

		data = new BYTE [this->size];
	}
	else
	{
		// 内容無し。

		data = NULL;
	}
}

/**
 * @brief オブジェクトの構築とともに指定バイナリブロックのコピーを行う。
 *
 * @author kumagai
 *
 * @param sourceBinaryBlock コピー元バイナリブロック
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlock::CBinaryBlock(CBinaryBlock * sourceBinaryBlock)
{
	this->size = sourceBinaryBlock->size;
	if(this->size > 0)
	{
		// 内容あり。

		data = new BYTE [this->size];
		::CopyMemory(data, sourceBinaryBlock->data, sourceBinaryBlock->size);
	}
	else
	{
		// 内容無し。

		data = NULL;
	}
}

/**
 * @brief オブジェクトの構築とともに指定バッファのコピーを行う。
 *
 * @author kumagai
 *
 * @param buffer バッファ
 * @param offset オフセット
 * @param size サイズ
 *
 * @date 2005/04/13
 *	- kumagai:(2.0.1.0 → 2.0.2.0)指定サイズ 0 の場合バイト列の確保を行わないよ
 *		う変更。
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlock::CBinaryBlock(BYTE * buffer, UINT offset, UINT size)
{
	this->size = size;
	if(this->size > 0)
	{
		// 内容あり。

		data = new BYTE [this->size];
		::CopyMemory(data, buffer + offset, size);
	}
	else
	{
		// 内容無し。

		data = NULL;
	}
}

/**
 * @brief オブジェクトの破棄とともにバッファの解放を行う。
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlock::~CBinaryBlock()
{
	if(size > 0)
	{
		// 内容あり。

		delete [] data;
	}
}

/**
 * @brief ログ用のダンプ機能。文字列にてバッファ内容のダンプを取得する。
 *
 * @author kumagai
 *
 * @return ダンプ文字列
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 *	- kumagai:Unicode 対応。
 */
CString CBinaryBlock::DumpForLog(void)
{
	UINT i;
	CString line, returnChar;
	CString ret;

	for(i=0 ; i<size ; i++)
	{
		line.FormatMessage(_T("%1!02x!"), data[i]);
		ret += line;
	}

	ret += returnChar;

	return ret;
}

/**
 * @brief デバッグ用のダンプ機能。文字列にてバッファ内容のダンプを取得する。
 *
 * @author kumagai
 *
 * @return ダンプ文字列
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 *	- kumagai:Unicode 対応。
 */
CString CBinaryBlock::Dump(void)
{
	UINT i;
	CString line, returnChar;
	CString ret;

	returnChar.FormatMessage(_T("\n"));

	for(i=0 ; i<size ; i++)
	{
		line.FormatMessage(_T("%1!02x! "), data[i]);
		ret += line;

		if(i % 0x10 == 0xf)
			ret += returnChar;
	}

	ret += returnChar;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
// CBinaryBlockCollection

/**
 * @brief オブジェクトの構築とともに、集約しているバイナリブロック要素を解放する。
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 */
CBinaryBlockCollection::~CBinaryBlockCollection()
{
	int i;
	CBinaryBlock * block;

	for(i=0 ; i<GetSize() ; i++)
	{
		block = (CBinaryBlock *)GetAt(i);
		delete block;
	}
}

/**
 * @brief 集約した複数のバイナリブロックを１つのバイナリブロックに結合する。
 *
 * @author kumagai
 *
 * @return 結合されたバイナリブロック
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 から移行。
 * @date 2008/03/19
 *	- kumagai:GetBinaryBlock() から改名。
 */
CBinaryBlock * CBinaryBlockCollection::Concatenate(void)
{
	int i;
	UINT totalSize, offset;
	CBinaryBlock * block;
	CBinaryBlock * collectBlock;

	// 結合後のサイズを算出。
	totalSize = 0;
	for(i=0 ; i<GetSize() ; i++)
	{
		block = (CBinaryBlock *)GetAt(i);
		totalSize += block->size;
	}

	collectBlock = new CBinaryBlock(totalSize);

	// １要素ずつコピーして結合。
	offset = 0;
	for(i=0 ; i<GetSize() ; i++)
	{
		block = (CBinaryBlock *)GetAt(i);
		::CopyMemory(collectBlock->data + offset, block->data, block->size);
		offset += block->size;
	}

	return collectBlock;
}
