
#ifndef _MACHINE_OPER_H_PERFECTPLUG_
#define _MACHINE_OPER_H_PERFECTPLUG_
#include "PPBase.h"
BOOL fGetAllMACAndIPSimplist(_Out_ std::vector<PSTRUCT_MACHINE_MAC_IP> pvstructMachineMACIP);
int fGetAllMACAndIPArraySimplist(_Out_ STRUCT_MACHINE_MAC_IP structMachineMACIP[10]);
BOOL fGetAllMACAndIP(_Out_ std::vector<PSTRUCT_MACHINE_MAC_IP> pvstructMachineMACIP);
int fGetAllMACAndIPArray(_Out_ STRUCT_MACHINE_MAC_IP structMachineMACIP[10]);
void fGetTimeStringBytm(time_t nowtime, char szTimes[50]);
void fGetTimeStringBytmAll(time_t nowtime, char szTimes[50]);
void fGetSystemNowTime(char szTimes[50]);
void fGetSystemStartTime(char szTimes[50]);
void fGetSystemRunTime(char szTimes[50]);
time_t fGetSystemStartTime_t();
#endif