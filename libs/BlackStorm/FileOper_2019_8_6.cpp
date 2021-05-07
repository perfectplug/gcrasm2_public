
#include "FileOper_2019_8_6.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#include <TlHelp32.h>
#include <tchar.h>

namespace FileOper_2019_8_6{

	//简单的读取文件函数
	BOOL ReadFileA(_In_ CONST char filePath[MAX_PATH],_Out_ char *&buffer,_Out_ DWORD &dwBufLen)
	{
		BOOL bRet = FALSE;
		HANDLE pFile;
		pFile = CreateFileA(filePath,GENERIC_READ,          
			FILE_SHARE_READ,
			NULL,               
			OPEN_EXISTING,        
			FILE_ATTRIBUTE_NORMAL, 
			NULL);

		if ( pFile == INVALID_HANDLE_VALUE)
		{
			printf("create file error!\n");
			CloseHandle(pFile);
			return FALSE;
		}
		DWORD dwFileSizeHigh=0;
		DWORD dwFileSize = GetFileSize(pFile,&dwFileSizeHigh);
		printf("File Size is:%d\r\n",dwFileSize);
		dwBufLen = dwFileSize;
		buffer = new char[dwFileSize+1];
		ZeroMemory(buffer,dwFileSize+1);
		DWORD dwBytesRead =0 ;

		if(::ReadFile(pFile,buffer,dwFileSize,&dwBytesRead,NULL))
		{
			bRet = TRUE;
			printf("ReadFile Success\r\n");
		}
		else
		{
			printf("ReadFile error!\n");
			bRet = FALSE;
		}
		CloseHandle(pFile);
		return bRet;
	}
};