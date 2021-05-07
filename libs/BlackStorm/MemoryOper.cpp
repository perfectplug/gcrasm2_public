
#include "MemoryOper.h"

VOID __stdcall fByteArraySetValue(BYTE byteArray_out1[] ,int iSetIndex ,DWORD dwValue ,int iValueSize)
{
	switch(iValueSize)
	{
	case 1:
		*(PBYTE)((DWORD)byteArray_out1+iSetIndex) = (BYTE)dwValue ;
		break;
	case 2:
		*(PWORD)((DWORD)byteArray_out1+iSetIndex) = (WORD)dwValue ;
		break;
	case 4:
		*(PDWORD)((DWORD)byteArray_out1+iSetIndex) = (DWORD)dwValue ;
		break;
	}
}

bool __stdcall fByteArrayCompare(BYTE byteArray_out1[] ,BYTE byteArray_out2[] ,int isize)
{
	for (int i = 0 ;i <isize ;i++)
	{
		if (byteArray_out1[i] != byteArray_out2[i])
		{
			return false ;
		}
	}
	return true ;
}

bool __stdcall fCompareByteArrayIgnore11(BYTE byteArray_out1[] ,BYTE byteArray_out2[] ,int isize)
{
	int i0x11Count_1 = 0, i0x11Count_2 = 0;
	for (int i = 0 ;i <isize ;i++)
	{
		if (byteArray_out1[i] == 0X11)
		{
			i0x11Count_1++;
			continue ;
		}
		if (byteArray_out2[i] == 0X11)
		{
			i0x11Count_2++;
			continue;
		}
		if (byteArray_out1[i] != byteArray_out2[i])
		{
			return false ;
		}
	}
	if ((i0x11Count_1 == isize) || (i0x11Count_2 == isize))
	{
		return false;
	}
	return true ;
}

bool __stdcall fCompareByteArrayIgnore11WithParam1(_In_ BYTE byteMakeSure[],_In_ BYTE byteArray_out2[],_In_ int isize)
{
	for (int i = 0; i < isize; i++)
	{
		if (byteArray_out2[i] == 0X11)
		{
			continue;
		}
		if (byteMakeSure[i] != byteArray_out2[i])
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//函数名：fGetAlignedSize
//功能说明：计算对齐后的大小   
//参数：
//返回值：
// 
unsigned long fGetAlignedSize(unsigned long Origin, unsigned long Alignment)   
{   
	return (Origin + Alignment - 1) / Alignment * Alignment;   
}   

//////////////////////////////////////////////////////////////////////////
//函数名：fCalcTotalImageSize
//功能说明：计算加载pe并对齐需要占用多少内存，未直接使用OptionalHeader.SizeOfImage作为结果是因为据说有的编译器生成的exe这个值会填0   
//参数：
//返回值：
// unsigned long
unsigned long fCalcTotalImageSize(PIMAGE_DOS_HEADER MzH , unsigned long FileLen , PIMAGE_NT_HEADERS peH , PIMAGE_SECTION_HEADERS peSecH)   
{   
	unsigned long res;   
	// 计算pe头的大小   
	res = fGetAlignedSize( peH->OptionalHeader.SizeOfHeaders , peH->OptionalHeader.SectionAlignment );   

	// 计算所有节的大小   
	for( int i = 0; i < peH->FileHeader.NumberOfSections; ++i)   
	{   
		// 超出文件范围   
		if(peSecH[i]->PointerToRawData + peSecH[i]->SizeOfRawData > FileLen)   
			return 0;   
		else if(peSecH[i]->VirtualAddress)//计算对齐后某节的大小   
		{   
			if(peSecH[i]->Misc.VirtualSize)   
			{   
				res = fGetAlignedSize( peSecH[i]->VirtualAddress + peSecH[i]->Misc.VirtualSize   
					, peH->OptionalHeader.SectionAlignment   
					);   
			}   
			else  
			{   
				res = fGetAlignedSize( peSecH[i]->VirtualAddress + peSecH[i]->SizeOfRawData   
					, peH->OptionalHeader.SectionAlignment   
					);   
			}   
		}   
		else if( peSecH[i]->Misc.VirtualSize < peSecH[i]->SizeOfRawData )   
		{   
			res += fGetAlignedSize( peSecH[i]->SizeOfRawData   
				, peH->OptionalHeader.SectionAlignment   
				);   
		}   
		else  
		{   
			res += fGetAlignedSize( peSecH[i]->Misc.VirtualSize   
				, peH->OptionalHeader.SectionAlignment   
				);   
		}// if_else   
	}// for   

	return res;   
}   

//////////////////////////////////////////////////////////////////////////
//函数名：fAlignPEToMem
//功能说明：加载pe到内存并对齐所有节   
//参数：
//	pBuf_in	(void*) ,
//	lBufLen_in	(long)	,
//	peH_out	(PIMAGE_NT_HEADERS &) ,
//	peSecH_out (PIMAGE_SECTION_HEADERS &) ,
//	pMem_out	(void *&) ,
//	ulImageSize_out	(unsigned long &) ,
//返回值：
//	BOOL
//	TRUE,
//	FALSE,
BOOL fAlignPEToMem( void *pBuf_in , long lBufLen_in , PIMAGE_NT_HEADERS &peH_out , PIMAGE_SECTION_HEADERS &peSecH_out , void *&pMem_out , unsigned long &ulImageSize_out)   
{   
	int  i = 0;
	PIMAGE_DOS_HEADER SrcMz;// DOS头   
	PIMAGE_NT_HEADERS SrcPeH;// PE头   
	PIMAGE_SECTION_HEADERS SrcPeSecH;// 节表   
	SrcMz = (PIMAGE_DOS_HEADER)pBuf_in;   
	if( lBufLen_in < sizeof(IMAGE_DOS_HEADER) )    
		return FALSE;   
	if( SrcMz->e_magic != IMAGE_DOS_SIGNATURE )   
		return FALSE;   
	if( lBufLen_in < SrcMz->e_lfanew + (long)sizeof(IMAGE_NT_HEADERS) )   
		return FALSE;   
	SrcPeH = (PIMAGE_NT_HEADERS)((int)SrcMz + SrcMz->e_lfanew);   
	if( SrcPeH->Signature != IMAGE_NT_SIGNATURE )   
		return FALSE;   
	if(((SrcPeH->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) ||  (SrcPeH->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)) )   
	{   
		return FALSE;   
	}   
	SrcPeSecH = (PIMAGE_SECTION_HEADERS)((int)SrcPeH + sizeof(IMAGE_NT_HEADERS));   
	ulImageSize_out = fCalcTotalImageSize( SrcMz, lBufLen_in, SrcPeH, SrcPeSecH);   
	if( ulImageSize_out == 0 )   
		return FALSE;   
	// 分配内存  
	pMem_out = VirtualAlloc( pMem_out , ulImageSize_out, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);  
	DWORD dwLastError = GetLastError() ;
	if( pMem_out != NULL )   
	{   
		// 计算需要复制的PE头字节数   
		unsigned long l = SrcPeH->OptionalHeader.SizeOfHeaders;   
		for( i = 0; i < SrcPeH->FileHeader.NumberOfSections; ++i)   
		{   
			if( (SrcPeSecH[i]->PointerToRawData) &&   
				(SrcPeSecH[i]->PointerToRawData < l) )   
			{   
				l = SrcPeSecH[i]->PointerToRawData;   
			}   
		}   
		memmove( pMem_out, SrcMz, l);   
		peH_out = (PIMAGE_NT_HEADERS)((int)pMem_out + ((PIMAGE_DOS_HEADER)pMem_out)->e_lfanew);   
		peSecH_out = (PIMAGE_SECTION_HEADERS)((int)peH_out + sizeof(IMAGE_NT_HEADERS));   

		void *Pt = (void *)((unsigned long)pMem_out + fGetAlignedSize( peH_out->OptionalHeader.SizeOfHeaders, peH_out->OptionalHeader.SectionAlignment));   
		for( i = 0; i < peH_out->FileHeader.NumberOfSections; ++i)   
		{   
			// 定位该节在内存中的位置   
			if(peSecH_out[i]->VirtualAddress)   
				Pt = (void *)((unsigned long)pMem_out + peSecH_out[i]->VirtualAddress);   

			if(peSecH_out[i]->SizeOfRawData)   
			{   
				// 复制数据到内存   
				memmove(Pt, (const void *)((unsigned long)(SrcMz) + peSecH_out[i]->PointerToRawData), peSecH_out[i]->SizeOfRawData);   
				if(peSecH_out[i]->Misc.VirtualSize < peSecH_out[i]->SizeOfRawData)   
					Pt = (void *)((unsigned long)Pt + fGetAlignedSize(peSecH_out[i]->SizeOfRawData, peH_out->OptionalHeader.SectionAlignment));   
				else // pt 定位到下一节开始位置   
					Pt = (void *)((unsigned long)Pt + fGetAlignedSize(peSecH_out[i]->Misc.VirtualSize, peH_out->OptionalHeader.SectionAlignment));   
			}   
			else  
			{   
				Pt = (void *)((unsigned long)Pt + fGetAlignedSize(peSecH_out[i]->Misc.VirtualSize, peH_out->OptionalHeader.SectionAlignment));   
			}   
		}   
	}   
	else
	{
		return FALSE ;
	}
	return TRUE;   
}   

//////////////////////////////////////////////////////////////////////////
//函数名：fHasRelocationTable
//功能说明：
//参数：
//返回值：
// 是否包含可重定向列表   
BOOL fHasRelocationTable(PIMAGE_NT_HEADERS peH)   
{   
	return (peH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) && (peH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);   
} 

//////////////////////////////////////////////////////////////////////////
//函数名：fDoReloction
//功能说明：重定向PE用到的地址  
//参数：
//返回值：
void fDoReloction(LPVOID baseorgp ,LPVOID basedllp ,LPVOID relocp)
{
	//定义
	PRelocBlock myreloc ;
	unsigned int reloccount ;
	WORD * startp ;
	unsigned int i ;
	unsigned int * p ;
	unsigned int dif ;
	int imytmp1 =0x3000 ;
	int imytmp2 =0x1000 ;
	myreloc =(PRelocBlock)relocp ;
	dif = (unsigned int)basedllp - (unsigned int)baseorgp ;
	startp =(WORD *)((unsigned int)relocp + 8) ;
	while (myreloc->vaddress != 0)
	{
		reloccount =(myreloc->size - 8) /sizeof(WORD) ;
		for ( i =0 ;i< reloccount ;i++)
		{

			if (((*startp) ^ imytmp1) < imytmp2)
			{
				p =(unsigned int *)(myreloc->vaddress + *startp%imytmp1 + (int)basedllp) ;
				*p =*p + dif ;
			}
			startp = (WORD *)((unsigned int )startp + sizeof(WORD)) ;
		}
		myreloc =(PRelocBlock)startp ;
		startp = (WORD *)((unsigned int )startp + 8) ;
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名：fRebuildIAT
//功能说明：重建输入表
//参数：
//
//返回值：
void fRebuildIAT(LPVOID lpImageBaseAddr ,LPVOID lpImageImportAddr )
{
	char szTmpBuf[MAX_PATH] ={0} ;
	PImportBlock myimport ;
	LPVOID thunksread, thunkswrite ;
	char * dllname ;
	HMODULE dllh ;
	unsigned long old ;
	unsigned int * ptmp ;
	myimport =(PImportBlock) lpImageImportAddr ;
	while(myimport->FirstThunk != NULL && myimport->Name !=NULL){
		dllname =(char *)((int )lpImageBaseAddr + (int )myimport->Name) ;
		dllh = LoadLibraryA(dllname) ;
		if (dllh == NULL){
			sprintf_s(szTmpBuf ,"fRebuildIAT: LoadLibrary %s failed\r\n" ,dllname) ;
			OutputDebugStringA(szTmpBuf) ;
		}
		thunksread = (LPVOID)((int )myimport->FirstThunk + (int)lpImageBaseAddr) ;
		thunkswrite = thunksread ;
		if ((int )myimport->TimeDateStamp == -1){
			thunksread =(LPVOID)((int)myimport->Characteristics + (int)lpImageBaseAddr) ;
		}
		while( *(unsigned int *)thunksread != 0){
			if (VirtualProtect(thunkswrite,4,PAGE_EXECUTE_READWRITE,&old)){
				if (( (*(unsigned int * )(thunksread)) & 0x80000000 )!= 0){
					*(unsigned int *)thunkswrite =(int )GetProcAddress(dllh ,(char *)( (*(unsigned int *)thunksread) & 0xffff));
				}else{
					*(unsigned int *)thunkswrite =(int )GetProcAddress(dllh ,(char *)((int)lpImageBaseAddr + (int )(*(unsigned long *)thunksread) +2) ) ;
				}
				VirtualProtect(thunkswrite,4,old,&old);
			}
			ptmp =(unsigned int *)thunksread ;
			ptmp ++ ;
			thunksread =ptmp ;
			ptmp =(unsigned int *)thunkswrite ;
			ptmp ++ ;
			thunkswrite =ptmp ;
		}
		myimport =(PImportBlock)((int)(myimport)+sizeof(ImportBlock));
	}
}

//////////////////////////////////////////////////////////////////////////
//函数名：fPECallEntryPoint
//功能说明：重定位，修复IAT，执行PE入口点
//参数：
//	pNTH	(PIMAGE_NT_HEADERS) ,
//	peSecH	(PIMAGE_SECTION_HEADERS) ,
//	pImageBuf	(void *) ,
//	ulImageSize		(unsigned long) ,
//	dwType	(DWORD) ,
//返回值：
int fPECallEntryPoint(PIMAGE_NT_HEADERS pNTH, PIMAGE_SECTION_HEADERS peSecH,void *pImageBuf, unsigned long ulImageSize,DWORD dwType)
{
	int iRetValue =0 ;
#ifndef _WIN64
	if (pImageBuf==NULL)
	{
		iRetValue = -1 ;
	}
	else
	{
		if(pImageBuf) 
		{
			PDWORD pEntryAddr = (PDWORD)((DWORD)pImageBuf + pNTH->OptionalHeader.AddressOfEntryPoint) ;
			if(fHasRelocationTable(pNTH))
			{
				fDoReloction((LPVOID)(pNTH->OptionalHeader.ImageBase) ,(LPVOID)pImageBuf,(LPVOID)((DWORD)pImageBuf + pNTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) ) ;
				fRebuildIAT(pImageBuf , (LPVOID)((DWORD)pImageBuf + pNTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) ;
			}
			if (dwType==ME_WIN32_DLL)
			{
				ProcDllMain DllMain ;
				DllMain = (ProcDllMain)(pNTH->OptionalHeader.AddressOfEntryPoint + (DWORD)pImageBuf) ;
				if (pNTH ->OptionalHeader .AddressOfEntryPoint != 0)
				{
					DllMain((HINSTANCE)NULL ,DLL_PROCESS_ATTACH ,NULL ) ;
				}
				iRetValue = 1 ;
			}
			else if (dwType == ME_WIN32_EXE)
			{
				__asm
				{
					PUSHAD
					CALL pEntryAddr
					POPAD
				}
				iRetValue = 1 ;
			}
			else
			{
				iRetValue = -3 ;
			}
		}   
		else
		{
			iRetValue = -2 ;
		}
	}
#endif // _WIN64
	return iRetValue ;
}

//////////////////////////////////////////////////////////////////////////
//函数名：fMemExecute
//功能说明：内存加载WIN32_DLL 和 WIN32_EXE
//参数：
//	pFileBuf	(void*) ,读取文件缓冲区首地址
//	lFileSize	(long) ,读取文件大小
//	dwType	(DWORD) ,加载PE类型，ME_WIN32_DLL，ME_WIN32_EXE
//返回值：
//	int
//	1：调用成功
//	-2：加载PE入口点失败
//	-1：PE文件内存对其失败
int fMemExecute(void *pFileBuf, long lFileSize ,DWORD dwType,DWORD dwImageBuf_in)   
{   
	int iRetValue =0 ;
	PIMAGE_NT_HEADERS pPENTHeaders;   
	PIMAGE_SECTION_HEADERS pPESectionHeaders;   
	void *pImageBuf = (PVOID)dwImageBuf_in;   
	unsigned long iPEImageSize;   
	if(fAlignPEToMem(pFileBuf, lFileSize, pPENTHeaders, pPESectionHeaders, pImageBuf, iPEImageSize))   
	{   
		if (fPECallEntryPoint(pPENTHeaders,pPESectionHeaders,pImageBuf,iPEImageSize,dwType)==1)
		{
			iRetValue = 1;
		}
		else
		{
			iRetValue = -2 ;
		}
	}   
	else
	{
		iRetValue = -1 ;
	}
	return iRetValue ;
}   

//通过flProtect获取名字
cstring fGetflProtectName(DWORD flProtect)
{
	cstring tstrName;
	switch (flProtect)
	{
	case PAGE_NOACCESS:tstrName = TEXT("PAGE_NOACCESS");break;
	case PAGE_READONLY:tstrName = TEXT("PAGE_READONLY"); break;
	case PAGE_READWRITE:tstrName = TEXT("PAGE_READWRITE"); break;
	case PAGE_WRITECOPY:tstrName = TEXT("PAGE_WRITECOPY"); break;
	case PAGE_EXECUTE:tstrName = TEXT("PAGE_EXECUTE"); break;
	case PAGE_EXECUTE_READ:tstrName = TEXT("PAGE_EXECUTE_READ"); break;
	case PAGE_EXECUTE_READWRITE:tstrName = TEXT("PAGE_EXECUTE_READWRITE"); break;
	case PAGE_EXECUTE_WRITECOPY:tstrName = TEXT("PAGE_EXECUTE_WRITECOPY"); break;
	case PAGE_GUARD:tstrName = TEXT("PAGE_GUARD"); break;
	case PAGE_NOCACHE:tstrName = TEXT("PAGE_NOCACHE"); break;
	case PAGE_WRITECOMBINE:tstrName = TEXT("PAGE_WRITECOMBINE"); break;
	case PAGE_ENCLAVE_THREAD_CONTROL:tstrName = TEXT("PAGE_ENCLAVE_THREAD_CONTROL"); break;
	//case PAGE_REVERT_TO_FILE_MAP:tstrName = TEXT("PAGE_REVERT_TO_FILE_MAP"); break;
	case PAGE_TARGETS_NO_UPDATE:tstrName = TEXT("PAGE_TARGETS_NO_UPDATE"); break;
	//case PAGE_TARGETS_INVALID:tstrName = TEXT("PAGE_TARGETS_INVALID"); break;
	case PAGE_ENCLAVE_DECOMMIT:tstrName = TEXT("PAGE_ENCLAVE_DECOMMIT"); break;
	default:tstrName = TEXT("Unknown"); break;
	}
	return tstrName;
}

//通过flProtect获取名字
std::string fGetflProtectNameA(DWORD flProtect)
{
	std::string tstrName;
	switch (flProtect)
	{
	case PAGE_NOACCESS:tstrName = ("PAGE_NOACCESS"); break;
	case PAGE_READONLY:tstrName = ("PAGE_READONLY"); break;
	case PAGE_READWRITE:tstrName = ("PAGE_READWRITE"); break;
	case PAGE_WRITECOPY:tstrName = ("PAGE_WRITECOPY"); break;
	case PAGE_EXECUTE:tstrName = ("PAGE_EXECUTE"); break;
	case PAGE_EXECUTE_READ:tstrName = ("PAGE_EXECUTE_READ"); break;
	case PAGE_EXECUTE_READWRITE:tstrName = ("PAGE_EXECUTE_READWRITE"); break;
	case PAGE_EXECUTE_WRITECOPY:tstrName = ("PAGE_EXECUTE_WRITECOPY"); break;
	case PAGE_GUARD:tstrName = ("PAGE_GUARD"); break;
	case PAGE_NOCACHE:tstrName = ("PAGE_NOCACHE"); break;
	case PAGE_WRITECOMBINE:tstrName = ("PAGE_WRITECOMBINE"); break;
	case PAGE_ENCLAVE_THREAD_CONTROL:tstrName = ("PAGE_ENCLAVE_THREAD_CONTROL"); break;
		//case PAGE_REVERT_TO_FILE_MAP:tstrName = TEXT("PAGE_REVERT_TO_FILE_MAP"); break;
	case PAGE_TARGETS_NO_UPDATE:tstrName = ("PAGE_TARGETS_NO_UPDATE"); break;
		//case PAGE_TARGETS_INVALID:tstrName = TEXT("PAGE_TARGETS_INVALID"); break;
	case PAGE_ENCLAVE_DECOMMIT:tstrName = ("PAGE_ENCLAVE_DECOMMIT"); break;
	default:tstrName = ("Unknown"); break;
	}
	return tstrName;
}

//通过flAllocationType获取名字
cstring fGetflAllocationTypeName(DWORD  flAllocationType)
{
	cstring tstrName;
	switch (flAllocationType)
	{
	case MEM_COMMIT:tstrName = TEXT("MEM_COMMIT"); break;
	case MEM_RESERVE:tstrName = TEXT("MEM_RESERVE"); break;
	case MEM_REPLACE_PLACEHOLDER:tstrName = TEXT("MEM_REPLACE_PLACEHOLDER"); break;
	case MEM_RESERVE_PLACEHOLDER:tstrName = TEXT("MEM_RESERVE_PLACEHOLDER"); break;
	case MEM_RESET:tstrName = TEXT("MEM_RESET"); break;
	case MEM_TOP_DOWN:tstrName = TEXT("MEM_TOP_DOWN"); break;
	case MEM_WRITE_WATCH:tstrName = TEXT("MEM_WRITE_WATCH"); break;
	case MEM_PHYSICAL:tstrName = TEXT("MEM_PHYSICAL"); break;
	case MEM_ROTATE:tstrName = TEXT("MEM_ROTATE"); break;
	//case MEM_DIFFERENT_IMAGE_BASE_OK:tstrName = TEXT("MEM_DIFFERENT_IMAGE_BASE_OK"); break;
	case MEM_RESET_UNDO:tstrName = TEXT("MEM_RESET_UNDO"); break;
	case MEM_LARGE_PAGES:tstrName = TEXT("MEM_LARGE_PAGES"); break;
	case MEM_4MB_PAGES:tstrName = TEXT("MEM_4MB_PAGES"); break;
	case MEM_64K_PAGES:tstrName = TEXT("MEM_64K_PAGES"); break;
	case MEM_UNMAP_WITH_TRANSIENT_BOOST:tstrName = TEXT("MEM_UNMAP_WITH_TRANSIENT_BOOST"); break;
	//case MEM_COALESCE_PLACEHOLDERS:tstrName = TEXT("MEM_COALESCE_PLACEHOLDERS"); break;
	case MEM_PRESERVE_PLACEHOLDER:tstrName = TEXT("MEM_PRESERVE_PLACEHOLDER"); break;
	//case MEM_DECOMMIT:tstrName = TEXT("MEM_DECOMMIT"); break;
	case MEM_RELEASE:tstrName = TEXT("MEM_RELEASE"); break;
	case MEM_FREE:tstrName = TEXT("MEM_FREE"); break;
	default:tstrName = TEXT("Unknown"); break;
	}
	return tstrName;
}

//通过flAllocationType获取名字
std::string fGetflAllocationTypeNameA(DWORD  flAllocationType)
{
	std::string tstrName;
	switch (flAllocationType)
	{
	case MEM_COMMIT:tstrName = ("MEM_COMMIT"); break;
	case MEM_RESERVE:tstrName = ("MEM_RESERVE"); break;
	case MEM_REPLACE_PLACEHOLDER:tstrName = ("MEM_REPLACE_PLACEHOLDER"); break;
	case MEM_RESERVE_PLACEHOLDER:tstrName = ("MEM_RESERVE_PLACEHOLDER"); break;
	case MEM_RESET:tstrName = ("MEM_RESET"); break;
	case MEM_TOP_DOWN:tstrName = ("MEM_TOP_DOWN"); break;
	case MEM_WRITE_WATCH:tstrName = ("MEM_WRITE_WATCH"); break;
	case MEM_PHYSICAL:tstrName = ("MEM_PHYSICAL"); break;
	case MEM_ROTATE:tstrName = ("MEM_ROTATE"); break;
		//case MEM_DIFFERENT_IMAGE_BASE_OK:tstrName = TEXT("MEM_DIFFERENT_IMAGE_BASE_OK"); break;
	case MEM_RESET_UNDO:tstrName = ("MEM_RESET_UNDO"); break;
	case MEM_LARGE_PAGES:tstrName = ("MEM_LARGE_PAGES"); break;
	case MEM_4MB_PAGES:tstrName = ("MEM_4MB_PAGES"); break;
	case MEM_64K_PAGES:tstrName = ("MEM_64K_PAGES"); break;
	case MEM_UNMAP_WITH_TRANSIENT_BOOST:tstrName = ("MEM_UNMAP_WITH_TRANSIENT_BOOST"); break;
		//case MEM_COALESCE_PLACEHOLDERS:tstrName = TEXT("MEM_COALESCE_PLACEHOLDERS"); break;
	case MEM_PRESERVE_PLACEHOLDER:tstrName = ("MEM_PRESERVE_PLACEHOLDER"); break;
		//case MEM_DECOMMIT:tstrName = TEXT("MEM_DECOMMIT"); break;
	case MEM_RELEASE:tstrName = ("MEM_RELEASE"); break;
	case MEM_FREE:tstrName = ("MEM_FREE"); break;
	default:tstrName = ("Unknown"); break;
	}
	return tstrName;
}

