/*
Author:perfectplug
time:2019-05-19 16:59
function:ע����ز���
*/
#ifndef _INJECTOR_NTCREATETHREAD_OPER_H_PERFECTPLUG_
#define _INJECTOR_NTCREATETHREAD_OPER_H_PERFECTPLUG_
#include <Windows.h>

BOOL fInjectorByNtCreateThreadEx(DWORD dwProcessId, const TCHAR *DllFullPath);
#endif