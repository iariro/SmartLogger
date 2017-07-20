#include "stdafx.h"
#include "Windows.h"

#include "Log/Str.h"
#include "Log/StrArray.h"
#include "Log/SmartLoggerLogData.h"
#include "Log/SmartLoggerLogging.h"

// CreatePipe�֐�
// http://www.ne.jp/asahi/hishidama/home/tech/c/windows/CreatePipe.html

HANDLE readPipe  = NULL;
HANDLE writePipe = NULL;
HANDLE childProcess = NULL;

void printError(const char *message)
{
	LPVOID lpvMessageBuffer;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvMessageBuffer,
		0,
		NULL);
	fprintf(stderr, "%s: (%d) %s\n", message, GetLastError(), lpvMessageBuffer);
	LocalFree(lpvMessageBuffer);
}

int createPipe(void)
{
	HANDLE readTemp;

	// �p�C�v���쐬�i���n���h���Ƃ��q�v���Z�X�֌p���s�j
	if (!CreatePipe(&readTemp, &writePipe, NULL, 0)) {
		printError("CreatePipe");
		return -1;
	}

	// �Ǎ��n���h���𕡐��i�q�v���Z�X�֌p���\�Ȍ����̓Ǎ��n���h�����쐬�j
	if (!DuplicateHandle(
		GetCurrentProcess(), readTemp,
		GetCurrentProcess(), &readPipe,
		0, TRUE, DUPLICATE_SAME_ACCESS)
	) {
		printError("DuplicateHandle");
		if (!CloseHandle(readTemp)) {
			printError("CloseHandle(readTemp)");
		}
		return -1;
	}

	// �������̃n���h���͎g��Ȃ��̂ŃN���[�Y
	if (!CloseHandle(readTemp)) {
		printError("CloseHandle(readTemp)");
		return -1;
	}

	return 0;
}

int SendtoFluentCat(CString fluentcatPath, CString sensorName, CString address, char * message)
{
	createPipe();

	TCHAR commandLine [BUFSIZ];

	swprintf_s(
		commandLine,
		_T("%s %s -h %s --time-as-integer"),
		fluentcatPath,
		sensorName,
		address);
	//CSmartLoggerLogging::PutLog(new CSendtoFluentdLog1(commandLine));

	BOOL  bInheritHandles = TRUE;
	DWORD creationFlags   = 0;

	STARTUPINFO si = {};
	si.cb         = sizeof(STARTUPINFO);
	si.dwFlags    = STARTF_USESTDHANDLES;
	si.hStdInput  = readPipe;	//�q�v���Z�X�̕W�����͓͂Ǎ��p�p�C�v�ɂ���
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE); //�e�v���Z�X�̕W���o�͂��p��
	si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);  //�e�v���Z�X�̕W���G���[���p��

	if (si.hStdOutput == INVALID_HANDLE_VALUE) {
		printError("GetStdHandle(STD_OUTPUT_HANDLE)");
		return -1;
	}
	if (si.hStdError == INVALID_HANDLE_VALUE) {
		printError("GetStdHandle(STD_ERROR_HANDLE)");
		return -1;
	}

	// �q�v���Z�X���N��
	PROCESS_INFORMATION pi = {};
	if (!CreateProcess(
		NULL,
		commandLine,
		NULL,	//�v���Z�X�̃Z�L�����e�B�[�L�q�q
		NULL,	//�X���b�h�̃Z�L�����e�B�[�L�q�q
		bInheritHandles,
		creationFlags,
		NULL,	//���ϐ��͈����p��
		NULL,	//�J�����g�f�B���N�g���[�͓���
		&si,
		&pi)
	) {
		printError("CreateProcess");
		return -1;
	}
	childProcess = pi.hProcess;
	if (!CloseHandle(pi.hThread)) {
		printError("CloseHandle(hThread)");
	}

	// �q�v���Z�X���N��������e�v���Z�X���ł͓Ǎ��n���h�����g��Ȃ��̂ŁA�N���[�Y����
	if(!CloseHandle(readPipe)) {
		printError("CloseHandle(readPipe)");
	}
	readPipe = NULL;

	// �p�C�v�ւ̏������݁i�q�v���Z�X�ւ̑��M�j
	DWORD numberOfBytesWritten;
	if (!WriteFile(writePipe, message, strlen(message), &numberOfBytesWritten, NULL)) {
		printError("WriteFile");
		return -1;
	}
	if(!CloseHandle(writePipe)) {
		printError("CloseHandle(writePipe)");
		return -1;
	}
	writePipe = NULL;

	// �q�v���Z�X�̏I���҂�
	DWORD r = WaitForSingleObject(childProcess, INFINITE);
	/*
	switch(r) {
	case WAIT_FAILED:
		printError("wait result=WAIT_FAILED");
		return -1;
	case WAIT_ABANDONED:
		printf("wait result=WAIT_ABANDONED\n");
		return -1;
	case WAIT_OBJECT_0: //����I��
		printf("wait result=WAIT_OBJECT_0\n");
		break;
	case WAIT_TIMEOUT:
		printf("wait result=WAIT_TIMEOUT\n");
		return -1;
	default:
		printf("wait result=%d\n", r);
		return -1;
	}
	*/
	return r;
}
