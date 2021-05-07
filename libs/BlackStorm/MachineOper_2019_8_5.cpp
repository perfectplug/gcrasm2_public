
#include "MachineOper_2019_8_5.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "ProcessOper.h"
#include "ProcessOper_2019_8_5.h"

namespace MachineOper_2019_8_5 {

	//关掉Windows
	BOOL ShutDownWindows()
	{
		//获取关机权限
		ProcessOper_2019_8_5::TokenShutDownPrivileges();
		// 强制关闭计算机
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) {
			return false;
		}
		return true;
	}

	//重启计算机
	BOOL RestartWindows()
	{
		//获取关机权限
		ProcessOper_2019_8_5::TokenShutDownPrivileges();
		// 强制重启计算机
		if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
			return false;
		return true;
	}
};