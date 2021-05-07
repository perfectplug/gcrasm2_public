#include "2019_8_5_IPOper.h"
#include "IString.h"

namespace _2019_8_5_IPOper {
	//通过IP字符串获取DWORD类型IP
	int GetAddressA(_In_ char szIp[MAX_PATH], _Out_ DWORD& dwIp)
	{
		std::vector<std::string> strvIp;
		strvIp = UtilString::split(szIp, ".");
		dwIp = atoi(strvIp[0].c_str()) * 0x1000000 + \
			atoi(strvIp[1].c_str()) * 0x10000 + \
			atoi(strvIp[2].c_str()) * 0x100 + \
			atoi(strvIp[3].c_str()) * 0x1;
		return 0;
	}
};
