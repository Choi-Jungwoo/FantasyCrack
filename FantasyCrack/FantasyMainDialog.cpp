// FantasyMainDialog.cpp: 实现文件
//

#include "pch.h"
#include "FantasyCrack.h"
#include "FantasyMainDialog.h"

DWORD handle = (DWORD)GetModuleHandleA("game.bin");


// FantasyMainDialog 对话框

IMPLEMENT_DYNAMIC(FantasyMainDialog, CDialogEx)

FantasyMainDialog::FantasyMainDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DIALOG, pParent)
{

}

FantasyMainDialog::~FantasyMainDialog()
{
}

void FantasyMainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FantasyMainDialog, CDialogEx)
	ON_BN_CLICKED(IDC_PACK_HOOK_BUTTON, &FantasyMainDialog::OnBnClickedPackHookButton)
	ON_BN_CLICKED(IDC_UNDO_PACK_BUTTON, &FantasyMainDialog::OnBnClickedUndoPackButton)
END_MESSAGE_MAP()


// FantasyMainDialog 消息处理程序

void CallDbgView(char* pszFormat, ...)
{
#ifdef _DEBUG

	char bufFormat[0x1000];
	char bufFormatGame[0x1100] = "";
	va_list argList;

	va_start(argList, pszFormat);
	vsprintf_s(bufFormat, pszFormat, argList);
	strcat_s(bufFormatGame, bufFormat);
	OutputDebugStringA(bufFormatGame);
	va_end(argList);

#endif
}

BOOL ToAdminMod(BOOL bEnable)
{
	BOOL fOK = FALSE;
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOK = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOK;
}

HWND GetWindowHandle()
{
	HWND windowHandle = *(HWND*)(handle + 0xB7BFB0);
	return windowHandle;
}

DWORD GPID = 0;
HANDLE GProcessHandle = 0;
DWORD GPackLength = 0;
DWORD GPackAddr = 0;
BYTE* GPackBuffer;
char GByteStr[0x100] = "";
char GDataStr[0x100] = "";
__declspec(naked) void PackHook()
{
	__asm {
		pushad
	}

	__asm {
		mov eax, [esi + 4]
		mov ecx, [esi + 8]
		mov GPackAddr, eax
		sub ecx, eax
		mov GPackLength, ecx
	}

	ToAdminMod(TRUE);
	GetWindowThreadProcessId(GetWindowHandle(), &GPID);
	GProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GPID);

	GPackBuffer = new byte[0x100];
	ReadProcessMemory(GProcessHandle, (LPCVOID)GPackAddr, GPackBuffer, GPackLength, 0);
	*(WORD*)GPackBuffer = GPackLength - 2;

	for (int i = 0; i < (int)GPackLength; i++)
	{
		sprintf_s(GByteStr, "%02X", GPackBuffer[i]);
		strcat_s(GDataStr, GByteStr);
	}

	CallDbgView("[FantasyCrack] PACK_LEN: %d PACK_DATA: %s", GPackLength, GDataStr);

	sprintf_s(GDataStr, "%s", "");
	
	__asm {
		popad
		mov eax, dword ptr[esi + 8]
		sub eax, ecx
		retn
	}
}

void FantasyMainDialog::OnBnClickedPackHookButton()
{
	/*00B92C82    8B46 08         mov     eax, dword ptr[esi + 8]
	00B92C85    2BC1            sub     eax, ecx*/

	DWORD hookedAddr = 0x00B92C82;
	DWORD hookCallAddr = (DWORD)PackHook;
	DWORD jmpValue = hookCallAddr - hookedAddr - 5;

	DWORD oldAttr = 0;
	VirtualProtect((LPVOID)hookedAddr, 100, PAGE_EXECUTE_READWRITE, &oldAttr);

	*(BYTE*)hookedAddr = 0xE8;
	*(DWORD*)(hookedAddr + 1) = jmpValue;

	VirtualProtect((LPVOID)hookedAddr, 100, oldAttr, &oldAttr);
}


void FantasyMainDialog::OnBnClickedUndoPackButton()
{
	DWORD hookedAddr = 0x00B92C82;

	DWORD oldAttr = 0;
	VirtualProtect((LPVOID)hookedAddr, 100, PAGE_EXECUTE_READWRITE, &oldAttr);

	*(BYTE*)hookedAddr = 0x8B;
	*(DWORD*)(hookedAddr + 1) = 0xC12B0846;

	VirtualProtect((LPVOID)hookedAddr, 100, oldAttr, &oldAttr);
}
