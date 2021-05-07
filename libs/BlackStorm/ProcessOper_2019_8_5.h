
#ifndef _PROCESS_OPER_2019_8_5_H_PERFECTPLUG_
#define _PROCESS_OPER_2019_8_5_H_PERFECTPLUG_

#include "PPBase.h"

namespace ProcessOper_2019_8_5 {
	//获取当前进程关机权限
	void TokenShutDownPrivileges();
	//根据指定进程句柄获取关机权限
	void TokenShutDownPrivileges(HANDLE hProcess);
};
#endif