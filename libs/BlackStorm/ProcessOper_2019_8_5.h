
#ifndef _PROCESS_OPER_2019_8_5_H_PERFECTPLUG_
#define _PROCESS_OPER_2019_8_5_H_PERFECTPLUG_

#include "PPBase.h"

namespace ProcessOper_2019_8_5 {
	//��ȡ��ǰ���̹ػ�Ȩ��
	void TokenShutDownPrivileges();
	//����ָ�����̾����ȡ�ػ�Ȩ��
	void TokenShutDownPrivileges(HANDLE hProcess);
};
#endif