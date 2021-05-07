
#include "MachineOper_2019_8_5.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "ProcessOper.h"
#include "ProcessOper_2019_8_5.h"

namespace MachineOper_2019_8_5 {

	//�ص�Windows
	BOOL ShutDownWindows()
	{
		//��ȡ�ػ�Ȩ��
		ProcessOper_2019_8_5::TokenShutDownPrivileges();
		// ǿ�ƹرռ����
		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) {
			return false;
		}
		return true;
	}

	//���������
	BOOL RestartWindows()
	{
		//��ȡ�ػ�Ȩ��
		ProcessOper_2019_8_5::TokenShutDownPrivileges();
		// ǿ�����������
		if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
			return false;
		return true;
	}
};