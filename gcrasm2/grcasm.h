
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
	int iJumpType ;									//��ת����
	char szTOIp[32] ;									//��תĿ��ASMָ��
	char szEip[32] ;									//��ת��ǰASMָ��
	DWORD dwEip ;									//��ת��ǰ��ַ
	DWORD dwTOIp ;									//��תĿ���ַ
	DWORD dwJMPCount ;							//��ת��ʶ
	DWORD dwJumpTableFirstLabel ;				//��ת��һ����ǩ
	DWORD dwJumpTableSecondLabel ;			//��ת�������ǩ
}OPINFO ,*POPINFO;

typedef struct _JumpTable 
{
	DWORD dwEip ;									//��ת��ǰ��ַ
	DWORD dwTOIp ;									//��תĿ���ַ
	DWORD dwJumpTableFirstLabel ;				//��ת��һ����ǩ
	DWORD dwJumpTableSecondLabel ;			//��ת�������ǩ
}JUMPTABLE ,*PJUMPTABLE;

//////////////////////////////////////////////////////////////////////////
//��������fGetRunableASMCodeToFile
//����˵������ȡָ����Χ�ķ������������溯������ʽ�������ı�
//������
// ulstart (ulong) ����ȡ�������뿪ʼ��ַ
//	ulend (ulong) ����ȡ�������������ַ
//����ֵ��
//	BOOL�����óɹ�����TRUE ������ʧ�ܷ���FALSE
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
//���ݹ����CALL�Ĳ���
extern DWORD gdwMaxCallNumber;
//��ʼ��ϵͳ�����Ϣ��ַ
void InitSysDllInfo();
//���з�����ļ�·��
typedef struct _struct_disassemble_file_path_
{
	std::string strFilePath;
	DWORD dwFunctionVAStart;
	DWORD dwFunctionVAEnd;
}STRUCT_DISASSEMBLE_FILE_PATH,*PSTRUCT_DISASSEMBLE_FILE_PATH;
extern std::vector<STRUCT_DISASSEMBLE_FILE_PATH> gvDisassembleFilePaths;
//���з�����ļ��ĺ�������
extern std::vector<std::string> gvDisassembleFunctionDeclaration;
//�����з�����ļ������ɵ���CPP/H�ļ�
BOOL MakeDisassembleFilesIntoSingle();
typedef struct _struct_call_address_
{
	DWORD dwCallAddress;
	std::string strcalladdress_h;
	std::string strcalladdress_cpp;
}STRUCT_CALL_ADDRESS,*PSTRUCT_CALL_ADDRESS;
//����CALL��ַ
extern std::vector<STRUCT_CALL_ADDRESS> gvdwCallAddress;

int StartGetCallStackToFile(char szFileDirectory[], long  ulstart, long  ulend, int iCallCount);
int fGetCallStackToFile(HANDLE hCurFile, long  ulstart, long  ulend, int iCallCount);

extern std::vector<DWORD> gvdwCallStack;
extern DWORD gdwTick;
extern std::vector<DWORD> gvdwIgnoreCallAddress;
extern std::vector<DWORD> gvdwIATAddress;

extern std::vector<DWORD> gvdwBigCallAddress;
//��ʼ�����Ե�ַ�б�
VOID InitIgnoreCallAddress();
#endif