#include "plugin.h"
#include "grcasm.h"
#include "ProcessOper.h"

enum
{
    MENU_TEST,
    MENU_DISASM_ADLER32,
    MENU_DUMP_ADLER32,
    MENU_STACK_ADLER32,
	MENU_DISASM_5,//递归抠取
	MENU_DISASM_10,//递归抠取
	MENU_DISASM_15,//递归抠取
	MENU_DISASM_20,//递归抠取
	MENU_DISASM_25,//递归抠取
	MENU_DISASM_30,//递归抠取
	MENU_ANALYSIS_CALLSTACK_5,//分析调用栈5
	MENU_ANALYSIS_CALLSTACK_10,//分析调用栈10
	MENU_ANALYSIS_CALLSTACK_15,//分析调用栈15
	MENU_ANALYSIS_CALLSTACK_20,//分析调用栈20
	MENU_ANALYSIS_CALLSTACK_25,//分析调用栈25
	MENU_ANALYSIS_CALLSTACK_30,//分析调用栈30
};

//初始化反汇编目录
BOOL InitDisassembleDirectory(int hWindow)
{
	SELECTIONDATA sel;
	GuiSelectionGet(hWindow, &sel);
	char szCurDirectory[MAX_PATH] = { 0 };
	if (fGetCurrentPathDirectoryA(szCurDirectory) != 0)
	{
		dputs("fGetCurrentPathDirectoryA failed");
		return FALSE;
	}	
	sprintf(szDisassembleDirectory, "%splugins\\gcrasm2", szCurDirectory);
	if (!CreateDirectoryA(szDisassembleDirectory, NULL))
	{
		dputs("CreateDirectoryA gcrasm2 failed ");
	}
	gdwTick = GetTickCount();
	sprintf(szDisassembleDirectory, "%splugins\\gcrasm2\\%08x-%08x-%08x", szCurDirectory, gdwTick, sel.start, sel.end);
	if (!CreateDirectoryA(szDisassembleDirectory, NULL))
	{
		dputs("CreateDirectoryA gcrasm2\\gettickcount-start-end failed");
		return FALSE;
	}
	
	return TRUE;
}

//初始化CallStack目录
BOOL InitCallStackDirectory(int hWindow)
{
	SELECTIONDATA sel;
	GuiSelectionGet(hWindow, &sel);
	char szCurDirectory[MAX_PATH] = { 0 };
	if (fGetCurrentPathDirectoryA(szCurDirectory) != 0)
	{
		dputs("fGetCurrentPathDirectoryA failed");
		return FALSE;
	}
	sprintf(gszCallStackDirectory, "%splugins\\gcrasm2", szCurDirectory);
	if (!CreateDirectoryA(gszCallStackDirectory, NULL))
	{
		dputs("CreateDirectoryA gcrasm2 failed ");
	}
	sprintf(gszCallStackDirectory, "%splugins\\gcrasm2\\callstack-%08x-%08x-%08x", szCurDirectory, GetTickCount(), sel.start, sel.end);
	if (!CreateDirectoryA(gszCallStackDirectory, NULL))
	{
		dputs("CreateDirectoryA gcrasm2\\callstack-gettickcount-start-end failed");
		return FALSE;
	}

	return TRUE;
}

//反汇编CPU窗口选中项
static void DisassembleCpuWndSelection(int hWindow)
{
	if (!DbgIsDebugging())
	{
		dputs("You need to be debugging to use this command");
		return;
	}
	if (!InitDisassembleDirectory(hWindow))
	{
		return;
	}
	//
	gvdwIgnoreCallAddress.clear();
	gvDisassembleFilePaths.clear();
	gvDisassembleFunctionDeclaration.clear();
	gvStructSysDllInfo.clear();
	gvdwCallAddress.clear();
	gvdwIATAddress.clear();
	//初始化系统组件信息
	InitSysDllInfo();
	//初始化忽略地址列表
	InitIgnoreCallAddress();
	
	SELECTIONDATA sel;
	GuiSelectionGet(hWindow, &sel);
	duint len = sel.end - sel.start + 1;
	dwCodeSegStart = sel.start;
	dwCodeSegEnd = sel.end;
	dputs("Disassemble start...");
	fGetRunableASMCodeToFile(szDisassembleDirectory, sel.start, sel.end, 0);

	dputs("Disassemble end...");
}

static void DisassembleCpuWndSelectionMakeCallStackToFile(int hWindow)
{
	if (!DbgIsDebugging())
	{
		dputs("You need to be debugging to use this command");
		return;
	}
	if (!InitCallStackDirectory(hWindow))
	{
		return;
	}
	gvdwCallStack.clear();
	gvStructSysDllInfo.clear();
	//初始化系统组件信息
	InitSysDllInfo();
	
	SELECTIONDATA sel;
	GuiSelectionGet(hWindow, &sel);
	duint len = sel.end - sel.start + 1;
	dwCodeSegStart = sel.start;
	dwCodeSegEnd = sel.end;
	dputs("CallStack start...");
	StartGetCallStackToFile(gszCallStackDirectory, sel.start, sel.end, 0);

	dputs("CallStack end...");
	
}

static void Adler32Menu(int hWindow)
{
    if(!DbgIsDebugging())
    {
        dputs("You need to be debugging to use this command");
        return;
    }
    SELECTIONDATA sel;
    GuiSelectionGet(hWindow, &sel);
    duint len = sel.end - sel.start + 1;
    unsigned char* data = new unsigned char[len];
    if(DbgMemRead(sel.start, data, len))
    {
        DWORD a = 1, b = 0;
        for(duint index = 0; index < len; ++index)
        {
            a = (a + data[index]) % 65521;
            b = (b + a) % 65521;
        }
        delete[] data;
        DWORD checksum = (b << 16) | a;
        dprintf("Adler32 of %p[%X] is: %08X\n", sel.start, len, checksum);
    }
    else
        dputs("DbgMemRead failed...");
}

static bool cbTestCommand(int argc, char* argv[])
{
    dputs("Test command!");
    char line[GUI_MAX_LINE_SIZE] = "";
    if(!GuiGetLineWindow("test", line))
        dputs("Cancel pressed!");
    else
        dprintf("Line: \"%s\"\n", line);
    return true;
}

static duint exprZero(int argc, duint* argv, void* userdata)
{
    return 0;
}

PLUG_EXPORT void CBINITDEBUG(CBTYPE cbType, PLUG_CB_INITDEBUG* info)
{
    dprintf("Debugging of %s started!\n", info->szFileName);
}

PLUG_EXPORT void CBSTOPDEBUG(CBTYPE cbType, PLUG_CB_STOPDEBUG* info)
{
    dputs("Debugging stopped!");
}

PLUG_EXPORT void CBEXCEPTION(CBTYPE cbType, PLUG_CB_EXCEPTION* info)
{
    //dprintf("ExceptionRecord.ExceptionCode: %08X\n", info->Exception->ExceptionRecord.ExceptionCode);
}

PLUG_EXPORT void CBDEBUGEVENT(CBTYPE cbType, PLUG_CB_DEBUGEVENT* info)
{
//     if(info->DebugEvent->dwDebugEventCode == EXCEPTION_DEBUG_EVENT)
//     {
//         dprintf("DebugEvent->EXCEPTION_DEBUG_EVENT->%.8X\n", info->DebugEvent->u.Exception.ExceptionRecord.ExceptionCode);
//     }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_TEST:
        MessageBoxA(hwndDlg, "From 2013 to 2019,I'm back!", PLUGIN_NAME, MB_ICONINFORMATION);
        break;

    case MENU_DISASM_ADLER32:
        Adler32Menu(GUI_DISASSEMBLY);
        break;

    case MENU_DUMP_ADLER32:
        Adler32Menu(GUI_DUMP);
        break;

    case MENU_STACK_ADLER32:
        Adler32Menu(GUI_STACK);
        break;

	case MENU_DISASM_5:
		gdwMaxCallNumber = 5;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_DISASM_10:
		gdwMaxCallNumber = 10;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_DISASM_15:
		gdwMaxCallNumber = 15;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_DISASM_20:
		gdwMaxCallNumber = 20;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_DISASM_25:
		gdwMaxCallNumber = 25;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_DISASM_30:
		gdwMaxCallNumber = 30;
		DisassembleCpuWndSelection(GUI_DISASSEMBLY);
		MakeDisassembleFilesIntoSingle();
		break;
	case MENU_ANALYSIS_CALLSTACK_5:
		gdwMaxCallNumber = 5;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
	case MENU_ANALYSIS_CALLSTACK_10:
		gdwMaxCallNumber = 10;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
	case MENU_ANALYSIS_CALLSTACK_15:
		gdwMaxCallNumber = 15;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
	case MENU_ANALYSIS_CALLSTACK_20:
		gdwMaxCallNumber = 20;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
	case MENU_ANALYSIS_CALLSTACK_25:
		gdwMaxCallNumber = 25;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
	case MENU_ANALYSIS_CALLSTACK_30:
		gdwMaxCallNumber = 30;
		DisassembleCpuWndSelectionMakeCallStackToFile(GUI_DISASSEMBLY);
		break;
    default:
        break;
    }
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, PLUGIN_NAME, cbTestCommand, false);
    _plugin_registerexprfunction(pluginHandle, PLUGIN_NAME ".zero", 0, exprZero, nullptr);
    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU_TEST, "&About");
    _plugin_menuaddentry(hMenuDisasm, MENU_DISASM_5, "&disassemble 5");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_10, "&disassemble 10");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_15, "&disassemble 15");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_20, "&disassemble 20");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_25, "&disassemble 25");
	_plugin_menuaddentry(hMenuDisasm, MENU_DISASM_30, "&disassemble 30");

	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_5, "&callstack 5");
	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_10, "&callstack 10");
	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_15, "&callstack 15");
	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_20, "&callstack 20");
	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_25, "&callstack 25");
	_plugin_menuaddentry(hMenuDisasm, MENU_ANALYSIS_CALLSTACK_30, "&callstack 30");
    //_plugin_menuaddentry(hMenuDump, MENU_DUMP_ADLER32, "&Adler32 Selection");
    //_plugin_menuaddentry(hMenuStack, MENU_STACK_ADLER32, "&Adler32 Selection");
}
