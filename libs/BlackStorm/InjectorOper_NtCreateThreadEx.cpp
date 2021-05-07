//Use NtCreateThreadEx to inject dll

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <tlhelp32.h>
#pragma comment(lib,"Advapi32.lib") 
#include "ntdll.h"


BOOL InjectDll(UINT32 ProcessId, const TCHAR *DllFullPath)
{
	
	if (_tcsstr(DllFullPath, TEXT("\\\\")) != 0)
	{
		printf("[!]Wrong Dll path\n");
		return FALSE;
	}
	
	if (_tcsstr(DllFullPath, TEXT("\\")) == 0)
	{
		printf("[!]Need Dll full path\n");
		return FALSE;
	}

	HANDLE ProcessHandle = NULL;

	ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	if (ProcessHandle == NULL)
	{
		printf("[!]OpenProcess error\n");
		return FALSE;
	}

	UINT32 DllFullPathLength = (_tcslen(DllFullPath)*sizeof(TCHAR) + 1);
	PVOID DllFullPathBufferData = VirtualAllocEx(ProcessHandle, NULL, DllFullPathLength, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (DllFullPathBufferData == NULL)
	{
		CloseHandle(ProcessHandle);
		printf("[!]DllFullPathBufferData error\n");
		return FALSE;
	}
	SIZE_T ReturnLength;
	BOOL bOk = WriteProcessMemory(ProcessHandle, DllFullPathBufferData, DllFullPath, DllFullPathLength, &ReturnLength);

	LPTHREAD_START_ROUTINE LoadLibraryAddress = NULL;
	HMODULE Kernel32Module = GetModuleHandleA("Kernel32");
#ifdef _UNICODE
	LoadLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(Kernel32Module, "LoadLibraryW");
#else
	LoadLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(Kernel32Module, "LoadLibraryA");
#endif
	pfnNtCreateThreadEx NtCreateThreadEx = (pfnNtCreateThreadEx)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
	if (NtCreateThreadEx == NULL)
	{
		CloseHandle(ProcessHandle);
		printf("[!]NtCreateThreadEx error\n");
		return FALSE;
	}
	HANDLE ThreadHandle = NULL;
	NtCreateThreadEx(&ThreadHandle, 0x1FFFFF, NULL, ProcessHandle, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, DllFullPathBufferData, FALSE, NULL, NULL, NULL, NULL);
	if (ThreadHandle == NULL)
	{
		CloseHandle(ProcessHandle);
		printf("[!]ThreadHandle error\n");
		return FALSE;
	}
	if (WaitForSingleObject(ThreadHandle, INFINITE) == WAIT_FAILED)
	{
		printf("[!]WaitForSingleObject error\n");
		return FALSE;
	}
	CloseHandle(ProcessHandle);
	CloseHandle(ThreadHandle);
	return TRUE;
}


BOOL FreeDll(UINT32 ProcessId, TCHAR *DllFullPath)
{
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	BOOL bSuccess = FALSE;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);
	bMore = Module32First(hSnapshot, &me);
	for (; bMore; bMore = Module32Next(hSnapshot, &me)) {
		if (!_tcsicmp((LPCTSTR)me.szModule, DllFullPath) || !_tcsicmp((LPCTSTR)me.szExePath, DllFullPath))
		{
			bFound = TRUE;
			break;
		}
	}
	if (!bFound) {
		CloseHandle(hSnapshot);
		return FALSE;
	}

	HANDLE ProcessHandle = NULL;

	ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);

	if (ProcessHandle == NULL)
	{
		printf("[!]OpenProcess error\n");
		return FALSE;
	}

	LPTHREAD_START_ROUTINE FreeLibraryAddress = NULL;
	HMODULE Kernel32Module = GetModuleHandleA("Kernel32");
	FreeLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(Kernel32Module, "FreeLibrary");
	pfnNtCreateThreadEx NtCreateThreadEx = (pfnNtCreateThreadEx)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
	if (NtCreateThreadEx == NULL)
	{
		CloseHandle(ProcessHandle);
		printf("[!]NtCreateThreadEx error\n");
		return FALSE;
	}
	HANDLE ThreadHandle = NULL;

	NtCreateThreadEx(&ThreadHandle, 0x1FFFFF, NULL, ProcessHandle, (LPTHREAD_START_ROUTINE)FreeLibraryAddress, me.modBaseAddr, FALSE, NULL, NULL, NULL, NULL);
	if (ThreadHandle == NULL)
	{
		CloseHandle(ProcessHandle);
		printf("[!]ThreadHandle error\n");
		return FALSE;
	}
	if (WaitForSingleObject(ThreadHandle, INFINITE) == WAIT_FAILED)
	{
		printf("[!]WaitForSingleObject error\n");
		return FALSE;
	}
	CloseHandle(ProcessHandle);
	CloseHandle(ThreadHandle);
	return TRUE;
}

BOOL EnableDebugPrivilege(BOOL fEnable)
{
	BOOL fOk = FALSE;
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return(fOk);
}

BOOL fInjectorByNtCreateThreadEx(DWORD dwProcessId, const TCHAR *DllFullPath)
{
	printf("Use NtCreateThreadEx to inject dll\n");
	if (!EnableDebugPrivilege(TRUE))
	{
		printf("[!]AdjustTokenPrivileges Failed.<%d>\n", GetLastError());
	}
	if (!InjectDll(dwProcessId, DllFullPath))
	{
		printf("[!]InjectDll error \n");
		return FALSE;
	}
	printf("[+]InjectDll success\n");
	return TRUE;
}

//int main(int argc, char *argv[])
//{
//	printf("Use NtCreateThreadEx to inject dll\n\n");
//	if (!EnableDebugPrivilege(TRUE))
//	{
//		printf("[!]AdjustTokenPrivileges Failed.<%d>\n", GetLastError());
//	}
//
//	if (argc != 3)
//	{
//		printf("Usage:\n");
//		printf("%s <PID> <Dll Full Path>\n", argv[0]);
//		return 0;
//	}
//	if (!InjectDll((DWORD)atoi(argv[1]), argv[2]))
//	{
//		printf("[!]InjectDll error \n");
//		return 1;
//	}
//
//	if (!FreeDll((DWORD)atoi(argv[1]), argv[2]))
//	{
//		printf("[!]FreeDll error \n");
//		return 1;
//	}
//	printf("[+]InjectDll success\n");
//	return 0;
//}