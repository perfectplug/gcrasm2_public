#ifndef _MEMORY_OPER_H_PERFECTPLUG_
#define _MEMORY_OPER_H_PERFECTPLUG_
#include "TraceLog.h"
#include "PPBase.h"
typedef   BOOL (__stdcall *ProcDllMain)(HINSTANCE, DWORD, LPVOID);

#define ME_WIN32_DLL		0X1
#define ME_WIN32_EXE		0X2

typedef IMAGE_SECTION_HEADER (*PIMAGE_SECTION_HEADERS)[1];   
#pragma pack(push, 1)   
typedef struct _RelocBlock 
{
	int vaddress ;
	int size ;
}RelocBlock ,*PRelocBlock;
typedef struct _ImportBlock
{
	unsigned int Characteristics ;
	unsigned int TimeDateStamp ;
	unsigned int ForwarderChain ;
	char * Name ;
	LPVOID FirstThunk ;
}ImportBlock ,*PImportBlock;
#pragma pack(pop)   


VOID __stdcall fByteArraySetValue(BYTE byteArray_out1[], int iSetIndex, DWORD dwValue, int iValueSize);
bool __stdcall fByteArrayCompare(BYTE byteArray_out1[], BYTE byteArray_out2[], int isize);
bool __stdcall fCompareByteArrayIgnore11(BYTE byteArray_out1[], BYTE byteArray_out2[], int isize);
bool __stdcall fCompareByteArrayIgnore11WithParam1(_In_ BYTE byteMakeSure[], _In_ BYTE byteArray_out2[], _In_ int isize);
unsigned long fGetAlignedSize(unsigned long Origin, unsigned long Alignment);
unsigned long fCalcTotalImageSize(PIMAGE_DOS_HEADER MzH, unsigned long FileLen, PIMAGE_NT_HEADERS peH, PIMAGE_SECTION_HEADERS peSecH);
BOOL fAlignPEToMem(void *pBuf_in, long lBufLen_in, PIMAGE_NT_HEADERS &peH_out, PIMAGE_SECTION_HEADERS &peSecH_out, void *&pMem_out, unsigned long &ulImageSize_out);
BOOL fHasRelocationTable(PIMAGE_NT_HEADERS peH);
void fDoReloction(LPVOID baseorgp, LPVOID basedllp, LPVOID relocp);
void fRebuildIAT(LPVOID lpImageBaseAddr, LPVOID lpImageImportAddr);
int fPECallEntryPoint(PIMAGE_NT_HEADERS pNTH, PIMAGE_SECTION_HEADERS peSecH, void *pImageBuf, unsigned long ulImageSize, DWORD dwType);
int fMemExecute(void *pFileBuf, long lFileSize, DWORD dwType, DWORD dwImageBuf_in);
//通过flProtect获取名字
cstring fGetflProtectName(DWORD flProtect);
//通过flAllocationType获取名字
cstring fGetflAllocationTypeName(DWORD  flAllocationType);
//通过flProtect获取名字
std::string fGetflProtectNameA(DWORD flProtect);
//通过flAllocationType获取名字
std::string fGetflAllocationTypeNameA(DWORD  flAllocationType);


namespace BS_Memory {


	template<typename T>
	BOOL ReadEx(_In_ HANDLE hProcess, _In_ PVOID pInBuff, _Out_ T& buffer)
	{
		DWORD dwReadAddr = 0;
		DWORD dwBytesRead = 0;
		if (ReadProcessMemory(hProcess, (LPCVOID)pInBuff, &buffer, sizeof(T), &dwBytesRead))
		{
			return TRUE;
		}
		return FALSE;
	}



};
#endif