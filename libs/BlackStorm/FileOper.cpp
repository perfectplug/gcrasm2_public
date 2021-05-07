
#include "FileOper.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#include <TlHelp32.h>
#include <string>
#include <tchar.h>

BOOL MyWriteFile(CONST TCHAR filePath[MAX_PATH],char *buffer, DWORD contentLen)
{
	HANDLE pFile;
	char *tmpBuf;
	DWORD dwBytesWrite,dwBytesToWrite;

	pFile = CreateFile(filePath,GENERIC_WRITE,          
		0,
		NULL,               
		CREATE_ALWAYS,        //���Ǵ����ļ�
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	if ( pFile == INVALID_HANDLE_VALUE)
	{
		printf("create file error!\n");
		CloseHandle(pFile);
		return FALSE;
	}

	dwBytesToWrite = contentLen;
	dwBytesWrite = 0;

	tmpBuf = buffer;

	do{                                       //ѭ��д�ļ���ȷ���������ļ���д��  

		WriteFile(pFile,tmpBuf,dwBytesToWrite,&dwBytesWrite,NULL);

		dwBytesToWrite -= dwBytesWrite;
		tmpBuf += dwBytesWrite;

	} while (dwBytesToWrite > 0);

	CloseHandle(pFile);

	return TRUE;
}