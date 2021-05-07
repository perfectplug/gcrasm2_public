
#ifndef _grcasm_h_PERFECTPLUG_
#define _grcasm_h_PERFECTPLUG_
#include "PPBase.h"

#define JUMP_TYPE_MEMORY_ADDR			0X1
#define JUMP_TYPE_TABLE					0X2
 
typedef struct _struct_sys_dll_info
{
	DWORD dwAddressStart;
	DWORD dwAddressEnd;
}STRUCT_SYS_DLL_INFO;

typedef struct _OpInfo
{
	int iJumpType ;									//跳转类型
	char szTOIp[32] ;									//跳转目标ASM指令
	char szEip[32] ;									//跳转当前ASM指令
	DWORD dwEip ;									//跳转当前地址
	DWORD dwTOIp ;									//跳转目标地址
	DWORD dwJMPCount ;							//跳转标识
	DWORD dwJumpTableFirstLabel ;				//跳转表一级标签
	DWORD dwJumpTableSecondLabel ;			//跳转表二级标签
}OPINFO ,*POPINFO;

typedef struct _JumpTable 
{
	DWORD dwEip ;									//跳转表当前地址
	DWORD dwTOIp ;									//跳转目标地址
	DWORD dwJumpTableFirstLabel ;				//跳转表一级标签
	DWORD dwJumpTableSecondLabel ;			//跳转表二级标签
}JUMPTABLE ,*PJUMPTABLE;

//////////////////////////////////////////////////////////////////////////
//函数名：fGetRunableASMCodeToFile
//功能说明：获取指定范围的反汇编代码以裸替函数的形式保存至文本
//参数：
// ulstart (ulong) ，获取反汇编代码开始地址
//	ulend (ulong) ，获取反汇编代码结束地址
//返回值：
//	BOOL，调用成功返回TRUE ，调用失败返回FALSE
BOOL fGetRunableASMCodeToFile(char szFileDirectory[] ,long  ulstart ,long  ulend ,int iCallCount )   ;


#define TYPE_GET_BYTE_ARRAY		1
#define TYPE_GET_WORD_ARRAY	2
#define TYPE_GET_DWORD_ARRAY	4
void fGetArrayHEXToFile(int itype ,long ulstart ,long ulend);



void fuck(DWORD dwNextAddr,DWORD dwSaveAddr) ;

extern DWORD dwCodeSegStart;
extern DWORD dwCodeSegEnd;
extern char szDisassembleDirectory[MAX_PATH] ;
extern char gszCallStackDirectory[MAX_PATH];
extern std::vector< STRUCT_SYS_DLL_INFO> gvStructSysDllInfo;
//最大递归调用CALL的层数
extern DWORD gdwMaxCallNumber;
//初始化系统组件信息地址
void InitSysDllInfo();
//所有反汇编文件路径
typedef struct _struct_disassemble_file_path_
{
	std::string strFilePath;
	DWORD dwFunctionVAStart;
	DWORD dwFunctionVAEnd;
}STRUCT_DISASSEMBLE_FILE_PATH,*PSTRUCT_DISASSEMBLE_FILE_PATH;
extern std::vector<STRUCT_DISASSEMBLE_FILE_PATH> gvDisassembleFilePaths;
//所有反汇编文件的函数声明
extern std::vector<std::string> gvDisassembleFunctionDeclaration;
//将所有反汇编文件导出成单个CPP/H文件
BOOL MakeDisassembleFilesIntoSingle();
typedef struct _struct_call_address_
{
	DWORD dwCallAddress;
	std::string strcalladdress_h;
	std::string strcalladdress_cpp;
}STRUCT_CALL_ADDRESS,*PSTRUCT_CALL_ADDRESS;
//所有CALL地址
extern std::vector<STRUCT_CALL_ADDRESS> gvdwCallAddress;

int StartGetCallStackToFile(char szFileDirectory[], long  ulstart, long  ulend, int iCallCount);
int fGetCallStackToFile(HANDLE hCurFile, long  ulstart, long  ulend, int iCallCount);

extern std::vector<DWORD> gvdwCallStack;
extern DWORD gdwTick;
extern std::vector<DWORD> gvdwIgnoreCallAddress;
extern std::vector<DWORD> gvdwIATAddress;

extern std::vector<DWORD> gvdwBigCallAddress;
//初始化忽略地址列表
VOID InitIgnoreCallAddress();
#endif