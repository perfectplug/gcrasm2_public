// grcasm.cpp : ���� DLL Ӧ�ó���ĵ���������
//


// erasing from list
#include "grcasm.h"
#include "pluginmain.h"
#include "FileOper_2019_8_6.h"
#include "IString.h"


std::vector< STRUCT_SYS_DLL_INFO> gvStructSysDllInfo;
//���з�����ļ�·��
std::vector<STRUCT_DISASSEMBLE_FILE_PATH> gvDisassembleFilePaths;
//���з�����ļ��ĺ�������
std::vector<std::string> gvDisassembleFunctionDeclaration;
//����CALL��ַ
std::vector<STRUCT_CALL_ADDRESS> gvdwCallAddress;
 DWORD dwCodeSegStart=0;
 DWORD dwCodeSegEnd=0;
 char szDisassembleDirectory[MAX_PATH] = {0};
 char gszCallStackDirectory[MAX_PATH] = {0};
 DWORD gdwMaxCallNumber = 0;
std::vector<DWORD> gvdwCallStack;
DWORD gdwTick = 0;


std::vector<DWORD> gvdwIgnoreCallAddress;
std::vector<DWORD> gvdwIATAddress;
//03E2DEFA                | 0000                       | add byte ptr ds:[eax],al                                |
DWORD gdwBigCallAddress = 0x03E2DEFA;
//0164FFFE                                                                | 0000                                   | add byte ptr ds:[eax],al                                                                      |
//��ʼ�����Ե�ַ�б�
VOID InitIgnoreCallAddress()
{



	//////////////////////////////////////////////////////////////////////////
	//9.18
	//gvdwIgnoreCallAddress.push_back(0x0140D520);//0140D520                       | 57                           | push edi                                            |
	//gvdwIgnoreCallAddress.push_back(0x0140DAA0);//0140DAA0                       | 57                           | push edi                                            |
	//gvdwIgnoreCallAddress.push_back(0x0141D450);//
	////gvdwIgnoreCallAddress.push_back(0x006AFB40);
	//gvdwIgnoreCallAddress.push_back(0x00CCE6D0);
	//00D275C0 | FF15 A8A34F01 | call dword ptr ds : [14FA3A8] |
// 	gvdwIATAddress.push_back(0x014FA1CC);
// 	gvdwIATAddress.push_back(0x014FA1D0);
// 	gvdwIATAddress.push_back(0x14FA3A8);

	//////////////////////////////////////////////////////////////////////////
	//9.16
	//00D275B0 <league of legends.sub_D275B0> | 83EC 08 | sub esp, 8 |
// 	gvdwIgnoreCallAddress.push_back(0x01476390);
// 	gvdwIgnoreCallAddress.push_back(0X01475E10);
// 	gvdwIgnoreCallAddress.push_back(0x01486290);
// 	gvdwIgnoreCallAddress.push_back(0x006AFB40);
// 	gvdwIgnoreCallAddress.push_back(0x00D275B0);
// 	//00D275C0 | FF15 A8A34F01 | call dword ptr ds : [14FA3A8] |
// 	gvdwIATAddress.push_back(0x014FA1CC);
// 	gvdwIATAddress.push_back(0x014FA1D0);
// 	gvdwIATAddress.push_back(0x14FA3A8);
}

bool IsIATAddress(DWORD dwAddress)
{
	for (auto _address : gvdwIATAddress)
	{
		if (_address == dwAddress)
		{
			return true;
		}
	}
	return false;
}

 //��ȫ�ֱ���CALL��Ϣ��ӵ��б���
 VOID AddGlobalDWORDCallIntoCallAddressList(DWORD dwCallJumpAddress)
 {
	 BOOL bIsIn = FALSE;
	 for (auto _callAddress: gvdwCallAddress)
	 {
		 if (_callAddress.dwCallAddress == dwCallJumpAddress)
		 {
			 bIsIn = TRUE;
			 break;
		 }
	 }
	 if (!bIsIn)
	 {
		 STRUCT_CALL_ADDRESS structCallAddress;
		 char szTmpBuf[MAX_PATH] = { 0 };
		 structCallAddress.dwCallAddress = dwCallJumpAddress;
		 sprintf(szTmpBuf, "extern DWORD gdw_%08X_%08X;\r\n", dwCallJumpAddress, gdwTick);
		 structCallAddress.strcalladdress_h = szTmpBuf;
		 sprintf(szTmpBuf, "DWORD gdw_%08X_%08X = 0x%08X;\r\n", dwCallJumpAddress, gdwTick, dwCallJumpAddress);
		 structCallAddress.strcalladdress_cpp = szTmpBuf;
		 gvdwCallAddress.push_back(structCallAddress);
	 }
 }

 //��ʼ��ϵͳ�����Ϣ��ַ
 void InitSysDllInfo()
 {
#define NUM_SYS_DLL	3
	 char szSysDlls[NUM_SYS_DLL][MAX_PATH] = {
		 "ntdll.dll","kernel32.dll","KernelBase.dll"
	 };
	 for (int i=0;i< NUM_SYS_DLL;i++)
	 {
		 STRUCT_SYS_DLL_INFO structSysDllInfo;
		 structSysDllInfo.dwAddressStart = Script::Module::BaseFromName(szSysDlls[i]);
		 structSysDllInfo.dwAddressEnd = structSysDllInfo.dwAddressStart + Script::Module::SizeFromName(szSysDlls[i]);
		 gvStructSysDllInfo.push_back(structSysDllInfo);
	 }
 }

 //�鿴��ǰ��ַ�Ƿ���ϵͳ�����
 bool IsAddressInSysDll(DWORD dwAddress)
 {
	 for (auto sysinfo:gvStructSysDllInfo)
	 {
		 if (sysinfo.dwAddressStart<=dwAddress && dwAddress<=sysinfo.dwAddressEnd)
		 {
			 return true;
		 }
	 }
	 return false;
 }

 //�Ժ�������ʶ��
 VOID _AnalysisFunction(DWORD &dwStart, DWORD &dwEnd)
 {
	 DWORD dwCurrentInstrIp = dwStart;
	 while (dwEnd >= dwCurrentInstrIp)
	 {
		 DISASM_INSTR instr = { 0 };
		 DbgDisasmAt(dwCurrentInstrIp, &instr);
		 std::string strInstr = instr.instruction;
		 if (strInstr.find("int3")!=std::string::npos)
		 {//��ǰָ��Ϊint3
			 //���ú���������ַΪint3ָ���ַ
			 dwEnd = dwCurrentInstrIp-1;
			 return;
		 }
		 //ָ����һ��ָ��
		 dwCurrentInstrIp += instr.instr_size;
	 }

 }

int fGetRunableASMCodeToFile(char szFileDirectory[] ,long  ulstart ,long  ulend ,int iCallCount ) 
{

	for (auto _ignorecall:gvdwIgnoreCallAddress)
	{
		if (_ignorecall == ulstart)
		{
			//�ں����б���
			return 3;
		}
	}

	if (iCallCount > gdwMaxCallNumber )
	{//���ò����������ֱ�ӷ���
		return 2;
	}

	//�Ų��Ƿ����ظ��ĺ���
	for (auto _dissassembleFilePath : gvDisassembleFilePaths)
	{
		if (_dissassembleFilePath.dwFunctionVAStart == ulstart)
		{
			return 1;
		}
	}
	char szTmpBuf[2048] ={0} ,szTmpBuf1[MAX_PATH] ={0},szTmpBuf3[MAX_PATH] ={0};
	
	char szBuf[1024] = { 0 };
	std::list<OPINFO> listJump;
	std::list<JUMPTABLE> listJumpTable;
	OPINFO struct_op_Jump ;
	JUMPTABLE struct_jt_JumpTable ;
	char szTmpJmpBuf[MAX_PATH] ={0} ;
	DWORD dwWrittenRet =0 ,dwJMPCount = 0;
	int i =0 ;
	//��������
	char szFunctionDeclaration[MAX_PATH] = { 0 };
	char szCallLabel[MAX_PATH] = { 0 };
	if (Script::Label::Get(ulstart, szCallLabel))
	{//�����ǰ��ַ���ڱ�ǩ
		sprintf(szTmpBuf, "%s\\%d-%08X-%08X-%s.txt", szFileDirectory, iCallCount, ulstart, ulend, szCallLabel);
	}
	else
	{
		sprintf(szTmpBuf, "%s\\%d-%08X-%08X.txt", szFileDirectory, iCallCount, ulstart, ulend);
	}
	
	//���浱ǰ�ļ�·��
	STRUCT_DISASSEMBLE_FILE_PATH structDissassembleFilePath;
	structDissassembleFilePath.strFilePath = szTmpBuf;
	structDissassembleFilePath.dwFunctionVAStart = ulstart;
	structDissassembleFilePath.dwFunctionVAEnd = ulend;
	gvDisassembleFilePaths.push_back(structDissassembleFilePath);
	
	_plugin_logprintf("%s\r\n" ,szTmpBuf) ;
	//ɾ���ļ�
	DeleteFileA(szTmpBuf) ;
	//���ļ�
	HANDLE hCurFile = CreateFileA(szTmpBuf , GENERIC_READ | GENERIC_WRITE ,
		FILE_SHARE_READ |FILE_SHARE_WRITE ,
		NULL ,OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hCurFile == INVALID_HANDLE_VALUE)
	{
		_plugin_logprintf("fGetRunableASMCodeToFile hCurFile == INVALID_HANDLE_VALUE \r\n") ;
		return 3 ;
	}

	if (Script::Label::Get(ulstart, szCallLabel))
	{
		sprintf(szFunctionDeclaration, "void %s_%08X();\r\n", szCallLabel,gdwTick);
		sprintf(szTmpBuf, "__declspec (naked) void %s_%08X()\r\n{\r\n	__asm{\r\n", szCallLabel,gdwTick);
	}
	else
	{
		sprintf(szFunctionDeclaration, "void SUB_%08X_%08X();\r\n", ulstart,gdwTick);
		sprintf(szTmpBuf, "__declspec (naked) void SUB_%08X_%08X()\r\n{\r\n	__asm{\r\n", ulstart,gdwTick);
	}
	
	//���溯������
	gvDisassembleFunctionDeclaration.push_back(szFunctionDeclaration);
	
	//д��ͷ��
	SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
	WriteFile(hCurFile , szTmpBuf ,strlen(szTmpBuf) , &dwWrittenRet ,NULL) ;

	char seps[] = " ";
	char seps1[] = ".>";
	char seps2[] = "," ;
	char seps3[] ="[*" ;
	char *token;
	DWORD dwAddrJumpTable =0 ;
	DWORD dwOffsetJumpTable = 0 ;
	int iJumpTableFirstLabel =0 ,iJumpTableSecondLabel = 0;
	BOOL bInListJumpTable = FALSE ,bInListJump = FALSE ;
	DWORD dwCurrentInstrIp = 0;//��ǰָ���ַ
	//////////////////////////////////////////////////////////////////////////
	//��һ��ɨ��jxx,switch...case��������list��
	dwJMPCount = 0 ;
	listJump.clear() ;
	dwCurrentInstrIp = ulstart;
	while (ulend >= dwCurrentInstrIp)
	{
		DISASM_INSTR instr = {0};
		DbgDisasmAt(dwCurrentInstrIp, &instr);

		if (instr.type == instr_branch)
		{//��ǰָ��Ϊ��תָ��

			if (instr.arg[0].type == arg_memory)
			{//�����ǰ��תָ�����ڴ����
				//���磺jmp dword ptr ds:[edx*4+0x255CE4]
				dwOffsetJumpTable = 0;
				iJumpTableSecondLabel = 0;
				while (TRUE)
				{
					//��ȡ��ת���ַ
					dwAddrJumpTable = Script::Memory::ReadDword(instr.arg[0].constant + dwOffsetJumpTable);
					if (dwAddrJumpTable < ulstart || dwAddrJumpTable >ulend)
					{
						break;
					}

					//������ת��Ŀ�Ľڵ�
					struct_op_Jump.dwJMPCount = 0;
					struct_op_Jump.dwEip = dwCurrentInstrIp;
					struct_op_Jump.dwTOIp = dwAddrJumpTable;
					struct_op_Jump.dwJumpTableFirstLabel = iJumpTableFirstLabel;
					struct_op_Jump.dwJumpTableSecondLabel = iJumpTableSecondLabel;
					struct_op_Jump.iJumpType = JUMP_TYPE_TABLE;
					listJump.push_back(struct_op_Jump);
					//������ת��ڵ�
					struct_jt_JumpTable.dwEip = dwCurrentInstrIp;
					struct_jt_JumpTable.dwTOIp = dwAddrJumpTable;
					struct_jt_JumpTable.dwJumpTableFirstLabel = iJumpTableFirstLabel;
					struct_jt_JumpTable.dwJumpTableSecondLabel = iJumpTableSecondLabel;
					listJumpTable.push_back(struct_jt_JumpTable);

					dwOffsetJumpTable += 4;
					iJumpTableSecondLabel++;
				}
				//dwArrayJumpTableJumpMaxNum[iJumpTableFirstLabel] = iJumpTableSecondLabel;
				iJumpTableFirstLabel++;
			}
			else if (instr.arg[0].type == arg_normal)
			{//ja 0x00255CCF
				if (instr.arg[0].constant > 0)
				{
					struct_op_Jump.dwEip = dwCurrentInstrIp;
					struct_op_Jump.dwJMPCount = dwJMPCount;
					struct_op_Jump.dwTOIp = instr.arg[0].constant;
					struct_op_Jump.iJumpType = JUMP_TYPE_MEMORY_ADDR;
					listJump.push_back(struct_op_Jump);
					dwJMPCount++;
				}
			}
		}
		//ָ����һ��ָ��
		dwCurrentInstrIp += instr.instr_size;
	}
	//////////////////////////////////////////////////////////////////////////
	//�ڶ���ɨ��ָ��д���ļ�
	dwCurrentInstrIp = ulstart;
	while(ulend >= dwCurrentInstrIp)
	{
		//����൱ǰָ��
		DISASM_INSTR instr = { 0 };
		DbgDisasmAt(dwCurrentInstrIp, &instr);

		std::string str_instr(instr.instruction);
		//���Ƶ�ǰָ�������
		sprintf(szTmpBuf, "		%s\r\n", instr.instruction);
		if (instr.type == instr_branch)
		{//�鿴��ǰָ���Ƿ�Ϊ��ָ֧��
			if (str_instr.find("call") != std::string::npos)
			{//callָ��
				
				//CALLҪ��ת���ĵ�ַ
				DWORD dwCallJumpAddress = 0x0;
				if ((instr.arg[0].type == arg_memory) &&
					(instr.arg[0].constant == instr.arg[0].value) &&
					(instr.arg[0].memvalue != 0) && (instr.arg[0].memvalue != 0xcccccccc))
				{
					//call dword ptr ds : [0x0025F000]
					//	instr.type : 00000001
					//	instr.argcount : 00000001
					//	instr.arg.mnemonic : 0025F000
					//	instr.arg.type : 00000001
					//	instr.arg.segment : 00000000
					//	instr.arg.constant : 0025f000
					//	instr.arg.value : 0025f000
					//	instr.arg.memvalue : 002511ea

					if (IsIATAddress(instr.arg[0].constant))
					{
						goto LABEL_NEXT;
					}
					//CALL��ת��ַ
					dwCallJumpAddress = instr.arg[0].memvalue;
				}
				else if ((instr.arg[0].type == arg_normal) &&
					(instr.arg[0].constant == instr.arg[0].value) &&
					(instr.arg[0].constant != 0))
				{
					//call 0x00251267
					//	instr.type:00000001
					//	instr.argcount : 00000001
					//	instr.arg.mnemonic : 251267
					//	instr.arg.type : 00000000
					//	instr.arg.segment : 00000000
					//	instr.arg.constant : 00251267
					//	instr.arg.value : 00251267
					//	instr.arg.memvalue : 00000000
					dwCallJumpAddress = instr.arg[0].constant;
				}
				else
				{

					// 			call eax
					// 				instr.type:00000001
					// 				instr.argcount : 00000001
					// 				instr.arg.mnemonic : eax
					// 				instr.arg.type : 00000000
					// 				instr.arg.segment : 00000000
					// 				instr.arg.constant : 00000000
					// 				instr.arg.value : d84f2d3b
					// 				instr.arg.memvalue : 00000000

					// 			call dword ptr ds : [esi * 4 + 0x400000]
					// 				instr.type : 00000001
					// 				instr.argcount : 00000001
					// 				instr.arg.mnemonic : esi * 4 + 400000
					// 				instr.arg.type : 00000001
					// 				instr.arg.segment : 00000000
					// 				instr.arg.constant : 00400000
					// 				instr.arg.value : 02a44e60
					// 				instr.arg.memvalue : 00000000

					// 				call dword ptr ds : [esi + 0x400]
					// 					instr.type : 00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : esi + 400
					// 					instr.arg.type : 00000001
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00000400
					// 					instr.arg.value : 00991798
					// 					instr.arg.memvalue:cccccccc

					// 				call dword ptr ds : [esi]
					// 					instr.type : 00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : esi
					// 					instr.arg.type : 00000001
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00000000
					// 					instr.arg.value : 00991398
					// 					instr.arg.memvalue:001a23e9
					dwCallJumpAddress = 0;
				}

				if (dwCallJumpAddress >= gdwBigCallAddress)
				{//���������
					//ֱ�Ӹ�Ϊ�����뼴��
					//���µ�ǰָ�������
					sprintf(szTmpBuf, "		//%s ��������Σ�Ҳ�ں����б���1\r\n", instr.instruction);
					//������ָ�ֱ�ӽ�����ָ��ת��Ϊ������
					char szBytesInstr[MAX_PATH] = { 0 };
					for (int i = 0; i < instr.instr_size; i++)
					{
						sprintf(szBytesInstr, "		_emit 0X%02X\r\n", Script::Memory::ReadByte(dwCurrentInstrIp + i));
						strcat(szTmpBuf, szBytesInstr);
					}
				}
				else
				{//�ڴ��������
					if (dwCallJumpAddress != 0 && (IsAddressInSysDll(dwCallJumpAddress) == false))
					{//�����ת��ַ����NULL����ô�����������

						//��鵱ǰ��ת��ַ�Ƿ�Ϊ����
						if (Script::Memory::ReadByte(dwCallJumpAddress) == 0xe9)
						{
							dwCallJumpAddress = dwCallJumpAddress + Script::Memory::ReadDword(dwCallJumpAddress + 1) + 5;
						}
						//���µ�ǰָ�������
						if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
						{
							sprintf(szTmpBuf, "		CALL %s_%08X//�б�ǩ\r\n", szCallLabel, gdwTick);
						}
						else
						{
							sprintf(szTmpBuf, "		CALL SUB_%08X_%08X\r\n", dwCallJumpAddress, gdwTick);
						}

						if (dwCallJumpAddress >= gdwBigCallAddress)
						{//��������Σ��˴���������Ǽ��ܵģ�ֱ�ӷ���ʧ�ܼ���
							//������ǰָ��ʧ�ܣ��޷������CALLָ�ֱ����Ӳ����
							//������CALL�б���
							AddGlobalDWORDCallIntoCallAddressList(dwCallJumpAddress);
							//���µ�ǰָ�������
							sprintf(szTmpBuf, "		CALL gdw_%08X_%08X//��������Σ��˴���������Ǽ��ܵ�1\r\n", dwCallJumpAddress, gdwTick);
						}
						else
						{
							//���ΪCALLָ����ת������δ�
							DWORD dwNextstart = dwCallJumpAddress;
							DWORD dwNextlend = 0;
							//����Ҫ���õĺ���
							Script::Function::FunctionInfo _functionInfo;
							bool bFunctionAnalysisSuccess = false;
							int iTryTimes = 0;
							while (!bFunctionAnalysisSuccess && iTryTimes < 10)
							{

								sprintf(szBuf, "analr %08x", dwCallJumpAddress);
								if (!DbgCmdExec(szBuf))
								{
									_plugin_logprintf("analysis function %08x failed\r\n", dwCallJumpAddress);
								}
								//��ȡ������Ϣ
								bFunctionAnalysisSuccess = Script::Function::GetInfo(dwCallJumpAddress, &_functionInfo);
								sprintf(szBuf, "get info  dwCallJumpAddress = %08x", dwCallJumpAddress);
								OutputDebugStringA(szBuf);
								//��Ϣ100����
								Sleep(100);
								iTryTimes++;
							}
							BOOL bIsIn = FALSE;
							if (bFunctionAnalysisSuccess)
							{
								DWORD _dwModuleBase = Script::Module::BaseFromName(_functionInfo.mod);
								dwNextlend = _dwModuleBase + _functionInfo.rvaEnd;
								DWORD dwNextstartNew = _dwModuleBase + _functionInfo.rvaStart;
								if (dwNextstart != dwNextstartNew)
								{//�����ǰ��ַ���µ�ַ��һ��
									if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
									{
										sprintf(szTmpBuf, "		CALL %s_%08X//�б�ǩ-��ǰ��ַ���µ�ַ:%08X��һ��\r\n", szCallLabel, gdwTick, dwNextstartNew);
									}
									else
									{
										sprintf(szTmpBuf, "		CALL SUB_%08X_%08X//��ǰ��ַ���µ�ַ:%08X��һ��\r\n", dwCallJumpAddress, gdwTick, dwNextstartNew);
									}
								}

								//�����Ų�һ�麯������ʼ��ַ�ͽ�����ַ
								_AnalysisFunction(dwNextstart, dwNextlend);
								sprintf(szBuf, "dwCallJumpAddress = %08x , dwNextstart = %08x,dwNextlend=%08x", dwCallJumpAddress, dwNextstart, dwNextlend);
								OutputDebugStringA(szBuf);

								//�ݹ����
								int iRet = fGetRunableASMCodeToFile(szFileDirectory, dwNextstart, dwNextlend, iCallCount + 1);
								if (iRet == 1)
								{//�õ�ַ�Ѿ�����calllist
									//�˴����Բ��ô���
									//���µ�ǰָ�������
									if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
									{
										sprintf(szTmpBuf, "		CALL %s_%08X//�Ѿ���listcall��\r\n", szCallLabel, gdwTick);
									}
									else
									{
										sprintf(szTmpBuf, "		CALL SUB_%08X_%08X//�Ѿ���listcall��\r\n", dwCallJumpAddress, gdwTick);
									}
								}
								else if (iRet == 2)
								{//�������ݹ����
									if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
									{
										sprintf(szTmpBuf, "		CALL %s_%08X//�ִ����ݹ����%d\r\n", szCallLabel, gdwTick, gdwMaxCallNumber);
									}
									else
									{
										//������CALL�б���
										AddGlobalDWORDCallIntoCallAddressList(dwCallJumpAddress);
										//���µ�ǰָ�������
										sprintf(szTmpBuf, "		CALL gdw_%08X_%08X//�ִ����ݹ����%d\r\n", dwCallJumpAddress, gdwTick, gdwMaxCallNumber);
									}
								}
								else if (iRet == 3)
								{//�ں����б���
									if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
									{
										sprintf(szTmpBuf, "		CALL %s_%08X//���Եĺ���\r\n", szCallLabel, gdwTick);
									}
									else
									{
										//������CALL�б���
										AddGlobalDWORDCallIntoCallAddressList(dwCallJumpAddress);
										//���µ�ǰָ�������
										sprintf(szTmpBuf, "		CALL gdw_%08X_%08X//���Եĺ���\r\n", dwCallJumpAddress, gdwTick);
									}

								}
								//else if (iRet == 4)
								//{//��������Σ�Ҳ�ں����б���
								//	//if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
								//	//{
								//	//	sprintf(szTmpBuf, "		CALL %s_%08X//��������Σ����Եĺ���\r\n", szCallLabel, gdwTick);
								//	//}
								//	//else
								//	{
								//		//������CALL�б���
								//		//AddGlobalDWORDCallIntoCallAddressList(dwCallJumpAddress);

								//		//���µ�ǰָ�������
								//		//sprintf(szTmpBuf, "		CALL gdw_%08X_%08X//��������Σ����Եĺ���\r\n", dwCallJumpAddress, gdwTick);

								//		//ֱ�Ӹ�Ϊ�����뼴��
								//		//���µ�ǰָ�������
								//		sprintf(szTmpBuf, "		//%s ��������Σ�Ҳ�ں����б���2\r\n", instr.instruction);
								//		//������ָ�ֱ�ӽ�����ָ��ת��Ϊ������
								//		char szBytesInstr[MAX_PATH] = { 0 };
								//		for (int i = 0; i < instr.instr_size; i++)
								//		{
								//			sprintf(szBytesInstr, "		_emit 0X%02X\r\n", Script::Memory::ReadByte(dwCurrentInstrIp + i));
								//			strcat(szTmpBuf, szBytesInstr);
								//		}
								//	}
								//}
							}
							else
							{
								if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
								{
									sprintf(szTmpBuf, "		CALL %s_%08X//����ʧ�ܵĺ���1\r\n", szCallLabel, gdwTick);
								}
								else
								{
									//������ǰָ��ʧ�ܣ��޷������CALLָ�ֱ����Ӳ����
									//������CALL�б���
									AddGlobalDWORDCallIntoCallAddressList(dwCallJumpAddress);
									//���µ�ǰָ�������
									sprintf(szTmpBuf, "		CALL gdw_%08X_%08X//����ʧ�ܵĺ���1\r\n", dwCallJumpAddress, gdwTick);
								}
							}
						}
					}
				}
			}
			else if (str_instr.find("jmp") != std::string::npos)
			{//����jmpָ��
				std::string strJmpMnemonic = instr.arg[0].mnemonic;
				if ((instr.arg[0].type == arg_memory) &&
					(strJmpMnemonic.find("*") != std::string::npos))
				{//����swith...case
					// 				jmp dword ptr ds : [edx * 4 + 0x995DA4]
					// 					instr.type : 00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : edx * 4 + 995DA4
					// 					instr.arg.type : 00000001
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00995da4
					// 					instr.arg.value:02fdac04
					// 					instr.arg.memvalue : 00000000
					sprintf(szTmpBuf1, "%s", instr.instruction);
					token = strtok(szTmpBuf1, seps3);
					if (token != NULL)
					{
						token = strtok(NULL, seps3);
						memset(szTmpBuf, 0, 2048);
						for (auto jumptable:listJumpTable)
						{
							if (jumptable.dwEip == dwCurrentInstrIp)
							{
								sprintf(szTmpBuf3, "		CMP %s, 0X%08X\r\n		JE SWITCH_CASE_%08X_%08X\r\n", token,
									jumptable.dwJumpTableSecondLabel,
									jumptable.dwJumpTableFirstLabel,
									jumptable.dwJumpTableSecondLabel);
								strcat(szTmpBuf, szTmpBuf3);
							}
						}
					}
				}
				else if ((instr.arg[0].type == arg_memory))
				{
					// 		jmp dword ptr ds : [esi + 0x400]
					// 			instr.type : 00000001
					// 			instr.argcount : 00000001
					// 			instr.arg.mnemonic : esi + 400
					// 			instr.arg.type : 00000001
					// 			instr.arg.segment : 00000000
					// 			instr.arg.constant : 00000400
					// 			instr.arg.value : 00991798
					// 			instr.arg.memvalue:cccccccc
					//���ﲻ�ô���
				}
				else if (instr.arg[0].type == arg_normal)
				{//��ͨ��jmpָ��
					if ((instr.arg[0].constant == instr.arg[0].value) &&
						(instr.arg[0].constant != 0))
					{
						// 				jmp 0x00995119
						// 					instr.type:00000001
						// 					instr.argcount : 00000001
						// 					instr.arg.mnemonic : 995119
						// 					instr.arg.type : 00000000
						// 					instr.arg.segment : 00000000
						// 					instr.arg.constant : 00995119
						// 					instr.arg.value:00995119
						// 					instr.arg.memvalue:
						
						for (auto jxx : listJump)
						{
							if (jxx.dwEip == dwCurrentInstrIp && (jxx.iJumpType == JUMP_TYPE_MEMORY_ADDR))
							{
								sprintf(szTmpJmpBuf, "%s", instr.instruction);
								token = strtok(szTmpJmpBuf, seps);
								if (Script::Label::Get(instr.arg[0].constant, szCallLabel))
								{
									sprintf(szTmpBuf, "		%s %s_%08X\r\n", token, szCallLabel,gdwTick);
								}
								else
								{
									sprintf(szTmpBuf, "		%s LABEL_%08X\r\n", token, jxx.dwJMPCount);
								}
							}
						}
					}
					else
					{

						// 				jmp eax
						// 					instr.type:00000001
						// 					instr.argcount : 00000001
						// 					instr.arg.mnemonic : eax
						// 					instr.arg.type : 00000000
						// 					instr.arg.segment : 00000000
						// 					instr.arg.constant : 00000000
						// 					instr.arg.value : d84f2d3b
						// 					instr.arg.memvalue : 00000000
						//���ﲻ�ô���
					}

				}
			}
			else if (
				(str_instr.find("ja") != std::string::npos) ||
				(str_instr.find("jae") != std::string::npos) ||

				(str_instr.find("jb") != std::string::npos) ||
				(str_instr.find("jbe") != std::string::npos) ||

				(str_instr.find("je") != std::string::npos) ||

				(str_instr.find("jg") != std::string::npos) ||
				(str_instr.find("jge") != std::string::npos) ||

				
				(str_instr.find("jl") != std::string::npos) ||
				(str_instr.find("jle") != std::string::npos) ||

				(str_instr.find("jp") != std::string::npos) ||
				(str_instr.find("js") != std::string::npos) ||

				(str_instr.find("jnp") != std::string::npos) ||
				
				
				(str_instr.find("jne") != std::string::npos) ||
				(str_instr.find("jns") != std::string::npos) ||
				(str_instr.find("jnz") != std::string::npos) ||
				(str_instr.find("jz") != std::string::npos)
				)
			{//����JXX
				if ((instr.arg[0].constant == instr.arg[0].value) &&
					(instr.arg[0].constant != 0))
				{
					// 				jmp 0x00995119
					// 					instr.type:00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : 995119
					// 					instr.arg.type : 00000000
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00995119
					// 					instr.arg.value:00995119
					// 					instr.arg.memvalue:
					for (auto jxx:listJump)
					{
						if (jxx.dwEip == dwCurrentInstrIp && (jxx.iJumpType == JUMP_TYPE_MEMORY_ADDR))
						{
							if ((str_instr.find("bnd") != std::string::npos))
							{//��ǰָ������bnd
// 								bnd jb 0x01471424
// 									instr.type:00000001
// 									instr.argcount : 00000001
// 									instr.arg.mnemonic : 1471424
// 									instr.arg.type : 00000000
// 									instr.arg.segment : 00000000
// 									instr.arg.constant : 01471424
// 									instr.arg.value : 01471424
// 									instr.arg.memvalue : 00000000
								std::vector<std::string> strv_instr = UtilString::split(str_instr, " ");
								if (Script::Label::Get(instr.arg[0].constant, szCallLabel))
								{
									sprintf(szTmpBuf, "		%s %s %s_%08X//bnd jxx�б�ǩ\r\n", strv_instr[0].c_str(), strv_instr[1].c_str(), szCallLabel,gdwTick);
								}
								else
								{
									sprintf(szTmpBuf, "		%s %s LABEL_%08X//bnd jxx\r\n", strv_instr[0].c_str(), strv_instr[1].c_str(), jxx.dwJMPCount);
								}
							}
							else
							{
								sprintf(szTmpJmpBuf, "%s", instr.instruction);
								token = strtok(szTmpJmpBuf, seps);
								if (Script::Label::Get(instr.arg[0].constant, szCallLabel))
								{
									sprintf(szTmpBuf, "		%s %s_%08X\r\n", token, szCallLabel,gdwTick);
								}
								else
								{
									sprintf(szTmpBuf, "		%s LABEL_%08X\r\n", token, jxx.dwJMPCount);
								}
							}
						}
					}
				}
				else
				{

					// 				jmp eax
					// 					instr.type:00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : eax
					// 					instr.arg.type : 00000000
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00000000
					// 					instr.arg.value : d84f2d3b
					// 					instr.arg.memvalue : 00000000
					//���ﲻ�ô���
				}
			}
		}
		else if (instr.type == instr_normal)
		{
			//�������ָͨ�����Ƿ���nopָ��
			if (str_instr.find("nop") != std::string::npos)
			{
				//���µ�ǰָ�������
				sprintf(szTmpBuf, "		//%s\r\n", instr.instruction);

			}
		}

LABEL_NEXT:
		//������ָ��
		if ((str_instr.find("fstp") != std::string::npos) ||
			(str_instr.find("fistp") != std::string::npos) ||
			(str_instr.find("fadd") != std::string::npos) ||
			(str_instr.find("fild") != std::string::npos) ||
			(str_instr.find("fxch") != std::string::npos) ||
			(str_instr.find("fcomip") != std::string::npos) ||
			(str_instr.find("fdivp") != std::string::npos) ||
			(str_instr.find("fld") != std::string::npos) || 
			(str_instr.find("fimul") != std::string::npos) || 
			(str_instr.find("fmul") != std::string::npos))
		{
			//���µ�ǰָ�������
			sprintf(szTmpBuf, "		//%s\r\n", instr.instruction);
			//������ָ�ֱ�ӽ�����ָ��ת��Ϊ������
			char szFloatInstr[MAX_PATH] = { 0 };
			for (int i = 0; i < instr.instr_size; i++)
			{
				sprintf(szFloatInstr, "		_emit 0X%02X\r\n", Script::Memory::ReadByte(dwCurrentInstrIp + i));
				strcat(szTmpBuf, szFloatInstr);
			}

		// 		fld st0, dword ptr ss : [esp + 0x8]
// 			instr.type : 00000002
// 			instr.argcount : 00000002
// 			instr.arg.mnemonic : st0
// 			instr.arg.type : 00000000
// 			instr.arg.segment : 00000000
// 			instr.arg.constant : 00000000
// 			instr.arg.value : 00000000
// 			instr.arg.memvalue : 00000000

		//fld st0, dword ptr ss : [esp]
// 				fstp dword ptr ds : [esi + 0x44] , st0
// 					instr.type : 00000000
// 					instr.argcount : 00000002
// 					instr.arg.mnemonic : esi + 44
// 					instr.arg.type : 00000001
// 					instr.arg.segment : 00000000
// 					instr.arg.constant : 00000044
// 					instr.arg.value : 000ba6a4
// 					instr.arg.memvalue : 03d964d0
		}

		//����ָ���
		if ((str_instr.find("vcvttss2usi") != std::string::npos) ||
			(str_instr.find("vcvtqq2pd") != std::string::npos))
		{
			//���µ�ǰָ�������
			sprintf(szTmpBuf, "		//%s\r\n", instr.instruction);
			//������ָ�ֱ�ӽ�����ָ��ת��Ϊ������
			char szInstr[MAX_PATH] = { 0 };
			for (int i = 0; i < instr.instr_size; i++)
			{
				sprintf(szInstr, "		_emit 0X%02X\r\n", Script::Memory::ReadByte(dwCurrentInstrIp + i));
				strcat(szTmpBuf, szInstr);
			}
		}


		//����switch...case...��ǩ
		for (auto jxx:listJump)
		{
			if (dwCurrentInstrIp == jxx.dwTOIp)
			{
				if (jxx.iJumpType == JUMP_TYPE_MEMORY_ADDR)
				{
					sprintf(jxx.szTOIp, "LABEL_%08X:\r\n", jxx.dwJMPCount);
				}
				else if (jxx.iJumpType == JUMP_TYPE_TABLE)
				{
					sprintf(jxx.szTOIp, "SWITCH_CASE_%08X_%08X:\r\n", jxx.dwJumpTableFirstLabel, jxx.dwJumpTableSecondLabel);
				}
				//д����ת��ǩ
				SetFilePointer(hCurFile, NULL, NULL, FILE_END);
				WriteFile(hCurFile, jxx.szTOIp, strlen(jxx.szTOIp), &dwWrittenRet, NULL);
				
			}
		}

		SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
		WriteFile(hCurFile , szTmpBuf ,strlen(szTmpBuf) , &dwWrittenRet ,NULL) ;
		//ָ����һ��ָ��
		dwCurrentInstrIp += instr.instr_size;
	}
	//д��β��
	memset(szTmpBuf ,0 ,MAX_PATH) ;
	strcpy(szTmpBuf ,"	}\r\n}\r\n") ;
	SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
	WriteFile(hCurFile , szTmpBuf ,strlen(szTmpBuf) , &dwWrittenRet ,NULL) ;
	CloseHandle(hCurFile) ;
	return 0 ;
}

int StartGetCallStackToFile(char szFileDirectory[], long  ulstart, long  ulend, int iCallCount)
{
	char szTmpBuf[2048] = { 0 };
	char szCallLabel[MAX_PATH] = { 0 };
	if (Script::Label::Get(ulstart, szCallLabel))
	{//�����ǰ��ַ���ڱ�ǩ
		sprintf(szTmpBuf, "%s\\%08X-%08X-%s.txt", szFileDirectory, ulstart, ulend, szCallLabel);
	}
	else
	{
		sprintf(szTmpBuf, "%s\\%08X-%08X.txt", szFileDirectory, ulstart, ulend);
	}

	_plugin_logprintf("%s\r\n", szTmpBuf);
	//ɾ���ļ�
	DeleteFileA(szTmpBuf);
	//���ļ�
	HANDLE hCurFile = CreateFileA(szTmpBuf, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCurFile == INVALID_HANDLE_VALUE)
	{
		_plugin_logprintf("StartGetCallStackToFile hCurFile == INVALID_HANDLE_VALUE \r\n");
		return 3;
	}
	fGetCallStackToFile(hCurFile, ulstart, ulend, iCallCount);
	CloseHandle(hCurFile);
}

int fGetCallStackToFile(HANDLE hCurFile, long  ulstart, long  ulend, int iCallCount)
{
	if (iCallCount > gdwMaxCallNumber)
	{//���ò����������ֱ�ӷ���
		return 2;
	}
	char szCallLabel[MAX_PATH] = { 0 };
	char szTmpBuf[2048] = { 0 };
	char szBuf[1024] = { 0 };
	std::list<OPINFO> listJump;
	std::list<JUMPTABLE> listJumpTable;
	OPINFO struct_op_Jump;
	JUMPTABLE struct_jt_JumpTable;
	char szTmpJmpBuf[MAX_PATH] = { 0 };
	DWORD dwWrittenRet = 0, dwJMPCount = 0;
	int i = 0;
	char seps[] = " ";
	char seps1[] = ".>";
	char seps2[] = ",";
	char seps3[] = "[*";
	char* token;
	DWORD dwAddrJumpTable = 0;
	DWORD dwOffsetJumpTable = 0;
	int iJumpTableFirstLabel = 0, iJumpTableSecondLabel = 0;
	BOOL bInListJumpTable = FALSE, bInListJump = FALSE;
	DWORD dwCurrentInstrIp = 0;//��ǰָ���ַ
	//////////////////////////////////////////////////////////////////////////
	//ɨ��ָ�� ����CALLStack
	dwCurrentInstrIp = ulstart;
	while (ulend >= dwCurrentInstrIp)
	{
		//����൱ǰָ��
		DISASM_INSTR instr = { 0 };
		DbgDisasmAt(dwCurrentInstrIp, &instr);
		std::string str_instr(instr.instruction);
		if (instr.type == instr_branch)
		{//�鿴��ǰָ���Ƿ�Ϊ��ָ֧��
			if (str_instr.find("call") != std::string::npos)
			{//callָ��
				//CALLҪ��ת���ĵ�ַ
				DWORD dwCallJumpAddress = 0x0;
				if ((instr.arg[0].type == arg_memory) &&
					(instr.arg[0].constant == instr.arg[0].value) &&
					(instr.arg[0].memvalue != 0) && (instr.arg[0].memvalue != 0xcccccccc))
				{
					//call dword ptr ds : [0x0025F000]
					//	instr.type : 00000001
					//	instr.argcount : 00000001
					//	instr.arg.mnemonic : 0025F000
					//	instr.arg.type : 00000001
					//	instr.arg.segment : 00000000
					//	instr.arg.constant : 0025f000
					//	instr.arg.value : 0025f000
					//	instr.arg.memvalue : 002511ea
					//CALL��ת��ַ
					dwCallJumpAddress = instr.arg[0].memvalue;
				}
				else if ((instr.arg[0].type == arg_normal) &&
					(instr.arg[0].constant == instr.arg[0].value) &&
					(instr.arg[0].constant != 0))
				{
					//call 0x00251267
					//	instr.type:00000001
					//	instr.argcount : 00000001
					//	instr.arg.mnemonic : 251267
					//	instr.arg.type : 00000000
					//	instr.arg.segment : 00000000
					//	instr.arg.constant : 00251267
					//	instr.arg.value : 00251267
					//	instr.arg.memvalue : 00000000
					dwCallJumpAddress = instr.arg[0].constant;
				}
				else
				{

					// 			call eax
					// 				instr.type:00000001
					// 				instr.argcount : 00000001
					// 				instr.arg.mnemonic : eax
					// 				instr.arg.type : 00000000
					// 				instr.arg.segment : 00000000
					// 				instr.arg.constant : 00000000
					// 				instr.arg.value : d84f2d3b
					// 				instr.arg.memvalue : 00000000

					// 			call dword ptr ds : [esi * 4 + 0x400000]
					// 				instr.type : 00000001
					// 				instr.argcount : 00000001
					// 				instr.arg.mnemonic : esi * 4 + 400000
					// 				instr.arg.type : 00000001
					// 				instr.arg.segment : 00000000
					// 				instr.arg.constant : 00400000
					// 				instr.arg.value : 02a44e60
					// 				instr.arg.memvalue : 00000000

					// 				call dword ptr ds : [esi + 0x400]
					// 					instr.type : 00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : esi + 400
					// 					instr.arg.type : 00000001
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00000400
					// 					instr.arg.value : 00991798
					// 					instr.arg.memvalue:cccccccc

					// 				call dword ptr ds : [esi]
					// 					instr.type : 00000001
					// 					instr.argcount : 00000001
					// 					instr.arg.mnemonic : esi
					// 					instr.arg.type : 00000001
					// 					instr.arg.segment : 00000000
					// 					instr.arg.constant : 00000000
					// 					instr.arg.value : 00991398
					// 					instr.arg.memvalue:001a23e9
					dwCallJumpAddress = 0;
				}

				//if ()
				//{

				//}
				if (dwCallJumpAddress != 0 && (IsAddressInSysDll(dwCallJumpAddress) == false))
				{//�����ת��ַ����NULL����ô�����������

					//��鵱ǰ��ת��ַ�Ƿ�Ϊ����
					if (Script::Memory::ReadByte(dwCallJumpAddress) == 0xe9)
					{
						dwCallJumpAddress = dwCallJumpAddress + Script::Memory::ReadDword(dwCallJumpAddress + 1) + 5;
					}
					//CALL�ظ��Լ��
					bool bIsIn = false;
					for (auto _callstack : gvdwCallStack)
					{
						if (_callstack == dwCallJumpAddress)
						{
							bIsIn = true;
							break;
						}
					}
					if (bIsIn == false)
					{
						gvdwCallStack.push_back(dwCallJumpAddress);

						//���µ�ǰָ�������
						if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
						{
							sprintf(szTmpBuf, "%d-CALL %s//�б�ǩ\r\n", iCallCount, szCallLabel);
						}
						else
						{
							sprintf(szTmpBuf, "%d-CALL SUB_%08X\r\n", iCallCount, dwCallJumpAddress);
						}

						//���ΪCALLָ����ת������δ�
						DWORD dwNextstart = dwCallJumpAddress;
						DWORD dwNextlend = 0;
						//����Ҫ���õĺ���
						Script::Function::FunctionInfo _functionInfo;
						bool bFunctionAnalysisSuccess = false;
						int iTryTimes = 0;
						while (!bFunctionAnalysisSuccess && iTryTimes < 10)
						{

							sprintf(szBuf, "analr %08x", dwCallJumpAddress);
							if (!DbgCmdExec(szBuf))
							{
								_plugin_logprintf("analysis function %08x failed\r\n", dwCallJumpAddress);
							}
							//��ȡ������Ϣ
							bFunctionAnalysisSuccess = Script::Function::GetInfo(dwCallJumpAddress, &_functionInfo);
							sprintf(szBuf, "get info  dwCallJumpAddress = %08x", dwCallJumpAddress);
							OutputDebugStringA(szBuf);
							//��Ϣ100����
							Sleep(100);
							iTryTimes++;
						}
						BOOL bIsIn = FALSE;
						if (bFunctionAnalysisSuccess)
						{
							DWORD _dwModuleBase = Script::Module::BaseFromName(_functionInfo.mod);
							dwNextlend = _dwModuleBase + _functionInfo.rvaEnd;
							DWORD dwNextstartNew = _dwModuleBase + _functionInfo.rvaStart;
							if (dwNextstart != dwNextstartNew)
							{//�����ǰ��ַ���µ�ַ��һ��
								if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
								{
									sprintf(szTmpBuf, "%d-CALL %s//�б�ǩ-��ǰ��ַ���µ�ַ:%08X��һ��\r\n", iCallCount, szCallLabel, dwNextstartNew);
								}
								else
								{
									sprintf(szTmpBuf, "%d-CALL SUB_%08X//��ǰ��ַ���µ�ַ:%08X��һ��\r\n", iCallCount, dwCallJumpAddress, dwNextstartNew);
								}
							}

							//�����Ų�һ�麯������ʼ��ַ�ͽ�����ַ
							_AnalysisFunction(dwNextstart, dwNextlend);
							sprintf(szBuf, "dwCallJumpAddress = %08x , dwNextstart = %08x,dwNextlend=%08x", dwCallJumpAddress, dwNextstart, dwNextlend);
							OutputDebugStringA(szBuf);

							//�ݹ����
							int iRet = fGetCallStackToFile(hCurFile, dwNextstart, dwNextlend, iCallCount + 1);
							if (iRet == 1)
							{//�õ�ַ�Ѿ�����calllist
								//�˴����Բ��ô���
								//���µ�ǰָ�������
								if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
								{
									sprintf(szTmpBuf, "%d-CALL %s//�Ѿ���listcall��\r\n", iCallCount, szCallLabel);
								}
								else
								{
									sprintf(szTmpBuf, "%d-CALL SUB_%08X//�Ѿ���listcall��\r\n", iCallCount, dwCallJumpAddress);
								}
							}
							else if (iRet == 2)
							{//�������ݹ����
								if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
								{
									sprintf(szTmpBuf, "%d-CALL %s//�ִ����ݹ����%d\r\n", iCallCount, szCallLabel, gdwMaxCallNumber);
								}
								else
								{
									//���µ�ǰָ�������
									sprintf(szTmpBuf, "%d-CALL SUB_%08X//�ִ����ݹ����%d\r\n", iCallCount, dwCallJumpAddress, gdwMaxCallNumber);
								}
							}
						}
						else
						{
							if (Script::Label::Get(dwCallJumpAddress, szCallLabel))
							{
								sprintf(szTmpBuf, "%d-CALL %s//����ʧ�ܵĺ���2\r\n", iCallCount, szCallLabel);
							}
							else
							{
								//������ǰָ��ʧ�ܣ��޷������CALLָ�ֱ����Ӳ����
								//���µ�ǰָ�������
								sprintf(szTmpBuf, "%d-CALL SUB_%08X//����ʧ�ܵĺ���2\r\n", iCallCount, dwCallJumpAddress);
							}
						}


						SetFilePointer(hCurFile, NULL, NULL, FILE_END);
						WriteFile(hCurFile, szTmpBuf, strlen(szTmpBuf), &dwWrittenRet, NULL);
					}
				}
			}
		}
		//ָ����һ��ָ��
		dwCurrentInstrIp += instr.instr_size;
	}
	return 0;
}

void fGetArrayHEXToFile(int itype ,long ulstart ,long ulend)
{
	char szTmpOneBuf[20] ={0} ;
	char szFilePath[MAX_PATH] ={0} ;
	sprintf(szFilePath ,"%s" ,"C:\\ARRAY_HEX_.txt") ;
	DeleteFileA(szFilePath) ;
	HANDLE hCurFile = CreateFileA(szFilePath , GENERIC_READ | GENERIC_WRITE ,
		FILE_SHARE_READ |FILE_SHARE_WRITE ,
		NULL ,OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL ,NULL) ;
	if (hCurFile == INVALID_HANDLE_VALUE)
	{
		_plugin_logprintf("fGetArrayHEXToFile hCurFile == INVALID_HANDLE_VALUE \r\n") ;
		return  ;
	}
	long lBufLen = ulend - ulstart ;
	BYTE *pBuf = new BYTE[lBufLen] ;
	memset(pBuf ,0 ,lBufLen) ;
	duint sizeRead = 0;
	Script::Memory::Read(ulstart, pBuf, lBufLen, &sizeRead);
	DWORD dwWrittenRet =0 ;
	if (itype == TYPE_GET_BYTE_ARRAY)
	{
		for (int i = 0  ;i < ulend - ulstart ;i =i +itype)
		{
			if (i == 0)
			{
				sprintf(szTmpOneBuf ,"0X%02X" , *(BYTE*)((DWORD)pBuf + i) ) ;
			}
			else
			{
				if ( i%16 ==0 )
				{
					sprintf(szTmpOneBuf ,",0X%02X\r\n" ,*(BYTE*)((DWORD)pBuf + i) ) ;
				}
				else
				{
					sprintf(szTmpOneBuf ,",0X%02X" ,*(BYTE*)((DWORD)pBuf + i) ) ;
				}
			}
			SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
			WriteFile(hCurFile , szTmpOneBuf ,strlen(szTmpOneBuf) , &dwWrittenRet ,NULL) ;
		}
	}
	else if (itype == TYPE_GET_WORD_ARRAY)
	{
		for (int i =0  ;i < ulend -ulstart ;i =i +itype)
		{
			if (i == 0)
			{
				sprintf(szTmpOneBuf ,"0X%04X" ,*(WORD*)((DWORD)pBuf + i)) ;
			}
			else
			{
				if ( i%16 ==0 )
				{
					sprintf(szTmpOneBuf ,",0X%04X\r\n" ,*(WORD*)((DWORD)pBuf + i) ) ;
				}
				else
				{
					sprintf(szTmpOneBuf ,",0X%04X" ,*(WORD*)((DWORD)pBuf + i) ) ;
				}
			}
			SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
			WriteFile(hCurFile , szTmpOneBuf ,strlen(szTmpOneBuf) , &dwWrittenRet ,NULL) ;
		}
	}
	else if (itype == TYPE_GET_DWORD_ARRAY )
	{
		for (int i =0  ;i <ulend -ulstart ;i =i +itype)
		{
			if (i == 0)
			{
				sprintf(szTmpOneBuf ,"0X%08X" ,*(DWORD*)((DWORD)pBuf + i) ) ;
			}
			else
			{
				if ( i%16 ==0 )
				{
					sprintf(szTmpOneBuf ,",0X%08X\r\n" ,*(DWORD*)((DWORD)pBuf + i) ) ;
				}
				else
				{
					sprintf(szTmpOneBuf ,",0X%08X" ,*(DWORD*)((DWORD)pBuf + i) ) ;
				}
			}
			SetFilePointer(hCurFile ,NULL ,NULL ,FILE_END) ;
			WriteFile(hCurFile , szTmpOneBuf ,strlen(szTmpOneBuf) , &dwWrittenRet ,NULL) ;
		}
	}
	delete []pBuf ;
	pBuf = NULL ;
	CloseHandle(hCurFile) ;
}

//�����з�����ļ������ɵ���CPP/H�ļ�
BOOL MakeDisassembleFilesIntoSingle()
{
	//����.h�ļ�
	DWORD dwTickCount = GetTickCount();
	char szTmpBuf[MAX_PATH] = { 0 };
	sprintf(szTmpBuf, "%s\\%08X-%08X-%08X.h", szDisassembleDirectory, dwTickCount, dwCodeSegStart, dwCodeSegEnd);
	DeleteFileA(szTmpBuf);
	HANDLE hCurFile = CreateFileA(szTmpBuf, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCurFile == INVALID_HANDLE_VALUE)
	{
		_plugin_logprintf("MakeDisassembleFilesIntoSingle hCurFile == INVALID_HANDLE_VALUE \r\n");
		return FALSE;
	}
	DWORD dwWrittenRet = 0;
	//д�����ͷ�ļ�
	sprintf(szTmpBuf, "#include <windows.h>\r\n");
	SetFilePointer(hCurFile, NULL, NULL, FILE_END);
	WriteFile(hCurFile, szTmpBuf, strlen(szTmpBuf), &dwWrittenRet, NULL);
	//д��ȫ�ֱ�������
	for (auto valueDefine:gvdwCallAddress)
	{
		SetFilePointer(hCurFile, NULL, NULL, FILE_END);
		WriteFile(hCurFile, valueDefine.strcalladdress_h.c_str(), valueDefine.strcalladdress_h.size(), &dwWrittenRet, NULL);
	}
	//д�뺯������
	for (auto functionDeclaration: gvDisassembleFunctionDeclaration)
	{
		SetFilePointer(hCurFile, NULL, NULL, FILE_END);
		WriteFile(hCurFile, functionDeclaration.c_str(), functionDeclaration.size(), &dwWrittenRet, NULL);
	}
	CloseHandle(hCurFile);
	//����.cpp�ļ�
	sprintf(szTmpBuf, "%s\\%08X-%08X-%08X.cpp", szDisassembleDirectory, dwTickCount, dwCodeSegStart, dwCodeSegEnd);
	DeleteFileA(szTmpBuf);
	hCurFile = CreateFileA(szTmpBuf, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCurFile == INVALID_HANDLE_VALUE)
	{
		_plugin_logprintf("MakeDisassembleFilesIntoSingle hCurFile == INVALID_HANDLE_VALUE \r\n");
		return FALSE;
	}
	sprintf(szTmpBuf, "#include \"%08X-%08X-%08X.h\"\r\n", dwTickCount, dwCodeSegStart, dwCodeSegEnd);
	SetFilePointer(hCurFile, NULL, NULL, FILE_END);
	WriteFile(hCurFile, szTmpBuf, strlen(szTmpBuf), &dwWrittenRet, NULL);
	//д��ȫ�ֱ�������
	for (auto valueDefine : gvdwCallAddress)
	{
		SetFilePointer(hCurFile, NULL, NULL, FILE_END);
		WriteFile(hCurFile, valueDefine.strcalladdress_cpp.c_str(), valueDefine.strcalladdress_cpp.size(), &dwWrittenRet, NULL);
	}
	//������ָ��д��һ��CPP�ļ���
	for (auto _dissassembleFilePath : gvDisassembleFilePaths)
	{
		DWORD dwReadBytes=0;
		char* pbuf = nullptr;
		if (!FileOper_2019_8_6::ReadFileA(_dissassembleFilePath.strFilePath.c_str(), pbuf, dwReadBytes))
		{
			_plugin_logprintf("MakeDisassembleFilesIntoSingle read disassemble file path failed \r\n");
			continue;
		}
		SetFilePointer(hCurFile, NULL, NULL, FILE_END);
		WriteFile(hCurFile, pbuf, dwReadBytes, &dwWrittenRet, NULL);
		SetFilePointer(hCurFile, NULL, NULL, FILE_END);
		WriteFile(hCurFile, "\r\n", strlen("\r\n"), &dwWrittenRet, NULL);
	}
	CloseHandle(hCurFile);
}
