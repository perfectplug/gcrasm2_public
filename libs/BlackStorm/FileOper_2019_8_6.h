#ifndef _FILE_OPER_2019_8_6_H_
#define _FILE_OPER_2019_8_6_H_
#include "PPBase.h"

namespace FileOper_2019_8_6{
	//简单的读取文件函数
	BOOL ReadFileA(_In_ CONST char filePath[MAX_PATH],_Out_ char *&buffer,_Out_ DWORD &dwBufLen);
};

#endif