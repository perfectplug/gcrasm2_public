
#ifndef _TRACE_LOG_H_PERFECTPLUG_
#define _TRACE_LOG_H_PERFECTPLUG_
#include "PPBase.h"

extern TCHAR m_szTraceLogFileName[MAX_PATH];

void fOutputDebugString(const TCHAR *psz);
void fTraceLogAppend(const TCHAR szFileName[], const TCHAR * pstr);
void fTraceLogEncryptToFile(TCHAR *pstr);
void fTraceLogAndTimeToFile(TCHAR *pszFileName, TCHAR * pstr);
void fTraceLogToFile(TCHAR *pszFileName, TCHAR * pstr);
void fTraceLogMFCApp(TCHAR *pstr);
void fTraceLogMemoryToFile(TCHAR tcsFilePath[], PBYTE pByteBuf, int iByteBufLen);
void fTraceLogBYTEToFile(TCHAR *pszFileName, PBYTE pByteBuf, DWORD dwByteBufLen);
void fTraceLogStringToFile(TCHAR *pszFileName, TCHAR tcsBuf[]);
void fTraceLogStringToFile(TCHAR *pszFileName, const TCHAR* format, ...);
VOID fTraceLogOutputString(bool bDebugMessage, const TCHAR* format, ...);
VOID fTraceLogPrintf(bool bDebugMessage, const TCHAR* format, ...);
//初始化日志文件
void fplogInit(std::string strFileName);
#endif