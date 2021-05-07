
#include "MachineOper.h"
#include <stdio.h>
#include <time.h>


#define PRINT_ALL    0xff
#define PRINT_DATE    (1 << 0)
#define PRINT_TIME    (1 << 1)

/*
demo:
	vector<STRUCT_MACHINE_MAC_IP> vector_struct_machine_mac_ip;
	BS->Machine->GetAllMACAndIP(vector_struct_machine_mac_ip);
*/
BOOL fGetAllMACAndIP(_Out_ std::vector<PSTRUCT_MACHINE_MAC_IP> pvstructMachineMACIP)
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	char *mac_addr = (char*)malloc(18);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) 
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		free(mac_addr);
		return FALSE; // it is safe to call free(NULL)
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) 
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return FALSE;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) 
	{
		// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;
			STRUCT_MACHINE_MAC_IP *pstruct_machine_mac_ip = new STRUCT_MACHINE_MAC_IP;
			memset(pstruct_machine_mac_ip, 0, sizeof(STRUCT_MACHINE_MAC_IP));
			strcpy(pstruct_machine_mac_ip->szIP, pAdapterInfo->IpAddressList.IpAddress.String);
			strcpy(pstruct_machine_mac_ip->szMAC, mac_addr);
			pvstructMachineMACIP.push_back(pstruct_machine_mac_ip);

			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);
	free(mac_addr);
	return TRUE; // caller must free.
}

BOOL fGetAllMACAndIPSimplist(_Out_ std::vector<PSTRUCT_MACHINE_MAC_IP> pvstructMachineMACIP)
{
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	char *mac_addr = (char*)malloc(18);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) 
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		free(mac_addr);
		return FALSE; // it is safe to call free(NULL)
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) 
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return FALSE;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) 
	{
		// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;
			STRUCT_MACHINE_MAC_IP *pstruct_machine_mac_ip = new STRUCT_MACHINE_MAC_IP;
			memset(pstruct_machine_mac_ip, 0, sizeof(STRUCT_MACHINE_MAC_IP));
			strcpy(pstruct_machine_mac_ip->szIP, pAdapterInfo->IpAddressList.IpAddress.String);
			strcpy(pstruct_machine_mac_ip->szMAC, mac_addr);
			pvstructMachineMACIP.push_back(pstruct_machine_mac_ip);

			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);
	free(mac_addr);
	return TRUE; // caller must free.
}

int fGetAllMACAndIPArray(_Out_ STRUCT_MACHINE_MAC_IP structMachineMACIP[10])
{
	int icount = 0;
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	char *mac_addr = (char*)malloc(18);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL)
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		free(mac_addr);
		return 0; // it is safe to call free(NULL)
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return 0;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		int i = 0;
		// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;
			strcpy(structMachineMACIP[i].szIP, pAdapterInfo->IpAddressList.IpAddress.String);
			strcpy(structMachineMACIP[i].szMAC, mac_addr);
			i++;
			icount++;
			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);
	free(mac_addr);
	return icount; // caller must free.
}

int fGetAllMACAndIPArraySimplist(_Out_ STRUCT_MACHINE_MAC_IP structMachineMACIP[10])
{
	int icount = 0;
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	char *mac_addr = (char*)malloc(18);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL)
	{
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		free(mac_addr);
		return 0; // it is safe to call free(NULL)
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(AdapterInfo);
		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL)
		{
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			free(mac_addr);
			return 0;
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR)
	{
		int i = 0;
		// Contains pointer to current adapter info
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		do {
			// technically should look at pAdapterInfo->AddressLength
			//   and not assume it is 6.
			sprintf(mac_addr, "%02X%02X%02X%02X%02X%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			// print them all, return the last one.
			// return mac_addr;
			strcpy(structMachineMACIP[i].szIP, pAdapterInfo->IpAddressList.IpAddress.String);
			strcpy(structMachineMACIP[i].szMAC, mac_addr);
			i++;
			icount++;
			printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);
	free(mac_addr);
	return icount; // caller must free.
}

void fGetTimeStringBytm(time_t nowtime,char szTimes[50])
{
	struct tm *timeinfo;
	timeinfo = localtime(&nowtime);

	int year, month, day, hour, min, sec;
	year = timeinfo->tm_year + 1900;
	month = timeinfo->tm_mon + 1;
	day = timeinfo->tm_mday;
	hour = timeinfo->tm_hour;
	min = timeinfo->tm_min;
	sec = timeinfo->tm_sec;

	sprintf(szTimes,"%02d:%02d:%02d", hour, min, sec);
}

void fGetTimeStringBytmAll(time_t nowtime, char szTimes[50])
{
	struct tm *timeinfo;
	timeinfo = localtime(&nowtime);

	int year, month, day, hour, min, sec;
	year = timeinfo->tm_year + 1900;
	month = timeinfo->tm_mon + 1;
	day = timeinfo->tm_mday;
	hour = timeinfo->tm_hour;
	min = timeinfo->tm_min;
	sec = timeinfo->tm_sec;

	sprintf(szTimes, "%d-%d-%d %02d:%02d:%02d", year, month, day, hour, min, sec);
}

void fGetSystemNowTime(char szTimes[50])
{
	time_t nowtime;
	time(&nowtime);
	fGetTimeStringBytm(nowtime, szTimes);
}

void fGetSystemStartTime(char szTimes[50])
{
	int iRunTime = GetTickCount();
	time_t nowtime;
	time(&nowtime);
	time_t DateTime = nowtime - (iRunTime / 1000);
	fGetTimeStringBytm(DateTime, szTimes);
}

void fGetSystemRunTime(char szTimes[50])
{
	int iRunTime = GetTickCount();
	int iH, iM, iS, iSec;
	iSec = iRunTime / 1000;
	iH = iSec / 3600;
	iM = iSec / 60 % 60;
	iS = iSec % 60;
	sprintf(szTimes,"%02d:%02d:%02d", iH, iM, iS);
}

time_t fGetSystemStartTime_t()
{
	int iRunTime = GetTickCount();
	time_t nowtime;
	time(&nowtime);
	time_t DateTime = nowtime - (iRunTime / 1000);
	return DateTime;
}