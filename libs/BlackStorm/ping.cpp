
#include "ping.h"
#include "ParseUrl.h"
#include<winsock2.h>
#include <string.h>
#include <stdio.h>

BOOL  GetIpByDomainName(char *szHost, char szIp[50][100], int *nCount)
{
	WSADATA        wsaData;
	HOSTENT   *pHostEnt;
	int             nAdapter = 0;
	struct       sockaddr_in   sAddr;
	if (WSAStartup(0x0101, &wsaData))
	{
		printf(" gethostbyname error for host:\n");
		return FALSE;
	}

	pHostEnt = gethostbyname(szHost);
	if (pHostEnt)
	{
		while (pHostEnt->h_addr_list[nAdapter])
		{
			memcpy(&sAddr.sin_addr.s_addr, pHostEnt->h_addr_list[nAdapter], pHostEnt->h_length);

			sprintf_s(szIp[nAdapter], "%s", inet_ntoa(sAddr.sin_addr));

			nAdapter++;
		}

		*nCount = nAdapter;
	}
	else
	{
		DWORD  dwError = GetLastError();
		*nCount = 0;
	}
	WSACleanup();
	return TRUE;

}

bool pingIp(char Ip[100])
{
	ParseUrl objParseUrl;
	PingReply reply;
	printf("\nPinging %s with %d bytes of data:\n\n", Ip, DEF_PACKET_SIZE);
	for (int i = 0; i < 4; i++)
	{
		if (objParseUrl.Ping(Ip, &reply))
		{
			printf("success -> Reply from %s: bytes=%ld time=%ldms TTL=%ld\n", Ip, reply.m_dwBytes, reply.m_dwRoundTripTime, reply.m_dwTTL);
			return true;
		}
		printf("failed -> Reply from %s: bytes=%ld time=%ldms TTL=%ld\n", Ip, reply.m_dwBytes, reply.m_dwRoundTripTime, reply.m_dwTTL);
	}
	return false;
}

bool pingurl(char url[100])
{
	int         nIpCount = 0;
	char        szIpList[50][100];
	char        szDomain[256] = { 0 };
	char        szIp[1024] = { 0 };
	strcpy_s(szDomain, url);
	GetIpByDomainName(szDomain, szIpList, &nIpCount);

	printf("域名 : %s \n", szDomain);

	for (int i = 0; i < nIpCount; i++)
	{
		if (pingIp(szIpList[i]))
		{
			return true;
		}
		strcat_s(szIp, szIpList[i]);
		strcat_s(szIp, "\t");
	}

	printf("\n域名解析IP列表 : %s \n\n", szIp);
	return false;
}