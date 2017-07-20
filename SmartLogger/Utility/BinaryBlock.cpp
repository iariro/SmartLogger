#include "StdAfx.h"
#include "BinaryBlock.h"

////////////////////////////////////////////////////////////////////////////////
// CBinaryBlock

/**
 * @brief �I�u�W�F�N�g�̍\�z�݂̂ŁA�o�b�t�@�̊m�ۂ͍s��Ȃ��B
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
CBinaryBlock::CBinaryBlock(void)
{
	// �������Ȃ��B
}

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɏw��T�C�Y���̃o�b�t�@�m�ۂ��s���B
 *
 * @author kumagai
 *
 * @param size �m�ۂ���o�b�t�@�̃o�C�g��
 *
 * @date 2005/04/13
 *	- kumagai:(2.0.1.0 �� 2.0.2.0)�w��T�C�Y 0 �̏ꍇ�o�C�g��̊m�ۂ��s��Ȃ���
 *		���ύX�B
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
CBinaryBlock::CBinaryBlock(UINT size)
{
	this->size = size;
	if(this->size > 0)
	{
		// ���e����B

		data = new BYTE [this->size];
	}
	else
	{
		// ���e�����B

		data = NULL;
	}
}

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɏw��o�C�i���u���b�N�̃R�s�[���s���B
 *
 * @author kumagai
 *
 * @param sourceBinaryBlock �R�s�[���o�C�i���u���b�N
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
CBinaryBlock::CBinaryBlock(CBinaryBlock * sourceBinaryBlock)
{
	this->size = sourceBinaryBlock->size;
	if(this->size > 0)
	{
		// ���e����B

		data = new BYTE [this->size];
		::CopyMemory(data, sourceBinaryBlock->data, sourceBinaryBlock->size);
	}
	else
	{
		// ���e�����B

		data = NULL;
	}
}

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��Ɏw��o�b�t�@�̃R�s�[���s���B
 *
 * @author kumagai
 *
 * @param buffer �o�b�t�@
 * @param offset �I�t�Z�b�g
 * @param size �T�C�Y
 *
 * @date 2005/04/13
 *	- kumagai:(2.0.1.0 �� 2.0.2.0)�w��T�C�Y 0 �̏ꍇ�o�C�g��̊m�ۂ��s��Ȃ���
 *		���ύX�B
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
CBinaryBlock::CBinaryBlock(BYTE * buffer, UINT offset, UINT size)
{
	this->size = size;
	if(this->size > 0)
	{
		// ���e����B

		data = new BYTE [this->size];
		::CopyMemory(data, buffer + offset, size);
	}
	else
	{
		// ���e�����B

		data = NULL;
	}
}

/**
 * @brief �I�u�W�F�N�g�̔j���ƂƂ��Ƀo�b�t�@�̉�����s���B
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 */
CBinaryBlock::~CBinaryBlock()
{
	if(size > 0)
	{
		// ���e����B

		delete [] data;
	}
}

/**
 * @brief ���O�p�̃_���v�@�\�B������ɂăo�b�t�@���e�̃_���v���擾����B
 *
 * @author kumagai
 *
 * @return �_���v������
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 *	- kumagai:Unicode �Ή��B
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
 * @brief �f�o�b�O�p�̃_���v�@�\�B������ɂăo�b�t�@���e�̃_���v���擾����B
 *
 * @author kumagai
 *
 * @return �_���v������
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 *	- kumagai:Unicode �Ή��B
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
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�W�񂵂Ă���o�C�i���u���b�N�v�f���������B
 *
 * @author kumagai
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
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
 * @brief �W�񂵂������̃o�C�i���u���b�N���P�̃o�C�i���u���b�N�Ɍ�������B
 *
 * @author kumagai
 *
 * @return �������ꂽ�o�C�i���u���b�N
 *
 * @date 2008/03/06
 *	- kumagai:Ver2.1 ����ڍs�B
 * @date 2008/03/19
 *	- kumagai:GetBinaryBlock() ��������B
 */
CBinaryBlock * CBinaryBlockCollection::Concatenate(void)
{
	int i;
	UINT totalSize, offset;
	CBinaryBlock * block;
	CBinaryBlock * collectBlock;

	// ������̃T�C�Y���Z�o�B
	totalSize = 0;
	for(i=0 ; i<GetSize() ; i++)
	{
		block = (CBinaryBlock *)GetAt(i);
		totalSize += block->size;
	}

	collectBlock = new CBinaryBlock(totalSize);

	// �P�v�f���R�s�[���Č����B
	offset = 0;
	for(i=0 ; i<GetSize() ; i++)
	{
		block = (CBinaryBlock *)GetAt(i);
		::CopyMemory(collectBlock->data + offset, block->data, block->size);
		offset += block->size;
	}

	return collectBlock;
}
