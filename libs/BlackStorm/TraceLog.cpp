
#include "TraceLog.h"
#ifdef MYPPLIB_KEYBORD_LOG_FILE
#include "keylog.h"
#endif
#include "ProcessOper.h"
#include <direct.h>
#include <plog/Log.h>
#include "IString.h"

TCHAR m_szTraceLogFileName[MAX_PATH] = TEXT("c:\\test.txt");

void fOutputDebugString(const TCHAR *psz)
{
#define OUT_PUT_DEBUG_STRING
#ifdef OUT_PUT_DEBUG_STRING
	OutputDebugString(psz) ;
#endif
}

void fTraceLogAppend(const TCHAR szFileName[] ,const TCHAR * pstr)
{
//	fTraceLogEncryptToFile(pstr) ;
//	fOutputDebugString(pstr) ;
	HANDLE hFile = CreateFile(szFileName ,  GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,
		NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		fOutputDebugString(TEXT("fTraceLogAppend: CreateFile failed\r\n")) ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	DWORD dwWriteRet =0 ;
	WriteFile(hFile ,pstr , _tcslen(pstr)*sizeof(TCHAR) ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
}

void fTraceLogEncryptToFile(TCHAR *pstr)
{
#ifdef MYPPLIB_KEYBORD_LOG_FILE
	if (gbOpenKeyLog==TRUE)
	{
		TCHAR szTmpBuf[1024] = {0} ;
		SYSTEMTIME st ;
		GetLocalTime(&st) ;
		sprintf(szTmpBuf ,"[%04d-%02d-%02d %02d:%02d:%02d]  " , st.wYear ,
			st.wMonth ,st.wDay ,st.wHour ,st.wMinute ,st.wSecond) ;
		strcat(szTmpBuf,pstr) ;
		//保存至键盘记录中
		KeyLogSaveToFile(szTmpBuf) ;
	}
#endif
}

void fTraceLogMFCApp(TCHAR *pstr)
{
#ifdef _MFC_APP_
	theDlgMain->SetDlgItemText(IDC_STATIC_INSTALL_INFO,pstr) ;
#endif
}

void fTraceLogAndTimeToFile(TCHAR *pszFileName ,TCHAR * pstr)
{
#ifdef TRACE_LOG_TO_FILE
	TCHAR szTmpBuf[2048] = {0} ;
	SYSTEMTIME st ;
	DWORD dwWriteRet =0 ;
	GetLocalTime(&st) ;
	sprintf(szTmpBuf ,"[%04d-%02d-%02d %02d:%02d:%02d]  " , st.wYear ,st.wMonth ,st.wDay ,st.wHour ,st.wMinute ,st.wSecond) ;
	HANDLE hFile = CreateFile(pszFileName ,  GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE){
		fOutputDebugString("fTraceLogAndTimeToFile: CreateFile failed\r\n") ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	WriteFile(hFile ,szTmpBuf , strlen(szTmpBuf) ,&dwWriteRet ,NULL) ;
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	WriteFile(hFile ,pstr , strlen(pstr) ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
#endif // TRACE_LOG_TO_FILE
}

void fTraceLogToFile(TCHAR *pszFileName ,TCHAR * pstr)
{
#ifdef TRACE_LOG_TO_FILE
	HANDLE hFile = CreateFile(pszFileName ,  GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		fOutputDebugString("fTraceLogToFile: CreateFile failed\r\n") ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	DWORD dwWriteRet =0 ;
	WriteFile(hFile ,pstr , strlen(pstr) ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
#endif // TRACE_LOG_TO_FILE
}

void fTraceLogMemoryToFile(TCHAR tcsFilePath[] ,PBYTE pByteBuf ,int iByteBufLen)
{
//#define TRACE_MEMORY_LOG_TO_FILE
#ifdef TRACE_MEMORY_LOG_TO_FILE
	TCHAR szTmpBuf[2048] = {0} ;
	TCHAR szHexByte[5] ={0} ;
	SYSTEMTIME st ;
	GetLocalTime(&st) ;
	sprintf(szTmpBuf ,"[%04d-%02d-%02d %02d:%02d:%02d]  " , st.wYear ,st.wMonth ,st.wDay ,st.wHour ,st.wMinute ,st.wSecond) ;
	HANDLE hFile = CreateFile(tcsFilePath,  GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE){
		fOutputDebugString("fTraceLogAppend: CreateFile failed\r\n") ;
		return ;
	}
	if (iByteBufLen>0){
		for (int i =0 ;i<iByteBufLen ;i++){
			sprintf(szHexByte,"%02X,",pByteBuf[i]) ;
			strcat(szTmpBuf,szHexByte) ;
		}
		strcat(szTmpBuf,"\r\n") ;
		SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
		DWORD dwWriteRet =0 ;
		WriteFile(hFile ,szTmpBuf , strlen(szTmpBuf) ,&dwWriteRet ,NULL) ;
	}
	CloseHandle(hFile) ;
#endif // TRACE_MEMORY_LOG_TO_FILE
}

void fTraceLogBYTEToFile(TCHAR *pszFileName ,PBYTE pByteBuf,DWORD dwByteBufLen)
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE){
		fOutputDebugString(TEXT("fTraceLogAppend: CreateFile failed\r\n")) ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	DWORD dwWriteRet =0 ;
	WriteFile(hFile ,pByteBuf , dwByteBufLen ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
}

void fTraceLogStringToFile(TCHAR *pszFileName ,TCHAR tcsBuf[])
{
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE){
		fOutputDebugString(TEXT("fTraceLogAppend: CreateFile failed\r\n")) ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	DWORD dwWriteRet =0 ;
	WriteFile(hFile ,tcsBuf , _tcslen(tcsBuf) ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
}

void fTraceLogStringToFile(TCHAR *pszFileName ,const TCHAR* format, ...)
{
	TCHAR* buffer = NULL;
	va_list args;
	va_start(args, format);
	int length = _vsctprintf(format, args) + 1;
	buffer =new TCHAR[length+0x1] ;
	memset(buffer,0,(length+0x1)*sizeof(TCHAR)) ;
	if (!buffer) 
		return;
	_vstprintf_s(buffer, length, format, args);
	va_end(args);
	HANDLE hFile = CreateFile(pszFileName, GENERIC_READ|GENERIC_WRITE ,FILE_SHARE_READ|FILE_SHARE_WRITE ,NULL , OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hFile == INVALID_HANDLE_VALUE){
		fOutputDebugString(TEXT("fTraceLogAppend: CreateFile failed\r\n")) ;
		return ;
	}
	SetFilePointer(hFile , NULL ,NULL ,FILE_END) ;
	DWORD dwWriteRet =0 ;
	WriteFile(hFile ,buffer , _tcslen(buffer) ,&dwWriteRet ,NULL) ;
	CloseHandle(hFile) ;
	delete []buffer ;
}

//格式化OutputDebugString
VOID fTraceLogOutputString(bool bDebugMessage, const TCHAR* format, ...)
{
	const TCHAR* debugPrefix = TEXT("DEBUG: ");
	size_t debugPrefixLength = _tcsclen(debugPrefix);
	TCHAR* buffer = NULL;
	va_list args;
	va_start(args, format);
	int length = _vsctprintf(format, args) + 1;
	if (bDebugMessage){
		length += debugPrefixLength;
	}
	buffer =new TCHAR[length+0x1] ;
	memset(buffer,0,(length+0x1)*sizeof(TCHAR)) ;
	if (!buffer) 
		return;
	if (bDebugMessage){
		_tcscpy_s(buffer, length, debugPrefix);
		_vstprintf_s(buffer + debugPrefixLength, length - debugPrefixLength, format, args);
	}else{
		_vstprintf_s(buffer, length, format, args);
	}
	va_end(args);
	OutputDebugString(buffer) ;
	delete []buffer ;
}

//格式化wprintf
VOID fTraceLogPrintf(bool bDebugMessage, const TCHAR* format, ...)
{
	const TCHAR* debugPrefix = TEXT("DEBUG: ");
	size_t debugPrefixLength = _tcsclen(debugPrefix);
	TCHAR* buffer = NULL;
	va_list args;
	va_start(args, format);
	int length = _vsctprintf(format, args) + 1;
	if (bDebugMessage){
		length += debugPrefixLength;
	}
	buffer = (TCHAR*)malloc(length * sizeof(TCHAR));
	if (!buffer) 
		return;
	if (bDebugMessage){
		_tcscpy_s(buffer, length, debugPrefix);
		_vstprintf_s(buffer + debugPrefixLength, length - debugPrefixLength, format, args);
	}else{
		_vstprintf_s(buffer, length, format, args);
	}
	va_end(args);
	_tprintf(buffer) ;
	delete []buffer ;
}

//初始化日志文件
void fplogInit(std::string strFileName)
{
	TCHAR tcsPath[MAX_PATH] = { 0 };
	fGetProcessFullPathByProcessId(GetCurrentProcessId(), tcsPath);
#ifdef _UNICODE
	std::string strPath = UtilString::Easy_UnicodeToAnsi(tcsPath);
#else
	std::string strPath = tcsPath;
#endif // _UNICODE
	char szDrive[MAX_PATH] = { 0 };
	char szDirPath[MAX_PATH] = { 0 };
	char szFileName[MAX_PATH] = { 0 };
	char szExt[MAX_PATH] = { 0 };
	_splitpath(strPath.c_str(), szDrive, szDirPath, szFileName, szExt);
	std::string strLogPath;
	strLogPath += szDrive;
	strLogPath += szDirPath;
	strLogPath += "Logs";
	_mkdir(strLogPath.c_str());
	strLogPath += strFileName;
	plog::init(plog::info, strLogPath.c_str()); // Step2: initialize the logger.
}

//打印INFO日志
void fplogLogInfo(std::stringstream strStream)
{
	
}