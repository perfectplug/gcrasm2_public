#ifndef _2019_8_5_IP_OPER_H_PERFECTPLUG_
#define _2019_8_5_IP_OPER_H_PERFECTPLUG_
#include "PPBase.h"

namespace _2019_8_5_IPOper {
	//通过IP字符串获取DWORD类型IP
	int GetAddressA(_In_ char szIp[MAX_PATH], _Out_ DWORD& dwIp);
};


#endif