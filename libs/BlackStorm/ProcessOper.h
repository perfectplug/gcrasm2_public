#ifndef _PROCESS_OPER_H_
#define _PROCESS_OPER_H_
#include "PPBase.h"

std::vector<DWORD> fGetProcessIdVectorByName(const TCHAR *pszProcName);
void fTokenSEPrivileges();
//��ȡָ�����̵�����ģ����
std::vector<cstring> fGetModuleNameVectorEx(HANDLE hProcess);
DWORD fGetProcessIdByName(const TCHAR *pszProcName);
//��ȡ��ǰ��ִ�г���·����Ŀ¼
int fGetCurrentPathDirectoryA(_Out_ char szCurDirectory[]);
int fGetProcessFullPathByProcessId(DWORD dwProcessId_in, TCHAR szFullPath_out[]);
#endif