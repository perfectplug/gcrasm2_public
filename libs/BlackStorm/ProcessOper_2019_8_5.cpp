

#include "ProcessOper_2019_8_5.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")
#ifdef VMP
#include "VMProtectSDK.h"
#endif // VMP
#include <winternl.h>


namespace ProcessOper_2019_8_5 {

	//��ȡ��ǰ���̹ػ�Ȩ��
	void TokenShutDownPrivileges()
	{
		return TokenShutDownPrivileges(GetCurrentProcess());
	}

	//����ָ�����̾����ȡ�ػ�Ȩ��
	void TokenShutDownPrivileges(HANDLE hProcess)
	{
#ifdef _BS_SEH_
		try
		{
#endif // _BS_SEH_
			//����DebugȨ��
			HANDLE hT = INVALID_HANDLE_VALUE;
			BOOL bret = FALSE;
			if (OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hT))
			{
				TOKEN_PRIVILEGES tp;
				tp.PrivilegeCount = 1;
				if (LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid))
				{
					tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
					if (AdjustTokenPrivileges(hT, FALSE, &tp, sizeof(tp), NULL, NULL))
					{
						_tprintf(TEXT("fTokenSEPrivileges AdjustTokenPrivileges success\r\n"));
					}
					else
					{
						_tprintf(TEXT("fTokenSEPrivileges AdjustTokenPrivileges failed\r\n"));
					}
				}
				else
				{
					_tprintf(TEXT("fTokenSEPrivileges LookupPrivilegeValue failed\r\n"));
				}
			}
			else
			{
				_tprintf(TEXT("fTokenSEPrivileges OpenProcessToken failed\r\n"));
			}
#ifdef _BS_SEH_
		}
		catch (...)
		{
		}
#endif // _BS_SEH_
	}

};