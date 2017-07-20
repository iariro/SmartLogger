#include "StdAfx.h"
#include "../Utility/BinaryBlock.h"
#include "RegistryValue.h"
#include "RegistryKey.h"
#include "../Utility/NsvStringArray.h"

/**
 * @brief ���W�X�g���p�X�̃f���~�^�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
const char CRegistryKey::delimiter = '\\';

/**
 * @brief ���W�X�g���t���p�X���烋�[�g�ȉ��̃T�u�L�[�p�X���擾����B
 *
 * @author kumagai
 *
 * @param fullPath ���W�X�g���̃t���p�X
 *
 * @return �T�u�L�[�p�X
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CString CRegistryKey::GetSubKeyPath(CString fullPath)
{
	return fullPath.Mid(fullPath.Find(delimiter) + 1);
}

/**
 * @brief �T�u�N���X�̃f�X�g���N�^���z�֐����s�̂��߂̃f�X�g���N�^�B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryKey::~CRegistryKey()
{
	// �������Ȃ��B
}

/**
 * @brief �L�[�͗L�������擾�B
 *
 * @author kumagai
 *
 * @return TRUE=�L��/FALSE=����
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::IsValid(void)
{
	return key != NULL;
}

/**
 * @brief �T�u�L�[��ǂݎ�胂�[�h�ŃI�[�v������B
 *
 * @author kumagai
 *
 * @param subKey �T�u�L�[��
 *
 * @return �T�u�L�[�I�u�W�F�N�g
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryKey * CRegistryKey::OpenForRead(CString subKey)
{
	LONG recv;
	HKEY key;
	CRegistryKey * ret;

	// �I�[�v���B
	recv = ::RegOpenKeyEx(this->key, subKey, 0, KEY_READ, &key);

	ret = NULL;
	if(recv == ERROR_SUCCESS)
	{
		// �I�[�v�������B

		ret = new CRegistrySubKey(key);
	}

	return ret;
}

/**
 * @brief �T�u�L�[���������݃��[�h�ŃI�[�v������B
 *
 * @author kumagai
 *
 * @param subKey �T�u�L�[��
 *
 * @return �T�u�L�[�I�u�W�F�N�g
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryKey * CRegistryKey::OpenForWrite(CString subKey)
{
	LONG recv;
	HKEY key;
	CRegistryKey * ret;

	// �I�[�v���B
	recv = ::RegOpenKeyEx(this->key, subKey, 0, KEY_WRITE, &key);

	ret = NULL;
	if(recv == ERROR_SUCCESS)
	{
		// �I�[�v�������B

		ret = new CRegistrySubKey(key);
	}

	return ret;
}

/**
 * @brief �T�u�L�[���쐬���I�[�v������B
 *
 * @author kumagai
 *
 * @param subKeyName �쐬����T�u�L�[�̖��O
 *
 * @return �쐬�����T�u�L�[�^NULL=�쐬���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryKey * CRegistryKey::CreateSubKeyForWrite(CString subKeyName)
{
	LONG recv;
	HKEY subKeyHandle;
	CRegistryKey * subKey;

	recv =
		::RegCreateKeyEx(
			key,
			subKeyName,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&subKeyHandle,
			NULL);

	if(recv == ERROR_SUCCESS)
	{
		// �쐬�����B

		subKey = new CRegistrySubKey(subKeyHandle);
	}
	else
	{
		// �쐬���s�B

		subKey = NULL;
	}

	return subKey;
}

/**
 * @brief �l���o�C�i���̌`�Ŏ擾�B
 *
 * @author kumagai
 *
 * @param name �l��
 *
 * @return �l�I�u�W�F�N�g
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryValue * CRegistryKey::GetValue(CString name)
{
	return new CRegistryValue(key, name);
}

/**
 * @brief REG_SZ �̒l���擾����B
 *
 * @author kumagai
 *
 * @param name �l�̖��O
 *
 * @return �擾�����l�ւ̃|�C���^/NULL=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CString * CRegistryKey::GetStringValue(CString name)
{
	CString * ret;

	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// �����B

		if(value.type == REG_SZ)
		{
			// �^�͐������B

			if(value.size > 0)
			{
				// �P�o�C�g�ł�����B

				ret = new CString((TCHAR *)value.data);
			}
			else
			{
				// �O�o�C�g�B

				ret = new CString();
			}
		}
	}

	return ret;
}

/**
 * @brief REG_MULTI_SZ �̒l���擾����B
 *
 * @author kumagai
 *
 * @param name �l�̖��O
 *
 * @return �擾�����l�ւ̃|�C���^/NULL=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CStringArray * CRegistryKey::GetStringArray(CString name)
{
	CStringArray * ret;
	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// �����B

		if(value.type == REG_MULTI_SZ)
		{
			// �^�͐������B

			if(value.size > 0)
			{
				// �P�o�C�g�ł�����B

				ret = new CNsvStringArray((TCHAR *)value.data, value.size);
			}
			else
			{
				// �O�o�C�g�B

				ret = new CStringArray();
			}
		}
	}

	return ret;
}

/**
 * @brief REG_DWORD �̒l���擾����B
 *
 * @author kumagai
 *
 * @param name �l�̖��O
 *
 * @return �擾�����l�ւ̃|�C���^/NULL=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
DWORD * CRegistryKey::GetDWordValue(CString name)
{
	DWORD * ret;

	CRegistryValue value(key, name);

	ret = NULL;
	if(value.success)
	{
		// �����B

		if(value.type == REG_DWORD)
		{
			// �^�͐������B

			if(value.size == sizeof(DWORD))
			{
				// �o�C�g���͐������B

				ret = new DWORD;
				::CopyMemory(ret, value.data, sizeof(DWORD));
			}
		}
	}

	return ret;
}

/**
 * @brief ������̒l���������ށB
 *
 * @author kumagai
 *
 * @param name �l�̖��O
 * @param value �l
 *
 * @return TRUE=����/FALSE=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::SetStringValue(CString name, CString value)
{
	LONG recv;

	recv =
		::RegSetValueEx(
			key,
			name,
			0,
			REG_SZ,
			(BYTE *)value.GetBuffer(),
			sizeof(TCHAR) * value.GetLength());

	return recv == ERROR_SUCCESS;
}

/**
 * @brief �����s������l�̏������݁B
 *
 * @author kumagai
 *
 * @param name �f�[�^��
 * @param values �l
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::SetStringArray(CString name, CStringArray * values)
{
	int i, j;
	int offset;
	LONG recv;
	TCHAR * buffer;
	DWORD size;
	CString string;

	// �o�b�t�@�T�C�Y�v�Z�B
	size = 0;
	for(i=0 ; i<values->GetSize() ; i++)
	{
		string = values->GetAt(i);
		size += string.GetLength() + 1;
	}
	size += 1;

	// �o�b�t�@�m�ۋy�уf�[�^�ݒu�B
	offset = 0;
	buffer = new TCHAR [size];
	for(i=0 ; i<values->GetSize() ; i++)
	{
		string = values->GetAt(i);

		for(j=0 ; j<string.GetLength() ; j++)
		{
			buffer[offset + j] = string[j];
		}

		offset += string.GetLength();
		buffer[offset] = '\0';
		offset++;
	}

	buffer[offset] = '\0';

	// �i�[���s�B
	recv =
		::RegSetValueEx
			(key, name, 0, REG_MULTI_SZ, (BYTE *)buffer, sizeof(TCHAR) * size);

	delete [] buffer;

	return recv == ERROR_SUCCESS;
}

/**
 * @brief ������̒l���������ށB
 *
 * @author kumagai
 *
 * @param name �l�̖��O
 * @param value �l
 *
 * @return TRUE=����/FALSE=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::SetDWordValue(CString name, DWORD value)
{
	LONG recv;

	recv =
		::RegSetValueEx(
			key,
			name,
			0,
			REG_DWORD,
			(BYTE *)&value,
			sizeof(DWORD));

	return recv == ERROR_SUCCESS;
}

/**
 * @brief �L�[�̍폜�B
 *
 * @author kumagai
 *
 * @param keyName �L�[��
 *
 * @return TRUE=����/FALSE=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::DeleteKey(CString keyName)
{
	LONG recv;

	recv = ::RegDeleteKey(key, keyName);

	return recv == ERROR_SUCCESS;
}

/**
 * @brief �l�̍폜�B
 *
 * @author kumagai
 *
 * @param name �l��
 *
 * @return TRUE=����/FALSE=���s
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
BOOL CRegistryKey::DeleteValue(CString name)
{
	LONG recv;

	recv = ::RegDeleteValue(key, name);

	return recv == ERROR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�L�[�l�̊��蓖�Ă��s���B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
CRegistryClassesRootKey::CRegistryClassesRootKey(void)
{
	key = HKEY_CLASSES_ROOT;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�L�[�l�̊��蓖�Ă��s���B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
CRegistryCurrentUserRootKey::CRegistryCurrentUserRootKey(void)
{
	key = HKEY_CURRENT_USER;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�L�[�l�̊��蓖�Ă��s���B
 *
 * @author kumagai
 *
 * @date 2011/07/04
 *	- kumagai:�V�K�쐬�B
 */
CRegistryLocalMachineRootKey::CRegistryLocalMachineRootKey(void)
{
	key = HKEY_LOCAL_MACHINE;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ���W�X�g�����[�g�L�[�̖��O�ƒl�̑΁B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
const struct RootKeyNameAndValue
{
	char * name;
	HKEY value;
}
CRegistryRootKey::rootKeyNameAndValue [] =
{
	{"HKEY_CLASSES_ROOT",			HKEY_CLASSES_ROOT		},
	{"HKEY_CURRENT_USER",			HKEY_CURRENT_USER		},
	{"HKEY_LOCAL_MACHINE",			HKEY_LOCAL_MACHINE		},
	{"HKEY_USERS",					HKEY_USERS				},
	{"HKEY_PERFORMANCE_DATA",		HKEY_PERFORMANCE_DATA	},
	{"HKEY_PERFORMANCE_TEXT",		HKEY_PERFORMANCE_TEXT	},
	{"HKEY_PERFORMANCE_NLSTEXT",	HKEY_PERFORMANCE_NLSTEXT},
	{"HKEY_CURRENT_CONFIG",			HKEY_CURRENT_CONFIG		},
	{"HKEY_DYN_DATA",				HKEY_DYN_DATA			}
};

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�w�背�W�X�g���p�X�̃��[�g�L�[�̃L�[�I�u
 * �W�F�N�g�����s���B
 *
 * @author kumagai
 *
 * @param path ���W�X�g���p�X
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistryRootKey::CRegistryRootKey(CString path)
{
	int i;
	CString root;

	if(path.Find(delimiter) >= 0)
	{
		// �f���~�^���܂ށB�t���p�X�Ƃ݂Ȃ��B

		root = path.Left(path.Find(delimiter));
	}
	else
	{
		// �f���~�^���܂܂Ȃ��B���[�g�L�[�݂̂�\��������Ƃ݂Ȃ��B

		root = path;
	}

	this->key = NULL;
	for(i=0 ; i<sizeof(rootKeyNameAndValue)/sizeof(RootKeyNameAndValue) ; i++)
	{
		if(root == rootKeyNameAndValue[i].name)
		{
			// ���[�g�L�[���͈�v����B

			this->key = rootKeyNameAndValue[i].value;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief �I�u�W�F�N�g�̍\�z�ƂƂ��ɁA�w��L�[�n���h���̏W����s���B
 *
 * @author kumagai
 *
 * @param key ���W�X�g���L�[
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistrySubKey::CRegistrySubKey(HKEY key)
{
	this->key = key;
}

/**
 * @brief �I�u�W�F�N�g�̔j���ƂƂ��ɁA�W�񂵂Ă������W�X�g���L�[�̃N���[�Y���s���B
 *
 * @author kumagai
 *
 * @date 2008/03/26
 *	- kumagai:�V�K�쐬�B
 */
CRegistrySubKey::~CRegistrySubKey()
{
	LONG recv;

	recv = ::RegCloseKey(key);

	ASSERT(recv == ERROR_SUCCESS);
}
