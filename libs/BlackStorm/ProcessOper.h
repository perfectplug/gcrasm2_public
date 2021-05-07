#ifndef _PROCESS_OPER_H_
#define _PROCESS_OPER_H_
#include "PPBase.h"

std::vector<DWORD> fGetProcessIdVectorByName(const TCHAR *pszProcName);
void fTokenSEPrivileges();
//获取指定进程的所有模块名
std::vector<cstring> fGetModuleNameVectorEx(HANDLE hProcess);
DWORD fGetProcessIdByName(const TCHAR *pszProcName);
//获取当前可执行程序路径的目录
int fGetCurrentPathDirectoryA(_Out_ char szCurDirectory[]);
int fGetProcessFullPathByProcessId(DWORD dwProcessId_in, TCHAR szFullPath_out[]);
#endif