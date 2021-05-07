#ifndef _PP_BASE_H_
#define _PP_BASE_H_

//C++
#include <string>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <list>

//C
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

//WINDOWS
#include <ws2tcpip.h>
#include <winsock2.h>    
#pragma comment(lib, "WS2_32")     
#include <winsvc.h>
#include <tlhelp32.h>
#include <atlstr.h>
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib,"Version")
#include <comdef.h>
#include <gdiplus.h>
#include <Assert.h>
#include <wincrypt.h>
#include <strsafe.h>

//Libs
// #include "./Libcurl/include/curl/curl.h"
// #include "./jsoncpp/include/json/json.h"
// 
// #ifdef _DEBUG
// #   pragma comment(lib, "./jsoncpp/LIB Debug/JsonCpp_mtd.lib")
// #   pragma comment(lib, "./Libcurl/lib/libcurld_mt_s_ssl.lib")
// #else
// #   pragma comment(lib, "./jsoncpp/LIB Release/JsonCpp_mt.lib")
// #   pragma comment(lib, "./Libcurl/lib/libcurl_mt_s_ssl.lib")
// #endif


#ifdef _UNICODE
#define cstring  std::wstring
#define ccout  std::wcout
#else
#define cstring  std::string
#define ccout  std::cout
#endif

//结构体相关
typedef struct _struct_machine_mac_ip_
{
	char szIP[30];
	char szMAC[30];
}STRUCT_MACHINE_MAC_IP, *PSTRUCT_MACHINE_MAC_IP;


#endif
