#include <Windows.h>

#include "MyHook.h"

#include <fstream>
#include <time.h>  

/* 这是32位下的hook
typedef int(*ExecuteMAXScriptScript)(const WCHAR* s, BOOL quietErrors, void* fpv);
ExecuteMAXScriptScript OldFunc;	//指向原函数的指针
FARPROC pfOld;					//指向函数的远指针

//（我们定制的这条指令的大小）, 这里是5，5个字节嘛
BYTE NewCode[5];
// 将原API的入口前5个字节代码保存到OldCode[]
BYTE OldCode[5];

HANDLE hProcess;

int HookCommandLen = 5;

bool isHooked = false;

int MyExecuteMAXScriptScript(const WCHAR* s, BOOL quietErrors, void* fpv) {

	MessageBoxW(NULL, s, L"hook send", 0);
	return 0;
}

bool HookExecuteMAXScriptScriptAddress()
{
	DWORD dwPid = ::GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwPid);

	char path[] = "CommonDll.dll";
	HMODULE hmod = LoadLibraryEx(path, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!hmod) {
		std::cout << "加载DLL失败，退出。" << std::endl;
		MessageBoxW(NULL, L"加载DLL失败，退出。", L"hook send", 0);
		return false;
	}

	OldFunc = (ExecuteMAXScriptScript)::GetProcAddress(hmod, "exprot_test_str");
	pfOld = (FARPROC)OldFunc;

	_asm
	{
		lea edi, OldCode					//获取OldCode数组的地址,放到edi
		mov esi, pfOld						//获取原API入口地址，放到esi
		cld									//方向标志位，为以下两条指令做准备
		movsd								//复制原API入口前4个字节到OldCode数组
		movsb								//复制原API入口第5个字节到OldCode数组
	}

	// int nAddr= UserFunAddr – SysFunAddr - （我们定制的这条指令的大小）;
	// Jmp nAddr;
	_asm
	{
		lea eax, MyExecuteMAXScriptScript	//获取我们的MyMessageBoxW函数地址
		mov ebx, pfOld						//原系统API函数地址
		sub eax, ebx						//int nAddr= UserFunAddr – SysFunAddr
		sub eax, 5							//nAddr=nAddr-5
		mov dword ptr[NewCode + 1], eax		//将算出的地址nAddr保存到NewCode后面4个字节
											//注：一个函数地址占4个字节
	}

	return true;
}

void HookOn()
{
	assert(hProcess != NULL);

	DWORD dwTemp = 0;
	DWORD dwOldProtect;

	//修改API函数入口前5个字节为jmp xxxxxx
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfOld, NewCode, HookCommandLen, 0);
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, dwOldProtect, &dwTemp);

}

void HookOff()
{
	assert(hProcess != NULL);

	DWORD dwTemp = 0;
	DWORD dwOldProtect;

	//恢复API函数入口前5个字节
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfOld, OldCode, HookCommandLen, 0);
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, dwOldProtect, &dwTemp);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		MessageBoxW(NULL, L"注入成功。", L"狗子", 0);

		isHooked = HookExecuteMAXScriptScriptAddress();
		if (isHooked)HookOn();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		if (isHooked)HookOff();

	}

	return TRUE;
}
*/

void log(const char* msg)
{
	time_t nowtime;
	nowtime = time(NULL); //获取日历时间  
	tm * ptm = localtime(&nowtime);

	std::fstream out;
	out.open("D:\\log\\hooklog.txt", std::ios::out | std::ios::in | std::ios::app);
	if (out.is_open() != true)
	{
		std::cout << "打开文件失败!" << std::endl;
		return;
	}

	const char *pxq[] = { "日","一","二","三","四","五","六" };
	out << ptm->tm_year + 1900 << "年" << ptm->tm_mon + 1 << "月" << ptm->tm_mday << "日 ";
	out << ptm->tm_hour << ":" << ptm->tm_min << ":" << ptm->tm_sec << " ";
	//out << " 星期" << pxq[ptm->tm_wday] << " 当年的第" << ptm->tm_yday << "天 " << "\r\n";
	out << msg << "\r\n";

	out.close();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (isHooked = HookApi_Script()) {
			//log("hook successed");
			//MessageBoxW(NULL, L"HOOK成功。", L"狗子", 0);
			std::cout << "HOOK 成功" << std::endl;
		}
		else {
			//log("hook failed");
			//MessageBoxW(NULL, L"HOOK失败。", L"狗子", 0);
			std::cout << "HOOK 失败" << std::endl;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}