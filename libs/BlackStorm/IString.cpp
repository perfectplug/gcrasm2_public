
#include "IString.h"
#include <memory>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>
#include <string>
#include <tchar.h>

namespace UtilString
{
	namespace Internal
	{
		inline UINT MiGetConversionACP() throw()
		{
#ifdef _CONVERSION_DONT_USE_THREAD_LOCALE
			return CP_ACP;
#else
			return CP_THREAD_ACP;
#endif
		}

		LPWSTR MiC2W(LPCSTR lp)
		{
			int nWLen = 0;
			wchar_t *pWStr = NULL;

			nWLen = MultiByteToWideChar(MiGetConversionACP(), 0, lp, -1, NULL, 0);
			pWStr = (wchar_t *)malloc(nWLen * sizeof(wchar_t));
			nWLen = MultiByteToWideChar(MiGetConversionACP(), 0, lp, -1, pWStr, nWLen);

			return pWStr;
		}

		LPSTR MiW2C(LPCWSTR lp)
		{
			int nALen = 0;
			char *pAStr = NULL;

			nALen = WideCharToMultiByte(MiGetConversionACP(), 0, lp, -1, NULL, 0, NULL, NULL);
			pAStr = (char*)malloc(nALen * sizeof(char));
			WideCharToMultiByte(MiGetConversionACP(), 0, lp, -1, pAStr, nALen, NULL, NULL);

			return pAStr;
		}

		LPWSTR MiUE2W(LPCSTR lp)
		{
			int nWLen = 0;
			wchar_t *pWStr = NULL;

			nWLen = MultiByteToWideChar(CP_UTF8, 0, lp, -1, NULL, 0);
			pWStr = (wchar_t *)malloc(nWLen * sizeof(wchar_t));
			nWLen = MultiByteToWideChar(CP_UTF8, 0, lp, -1, pWStr, nWLen);

			return pWStr;
		}

		LPSTR MiW2UE(LPCWSTR lp)
		{
			int nALen = 0;
			char *pAStr = NULL;

			nALen = WideCharToMultiByte(CP_UTF8, 0, lp, -1, NULL, 0, NULL, NULL);
			pAStr = (char*)malloc(nALen * sizeof(char));
			WideCharToMultiByte(CP_UTF8, 0, lp, -1, pAStr, nALen, NULL, NULL);

			return pAStr;
		}

		LPSTR MiUE2C(LPCSTR lp)
		{
			LPWSTR lpW = MiUE2W(lp);
			if (lpW == NULL)
				return NULL;
			LPSTR lpRet = MiW2C(lpW);
			free(lpW);
			return lpRet;
		}

		LPSTR MiC2UE(LPCSTR lp)
		{
			LPWSTR lpW = MiC2W(lp);
			if (lpW == NULL)
				return NULL;
			LPSTR lpRet = MiW2UE(lpW);
			free(lpW);
			return lpRet;
		}

		LPWSTR MiW2W(LPCWSTR lp)
		{
			if (lp == NULL)
				return NULL;
			wchar_t *pWStr = (wchar_t *)malloc((wcslen(lp) + 1) * sizeof(wchar_t));
			wcscpy_s(pWStr, (wcslen(lp) + 1), lp);
			return pWStr;
		}

		LPSTR MiC2C(LPCSTR lp)
		{
			if (lp == NULL)
				return NULL;
			char *pAStr = (char *)malloc((strlen(lp) + 1) * sizeof(char));
			strcpy_s(pAStr, (strlen(lp) + 1), lp);
			return pAStr;
		}
	}

	// 普通函数
	int TestFunc(int a)
	{
		return a;
	}

	/************************************************************************/
	/*                       string去除空格                                 */
	/************************************************************************/
	static inline std::string &ltrim(std::string &s) {
		// 类成员函数
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
		return s;
	}

	// trim from end 
	static inline std::string &rtrim(std::string &s) {
		
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end());
		return s;
	}

	std::string& Trim(std::string &s)
	{
		return ltrim(rtrim(s));
	}

	/************************************************************************/
	/*                       wstring去除空格                                */
	/************************************************************************/
	// trim from start (in place)
	static inline void ltrim(std::wstring &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			[](int c) {return !std::isspace(c); }));
	}

	// trim from end (in place)
	static inline void rtrim(std::wstring &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
			[](int c) {return !std::isspace(c); }).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::wstring &s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::wstring ltrim_copy(std::wstring s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::wstring rtrim_copy(std::wstring s) {
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::wstring trim_copy(std::wstring s) {
		trim(s);
		return s;
	}

	std::wstring trim2(_In_ std::wstring& str)
	{
		std::wstring wsSpace(TEXT(" "));
		std::wstring wsValue = TEXT("");
		for (size_t i = 0; i < str.size(); i++)
		{
			if (wsSpace[0] != str[i])
			{
				wsValue += str[i];
			}
		}
		return wsValue;
	}

	std::wstring Trim(std::wstring &s)
	{
		return trim2(s);
	}

	bool StringSplit(const tstring &str, const tstring &delim, std::vector<tstring>& result)
	{
		tstring::size_type pos = 0;
		tstring::size_type length = str.length();
		tstring elt;

		TCHAR quoteChar = _T('\0');
		while (pos < length) {
			if (quoteChar == _T('\0') && (str[pos] == _T('"') || str[pos] == _T('\''))) {
				quoteChar = str[pos++];
				continue; // Skip the quote
			}
			else if (quoteChar == _T('\0') && str[pos] == L'\\' && pos + 1 < length &&
				(str[pos + 1] == _T('\'') || str[pos + 1] == _T('"'))) {
				++pos;
			}
			else if (quoteChar != _T('\0') && str[pos] == L'\\' && pos + 1 < length && str[pos + 1] == quoteChar) {
				++pos;
			}
			else if (quoteChar != _T('\0') && str[pos] == quoteChar) {
				++pos;
				quoteChar = _T('\0');
				continue; // Skip the end quote
			}
			else if (delim.find(str[pos]) != tstring::npos) {
				if (quoteChar == _T('\0')) {
					++pos;
					if (elt.length() > 0) {
						result.push_back(elt);
						elt = L"";
					}
					continue;
				}
			}

			if (pos < length) {
				elt += str[pos++];
			}
		}

		if (elt.length() > 0) {
			result.push_back(elt);
		}
		if (quoteChar != _T('\0')) {
			return false; // Unmatched quote.
		}
		return true;
	}

	std::string WstringToString(const std::wstring &s)
	{
		char* szA = Internal::MiW2C(s.c_str());
		std::string s_a = szA;
		free(szA);
		return s_a;
	}

	std::wstring StringToWstring(const std::string &s)
	{
		wchar_t* szW = Internal::MiC2W(s.c_str());
		std::wstring s_w = szW;
		free(szW);
		return s_w;
	}

	std::string TStringToString(const tstring &s)
	{
#if defined(UNICODE) || defined(_UNICODE)
		return WstringToString(s);
#else
		return s;
#endif
	}

	std::wstring TstringToWstring(const tstring &s)
	{
#if defined(UNICODE) || defined(_UNICODE)
		return s;
#else
		return StringToWstring(s);
#endif
	}

	tstring StringToTString(const std::string &s)
	{
#if defined(UNICODE) || defined(_UNICODE)
		return StringToWstring(s);
#else
		return s;
#endif
	}

	tstring WstringToTstring(const std::wstring &s)
	{
#if defined(UNICODE) || defined(_UNICODE)
		return s;
#else
		return WstringToString(s);
#endif
	}

	std::string Unicode2StdString(LPTSTR szString)
	{
		if (!szString) return "";
		int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
		int nChar = WideCharToMultiByte(codepage, 0, szString, -1, 0, 0, 0, 0);
		int nSize = nChar * sizeof(char) + 1;
		char* pchBuffer = new char[nSize];

		nChar = WideCharToMultiByte(codepage, 0, szString, -1, pchBuffer, nSize, 0, 0);
		if (nChar == 0)
		{
			delete[] pchBuffer;
			return "";
		}
		pchBuffer[nChar] = 0;
		std::string szRes(pchBuffer);
		delete[] pchBuffer;
		pchBuffer = NULL;
		return szRes;
	}

	std::string Unicode2StdString(LPCTSTR szString)
	{
		return Unicode2StdString((LPTSTR)szString);
	}

	bool UnicodeToAnsi(const std::wstring &src, std::string & result)
	{
		int ascii_size = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, NULL, 0, NULL, NULL);

		if (ascii_size == 0)
		{
			return false;
		}

		std::vector<char> result_buf(ascii_size, 0);
		int result_size = ::WideCharToMultiByte(CP_ACP, 0, src.c_str(), -1, &result_buf[0], ascii_size, NULL, NULL);

		if (result_size != ascii_size)
		{
			return false;
		}

		result = &result_buf[0];

		return true;
	}

	bool AnsiToUnicode(const std::string &src, std::wstring & result)
	{
		int wide_size = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);

		if (wide_size == 0)
		{
			return false;
		}

		std::vector<wchar_t> result_buf(wide_size, 0);
		int result_size = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, &result_buf[0], wide_size);

		if (result_size != wide_size)
		{
			return false;
		}

		result = &result_buf[0];

		return true;
	}

	bool UnicodeToUtf8(const std::wstring &src, std::string & result)
	{
		int utf8_size = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);

		if (utf8_size == 0)
		{
			return false;
		}

		std::vector<char> result_buf(utf8_size, 0);

		int result_size = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, &result_buf[0], utf8_size, NULL, NULL);

		if (result_size != utf8_size)
		{
			return false;
		}

		result = &result_buf[0];

		return true;
	}

	bool Utf8ToUnicode(const std::string &src, std::wstring & result)
	{
		int wide_size = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);

		if (wide_size == 0)
		{
			return false;
		}

		std::vector<wchar_t> result_buf(wide_size, 0);

		int result_size = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, &result_buf[0], wide_size);

		if (result_size != wide_size)
		{
			return false;
		}

		result = &result_buf[0];

		return true;
	}

	bool AnsiToUtf8(const std::string& src, std::string &result)
	{
		std::wstring wstr;
		if (!AnsiToUnicode(src, wstr))
			return false;

		return UnicodeToUtf8(wstr, result);
	}

	bool Utf8ToAnsi(const std::string& src, std::string &result)
	{
		std::wstring wstr;
		if (!Utf8ToUnicode(src, wstr))
			return false;

		return UnicodeToAnsi(wstr, result);
	}

	std::string Easy_UnicodeToAnsi(const std::wstring &src)
	{
		std::string strRet;
		if (!UnicodeToAnsi(src, strRet))
			return "";

		return strRet;
	}

	std::wstring Easy_AnsiToUnicode(const std::string &src)
	{
		std::wstring strRet;
		if (!AnsiToUnicode(src, strRet))
			return L"";

		return strRet;
	}

	std::string Easy_UnicodeToUtf8(const std::wstring &src)
	{
		std::string strRet;
		if (!UnicodeToUtf8(src, strRet))
			return "";

		return strRet;
	}

	std::wstring Easy_Utf8ToUnicode(const std::string &src)
	{
		std::wstring strRet;
		if (!Utf8ToUnicode(src, strRet))
			return L"";

		return strRet;
	}

	std::string Easy_AnsiToUtf8(const std::string &src)
	{
		std::string strRet;
		if (!AnsiToUtf8(src, strRet))
			return "";

		return strRet;
	}

	std::string Easy_Utf8ToAnsi(const std::string &src)
	{
		std::string strRet;
		if (!Utf8ToAnsi(src, strRet))
			return "";

		return strRet;
	}

	unsigned char ToHex(unsigned char x)
	{
		return  x > 9 ? x + 55 : x + 48;
	}

	unsigned char FromHex(unsigned char x)
	{
		unsigned char y;
		if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
		else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
		else if (x >= '0' && x <= '9') y = x - '0';
		return y;
	}

	std::string UrlEncode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}

	BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
	{
		if (szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
			return FALSE;

		size_t len_ascii = strlen(szSrc);
		if (len_ascii == 0)
		{
			pBuf[0] = 0;
			return TRUE;
		}

		BOOL bRet = FALSE;
		LPWSTR pUnicode = NULL;
		LPSTR pUTF8 = NULL;
		do
		{	   //先转换到UTF-8
			char baseChar = bUpperCase ? 'A' : 'a';
			int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
			pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
			if (pUnicode == NULL)break;

			MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);
			int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
			pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
			if (pUTF8 == NULL)
			{
				break;
			}
			WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
			pUTF8[cbUTF8] = '\0';

			unsigned char c;
			int cbDest = 0; //累加
			unsigned char *pSrc = (unsigned char*)pUTF8;
			unsigned char *pDest = (unsigned char*)pBuf;
			while (*pSrc && cbDest < cbBufLen - 1)
			{
				c = *pSrc;
				if (isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~' ||
					c == ':' ||
					c == '/' ||
					c == '_')
				{
					*pDest = c;
					++pDest;
					++cbDest;
				}
				else if (c == ' ')
				{
					*pDest = '+';
					++pDest;
					++cbDest;
				}
				else
				{
					//检查缓冲区大小是否够用？
					if (cbDest + 3 > cbBufLen - 1) break;
					pDest[0] = '%';
					pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
					pDest[2] = ((c & 0xF) >= 0xA) ? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
					pDest += 3;
					cbDest += 3;
				}
				++pSrc;
			}

			*pDest = '\0';
			bRet = TRUE;
		} while (FALSE);

		if (pUnicode)free(pUnicode);
		if (pUTF8)free(pUTF8);
		return bRet;
	}

	std::string UrlDecode(const std::string& str)
	{
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (str[i] == '+') strTemp += ' ';
			else if (str[i] == '%')
			{
				unsigned char high = FromHex((unsigned char)str[++i]);
				unsigned char low = FromHex((unsigned char)str[++i]);
				strTemp += high * 16 + low;
			}
			else strTemp += str[i];
		}
		return strTemp;
	}

	bool ReadFileToString(std::string file_name, std::string& fileData)
	{
		std::ifstream file(file_name.c_str(), std::ifstream::binary);
		if (file)
		{
			file.seekg(0, file.end);
			const int file_size = (int)file.tellg();
			char* file_buf = new char[file_size + 1];
			memset(file_buf, 0, file_size + 1);
			file.seekg(0, std::ios::beg);
			file.read(file_buf, file_size);
			if (file)
				fileData.append(file_buf);
			else
			{
				fileData.append(file_buf);
				return false;
			}
			file.close();
			delete[]file_buf;
		}
		else
			return false;
		return true;
	}

	bool WriteStringToFile(std::string file_name, std::string fileData, bool bAppend)
	{
		std::ios_base::openmode opMode = std::ios::binary;
		if (!bAppend)
			opMode |= std::ofstream::trunc;
		else
			opMode |= std::ofstream::app;
		std::ofstream file(file_name.c_str(), opMode);
		if (file)
		{
			file << fileData.c_str();
			file.close();
		}
		else
			return false;
		return true;
	}

	std::string FormatStdString(const std::string fmt_str, ...)
	{
		int final_n, n = ((int)fmt_str.size()) * 2;
		std::string str;
		std::unique_ptr<char[]> formatted;
		va_list ap;
		while (1) {
			formatted.reset(new char[n]);
			strcpy(&formatted[0], fmt_str.c_str());
			va_start(ap, fmt_str);
			final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
			va_end(ap);
			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return std::string(formatted.get());
	}

	std::wstring FormatStdString(const std::wstring fmt_str, ...)
	{
		int final_n, n = ((int)fmt_str.size()) * 2;
		std::string str;
		std::unique_ptr<wchar_t[]> formatted;
		va_list ap;
		while (1) {
			formatted.reset(new wchar_t[n]);
			wcscpy(&formatted[0], fmt_str.c_str());
			va_start(ap, fmt_str);
			final_n = vswprintf_s(&formatted[0], n, fmt_str.c_str(), ap);
			va_end(ap);
			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return std::wstring(formatted.get());
	}

	std::vector<std::string> split(std::string str, std::string pattern)
	{
		std::string::size_type pos;
		std::vector<std::string> result;
		str += pattern;
		size_t size = str.size();

		for (size_t i = 0; i < size; i++)
		{
			pos = str.find(pattern, i);
			if (pos < size)
			{
				std::string s = str.substr(i, pos - i);
				if (!s.empty())
					result.push_back(s);
				i = pos + pattern.size() - 1;
			}
		}
		return result;
	}

	void SplitStringToVec(std::string strSrc, std::vector<std::string>& vecDest, std::string cSeparator)
	{
		if (strSrc.empty())
		{
			return;
		}

		std::string::size_type iSearchPos = 0;
		std::string::size_type iCurrentPos = 0;

		while ((iSearchPos = strSrc.find_first_of(cSeparator, iSearchPos)) != std::string::npos)
		{
			std::string strSingleSection = strSrc.substr(iCurrentPos, iSearchPos - iCurrentPos);

			if (strSingleSection.length())
			{
				strSingleSection.erase(0, strSingleSection.find_first_not_of(" "));
				strSingleSection.erase(strSingleSection.find_last_not_of(" ") + 1);
			}
			if (strSingleSection.length())vecDest.push_back(strSingleSection);

			iCurrentPos = ++iSearchPos;
		}

		if (iCurrentPos < strSrc.length())
		{
			vecDest.push_back(strSrc.substr(iCurrentPos).c_str());
		}
	};

	bool strEndWith(const std::string& str, const std::string strEnd)
	{
		if (str.empty() || strEnd.empty() || strEnd.length() > str.length())
		{
			return false;
		}
		return str.compare(str.size() - strEnd.size(), strEnd.size(), strEnd) == 0 ? true : false;
	}
	bool strEndWithW(const std::wstring& str, const std::wstring strEnd)
	{
		if (str.empty() || strEnd.empty() || strEnd.length() > str.length())
		{
			return false;
		}
		return str.compare(str.size() - strEnd.size(), strEnd.size(), strEnd) == 0 ? true : false;
	}
}