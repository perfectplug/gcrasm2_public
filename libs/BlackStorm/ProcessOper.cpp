
#include "ProcessOper.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#include <TlHelp32.h>
#include <string>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
//函数名：fGetProcessIdVectorByName
//功能说明：通过进程名获取ID号列表
//参数：
//	pszProcName (TCHAR *)：指定进程名
//返回值：
//	std::vector<DWORD>
std::vector<DWORD> fGetProcessIdVectorByName(const TCHAR *pszProcName)
{
#ifdef _BS_SEH_
	try
	{
#endif // _BS_SEH_
		std::vector<DWORD> vProcessIds;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return vProcessIds;
		}
		HANDLE hProcess = NULL;
		BOOL bMore = ::Process32First(hProcessSnap, &pe32);
		while (bMore)
		{
			if (_tcsicmp(pe32.szExeFile, pszProcName) == NULL) {
				vProcessIds.push_back(pe32.th32ProcessID) ;
			}
			bMore = ::Process32Next(hProcessSnap, &pe32);
		}
		return vProcessIds;
#ifdef _BS_SEH_
	}
	catch (...)
	{
		return 0;
	}
#endif // _BS_SEH_
}

void fTokenSEPrivileges()
{
#ifdef _BS_SEH_
	try
	{
#endif // _BS_SEH_
		//提升Debug权限
		HANDLE hT = INVALID_HANDLE_VALUE ;
		BOOL bret = FALSE ;
		if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hT))
		{
			TOKEN_PRIVILEGES tp; 
			tp.PrivilegeCount=1; 
			if (LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))
			{
				tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED; 
				if (AdjustTokenPrivileges(hT,FALSE,&tp,sizeof(tp),NULL,NULL))
				{
				}
				else
				{
					//fTraceLogAppend(m_szTraceLogFileName,TEXT("fTokenSEPrivileges AdjustTokenPrivileges failed\r\n")) ;
				}
			}
			else
			{
				//fTraceLogAppend(m_szTraceLogFileName,TEXT("fTokenSEPrivileges LookupPrivilegeValue failed\r\n")) ;
			}
		}
		else
		{
			//fTraceLogAppend(m_szTraceLogFileName,TEXT("fTokenSEPrivileges OpenProcessToken failed\r\n")) ;
		}
#ifdef _BS_SEH_
	}
	catch(...)
	{
	}
#endif // _BS_SEH_
}

//获取指定进程的所有模块名
std::vector<cstring> fGetModuleNameVectorEx(HANDLE hProcess)
{
	std::vector<cstring> vModuleNames;
	DWORD cbNeeded = 0;
	TCHAR szModuleName[MAX_PATH] = { 0 };
	if (EnumProcessModules(hProcess, 0, 0, &cbNeeded))
	{
		HMODULE* hMods = (HMODULE*)malloc(cbNeeded * sizeof(HMODULE));
		if (EnumProcessModules(hProcess, hMods, cbNeeded, &cbNeeded))
		{
			for (unsigned int i = 0; i < cbNeeded / sizeof(HMODULE); i++)
			{
				szModuleName[0] = 0;
				if (GetModuleFileNameEx(hProcess, hMods[i], szModuleName, _countof(szModuleName)))
				{
					TCHAR* dllName = _tcsrchr(szModuleName, TEXT('\\'));
					TCHAR tcsBuf[MAX_PATH];
					if (dllName)
					{
						dllName++;
						_tcscpy(tcsBuf, dllName);
						vModuleNames.push_back(tcsBuf);
					}
				}
			}
		}
		free(hMods);
	}
	return vModuleNames;
}

//////////////////////////////////////////////////////////////////////////
//函数名：fGetProcessIdByName
//功能说明：通过进程名获取ID号
//参数：
//	pszProcName (TCHAR *)：指定进程名
//返回值：
//	DWORD
// -1 ：CreateToolhelp32Snapshot failed
// -2 ：find the process failed
// >0 ：Process Id success
DWORD fGetProcessIdByName(const TCHAR *pszProcName)
{
#ifdef _BS_SEH_
	try
	{
#endif // _BS_SEH_
		CString strTmpBuf ,strTmpBuf1 ;
		PROCESSENTRY32 pe32;
		pe32.dwSize=sizeof(pe32);
		HANDLE hProcessSnap=::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		if(hProcessSnap==INVALID_HANDLE_VALUE){
			return 0;
		}
		HANDLE hProcess = NULL;
		BOOL bMore=::Process32First(hProcessSnap,&pe32);
		while(bMore)
		{
			strTmpBuf.Format(_TEXT("%s"),pe32.szExeFile) ;
			strTmpBuf.MakeLower() ;
			strTmpBuf1.Format(TEXT("%s") ,pszProcName) ;
			strTmpBuf1.MakeLower() ;
			if (_tcscmp(strTmpBuf1.GetBuffer() ,strTmpBuf.GetBuffer()) == NULL){
				return pe32.th32ProcessID ;
			}
			bMore=::Process32Next(hProcessSnap,&pe32);
		}
		return 0 ;
#ifdef _BS_SEH_
	}
	catch(...)
	{
		return 0 ;
	}
#endif // _BS_SEH_
}

//获取当前可执行程序路径的目录
int fGetCurrentPathDirectoryA(_Out_ char szCurDirectory[])
{
#ifdef _BS_SEH_
	try
	{
#endif // _BS_SEH_
		int iret = 0;
		iret = GetModuleFileNameA(NULL, szCurDirectory, MAX_PATH);
		if (iret == 0) {
			return 1;
		}
		for (int i = strlen(szCurDirectory) - 1; i >= 0; i--) {
			if (szCurDirectory[i] != ('\\')) {
				szCurDirectory[i] = 0;
			}
			else {
				break;
			}
		}
		return 0;
#ifdef _BS_SEH_
	}
	catch (...)
	{
		return 0;
	}
#endif // _BS_SEH_
}

int fGetProcessFullPathByProcessId(DWORD dwProcessId_in, TCHAR szFullPath_out[])
{
#ifdef _BS_SEH_
	try
	{
#endif // _BS_SEH_
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId_in);
		if (hSnapshot == INVALID_HANDLE_VALUE)
			return -1;
		MODULEENTRY32 hModuleInfo;
		memset(&hModuleInfo, 0, sizeof(MODULEENTRY32));
		hModuleInfo.dwSize = sizeof(MODULEENTRY32);
		BOOL iReb = Module32First(hSnapshot, &hModuleInfo);
		if (iReb == FALSE)
			return -1;
		wsprintf(szFullPath_out, TEXT("%s"), hModuleInfo.szExePath);
		CloseHandle(hSnapshot);
		return 1;
#ifdef _BS_SEH_
	}
	catch (...)
	{
		return -1;
	}
#endif // _BS_SEH_
}