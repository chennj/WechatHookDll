// Dll_Injector.cpp : 定义控制台应用程序的入口点。
//
#if 0
#include <iostream>
#include <string>
#include <ctype.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
//Library needed by Linker to check file existance
#pragma comment(lib, "Shlwapi.lib")

#define BUFSIZE MAX_PATH      //缓冲区大小
using namespace std;

int		getProcID(const string& p_name);
bool	InjectDLL(const int &pid, const string &DLL_Path);
int		EnableDebugPriv(const char * name);
void	usage();


int main(int argc, char ** argv)
{

	if (argc != 2) {
		//usage();
		//return EXIT_FAILURE;
		char buffer[BUFSIZE] = { 0 };
		DWORD dwRet;
		dwRet = GetCurrentDirectory(BUFSIZE, buffer);

		if (dwRet == 0) //函数执行失败
		{
			cerr << "GetCurrentDirectory failed (" << GetLastError() << ")/n" << endl;
			return EXIT_FAILURE;
		}

		if (dwRet > BUFSIZE) //缓冲区空间不足
		{
			cerr << "Buffer too small, need " << dwRet << " characters/n" << endl;
			return EXIT_FAILURE;
		}
		argv[1] = lstrcat(buffer, "\\EasyHook.dll");
	}

	if (PathFileExistsA(argv[1]) == FALSE)
	{
		cerr << "[!]DLL " << argv[1] << " file does NOT exist!" << endl;
		return EXIT_FAILURE;
	}

	InjectDLL(getProcID("DemoRoom.exe"), argv[1]);

	return EXIT_SUCCESS;
}

int getProcID(const string& p_name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 structprocsnapshot = { 0 };

	structprocsnapshot.dwSize = sizeof(PROCESSENTRY32);

	if (snapshot == INVALID_HANDLE_VALUE)return 0;
	if (Process32First(snapshot, &structprocsnapshot) == FALSE)return 0;

	while (Process32Next(snapshot, &structprocsnapshot))
	{
		if (!strcmp(structprocsnapshot.szExeFile, p_name.c_str()))
		{
			CloseHandle(snapshot);
			cout << "[+]Process name is: " << p_name << "\n[+]Process ID: " << structprocsnapshot.th32ProcessID << endl;
			return structprocsnapshot.th32ProcessID;
		}
	}
	CloseHandle(snapshot);
	cerr << "[!]Unable to find Process ID" << endl;
	return 0;

}

bool InjectDLL(const int &pid, const string &DLL_Path)
{

	//提升进程权限为调试权限
	if (EnableDebugPriv(SE_DEBUG_NAME))
	{
		cerr << "[!]add privilege error" << endl;
		return false;
	}

	long dll_size = DLL_Path.length() + 1;
	HANDLE hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (hRemoteProcess == NULL)
	{
		cerr << "[!]Fail to open target process!" << endl;
		return false;
	}
	cout << "[+]Opening Target Process..." << endl;

	LPVOID MyAlloc = VirtualAllocEx(hRemoteProcess, NULL, dll_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (MyAlloc == NULL)
	{
		cerr << "[!]Fail to allocate memory in Target Process." << endl;
		return false;
	}

	cout << "[+]Allocating memory in Targer Process." << endl;
	int IsWriteOK = WriteProcessMemory(hRemoteProcess, MyAlloc, DLL_Path.c_str(), dll_size, 0);
	if (IsWriteOK == 0)
	{
		cerr << "[!]Fail to write in Target Process memory." << endl;
		return false;
	}
	cout << "[+]Creating Remote Thread in Target Process" << endl;

	DWORD dWord;
	LPTHREAD_START_ROUTINE addrLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32"), "LoadLibraryA");
	HANDLE ThreadReturn = CreateRemoteThread(hRemoteProcess, NULL, 0, addrLoadLibrary, MyAlloc, 0, &dWord);
	if (ThreadReturn == NULL)
	{
		cerr << "[!]Fail to create Remote Thread" << endl;
		return false;
	}

	if ((hRemoteProcess != NULL) && (MyAlloc != NULL) && (IsWriteOK != ERROR_INVALID_HANDLE) && (ThreadReturn != NULL))
	{
		cout << "[+]DLL Successfully Injected :)" << endl;
		return true;
	}

	return false;
}

int EnableDebugPriv(const char * name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		printf("OpenProcessToken error\n");
		return 1;
	}
	//获得进程本地唯一ID
	if (!LookupPrivilegeValue(NULL, name, &luid))
	{
		printf("LookupPrivilege error!\n");
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//调整进程权限
	if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		printf("AdjustTokenPrivileges error!\n");
		return 1;
	}
	return 0;
}

void usage()
{
	cout << "Usage: DLL_Injector.exe <DLL Path to Inject>" << endl;
}
#endif

#include <iostream>
#include <string>
#include <filesystem>
#include <Windows.h>
#include "RemoteInjectDLL.h"

#define DLL_NAME "EasyHook.dll"

std::string g_DllName = "";

std::string	GetModuleFilePath(const char* name)
{
	char tmppath[MAX_PATH];
	HMODULE h = GetModuleHandleA(name);

	if (0 != GetModuleFileNameA(h, tmppath, sizeof(tmppath)))
	{
		std::experimental::filesystem::path path = tmppath;

		if (std::experimental::filesystem::is_regular_file(path))
		{
			return path.parent_path().generic_string();
		}
	}
	return "";
}

int main()
{
	DWORD pid;
	std::string dllName = DLL_NAME;

	//提权
	std::cout << "提权:" << (AdjustSeltTokenPrivileges() ? "成功" : "失败") << std::endl;

	std::string pname("");
	std::cout << "输入目标进程名,例如 DemoRoom.exe" << std::endl;
	std::cin >> pname;
	pid = GetProcessID(pname);

	//取EXE目录
	std::string path = GetModuleFilePath(NULL) + "/" + dllName;
	std::cout << "注入 " << path << " 到进程 " << pid << std::endl;
	HMODULE h = RemoteInjectDLL(pid, path.c_str());
	std::cout << "模块句柄:" << h << std::endl;

	std::string cmd("");
	std::string exit("exit");
	std::cout << "输入 exit 退出并卸载DLL。" << std::endl;
	while (cmd.compare(exit) != 0) {
		std::cin >> cmd;
		std::cout << "输入的是：" << cmd << std::endl;
	}
	RemoteFreeDLL(pid, h);
	std::cout << "已退出，卸载DLL完毕" << std::endl;

	system("pause");
}