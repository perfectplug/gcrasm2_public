#pragma once
#include "PPBase.h"

#if !defined(tstring)
# if defined(UNICODE) || defined(_UNICODE)
#    define tstring std::wstring
#else
#   define tstring std::string
# endif
#endif

#if !defined(tostringstream)
# if defined(UNICODE) || defined(_UNICODE)
#   define tostringstream std::wostringstream
# else
#   define tostringstream std::ostringstream
# endif
#endif

#if !defined(tstringstream)
# if defined(UNICODE) || defined(_UNICODE)
#   define tstringstream std::wstringstream
# else
#   define tstringstream std::stringstream
# endif
#endif

namespace UtilString
{
	std::string& Trim(std::string &s);
	std::wstring Trim(std::wstring &s);
	//É¾³ýwstring×Ö·û´®µÄ¿Õ¸ñ·û
	std::wstring trim2(_In_ std::wstring& str);
	std::vector<std::string> split(std::string str, std::string pattern);
	void SplitStringToVec(std::string strSrc, std::vector<std::string>& vecDest, std::string cSeparator);
	bool StringSplit(const tstring &str, const tstring &delim, std::vector<tstring>& result);
	std::string FormatStdString(const std::string fmt_str, ...);
	std::wstring FormatStdString(const std::wstring fmt_str, ...);

	std::string  WstringToString(const std::wstring &s);
	std::wstring StringToWstring(const std::string &s);

	std::string  TStringToString(const tstring &s);
	std::wstring TstringToWstring(const tstring &s);

	tstring StringToTString(const std::string &s);
	tstring WstringToTstring(const std::wstring &s);

	std::string Unicode2StdString(LPTSTR szString);
	std::string Unicode2StdString(LPCTSTR szString);

	bool UnicodeToAnsi(const std::wstring &src, std::string &result);
	bool AnsiToUnicode(const std::string &src, std::wstring &result);
	bool UnicodeToUtf8(const std::wstring &src, std::string &result);
	bool Utf8ToUnicode(const std::string &src, std::wstring &result);
	bool AnsiToUtf8(const std::string &src, std::string &result);
	bool Utf8ToAnsi(const std::string &src, std::string &result);

	std::string UrlEncode(const std::string& str);
	BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);
	std::string UrlDecode(const std::string& str);

	std::string Easy_UnicodeToAnsi(const std::wstring &src);
	std::wstring Easy_AnsiToUnicode(const std::string &src);
	std::string Easy_UnicodeToUtf8(const std::wstring &src);
	std::wstring Easy_Utf8ToUnicode(const std::string &src);
	std::string Easy_AnsiToUtf8(const std::string &src);
	std::string Easy_Utf8ToAnsi(const std::string &src);

	bool ReadFileToString(std::string file_name, std::string& fileData);
	bool WriteStringToFile(std::string file_name, std::string fileData, bool bAppend);

	bool strEndWith(const std::string& str, const std::string strEnd);
	bool strEndWithW(const std::wstring& str, const std::wstring strEnd);
};