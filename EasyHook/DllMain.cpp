#include <Windows.h>

#include "MyHook.h"

#include <fstream>
#include <time.h>  

/* ����32λ�µ�hook
typedef int(*ExecuteMAXScriptScript)(const WCHAR* s, BOOL quietErrors, void* fpv);
ExecuteMAXScriptScript OldFunc;	//ָ��ԭ������ָ��
FARPROC pfOld;					//ָ������Զָ��

//�����Ƕ��Ƶ�����ָ��Ĵ�С��, ������5��5���ֽ���
BYTE NewCode[5];
// ��ԭAPI�����ǰ5���ֽڴ��뱣�浽OldCode[]
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
		std::cout << "����DLLʧ�ܣ��˳���" << std::endl;
		MessageBoxW(NULL, L"����DLLʧ�ܣ��˳���", L"hook send", 0);
		return false;
	}

	OldFunc = (ExecuteMAXScriptScript)::GetProcAddress(hmod, "exprot_test_str");
	pfOld = (FARPROC)OldFunc;

	_asm
	{
		lea edi, OldCode					//��ȡOldCode����ĵ�ַ,�ŵ�edi
		mov esi, pfOld						//��ȡԭAPI��ڵ�ַ���ŵ�esi
		cld									//�����־λ��Ϊ��������ָ����׼��
		movsd								//����ԭAPI���ǰ4���ֽڵ�OldCode����
		movsb								//����ԭAPI��ڵ�5���ֽڵ�OldCode����
	}

	// int nAddr= UserFunAddr �C SysFunAddr - �����Ƕ��Ƶ�����ָ��Ĵ�С��;
	// Jmp nAddr;
	_asm
	{
		lea eax, MyExecuteMAXScriptScript	//��ȡ���ǵ�MyMessageBoxW������ַ
		mov ebx, pfOld						//ԭϵͳAPI������ַ
		sub eax, ebx						//int nAddr= UserFunAddr �C SysFunAddr
		sub eax, 5							//nAddr=nAddr-5
		mov dword ptr[NewCode + 1], eax		//������ĵ�ַnAddr���浽NewCode����4���ֽ�
											//ע��һ��������ַռ4���ֽ�
	}

	return true;
}

void HookOn()
{
	assert(hProcess != NULL);

	DWORD dwTemp = 0;
	DWORD dwOldProtect;

	//�޸�API�������ǰ5���ֽ�Ϊjmp xxxxxx
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfOld, NewCode, HookCommandLen, 0);
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, dwOldProtect, &dwTemp);

}

void HookOff()
{
	assert(hProcess != NULL);

	DWORD dwTemp = 0;
	DWORD dwOldProtect;

	//�ָ�API�������ǰ5���ֽ�
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(hProcess, pfOld, OldCode, HookCommandLen, 0);
	VirtualProtectEx(hProcess, pfOld, HookCommandLen, dwOldProtect, &dwTemp);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		MessageBoxW(NULL, L"ע��ɹ���", L"����", 0);

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
	nowtime = time(NULL); //��ȡ����ʱ��  
	tm * ptm = localtime(&nowtime);

	std::fstream out;
	out.open("D:\\log\\hooklog.txt", std::ios::out | std::ios::in | std::ios::app);
	if (out.is_open() != true)
	{
		std::cout << "���ļ�ʧ��!" << std::endl;
		return;
	}

	const char *pxq[] = { "��","һ","��","��","��","��","��" };
	out << ptm->tm_year + 1900 << "��" << ptm->tm_mon + 1 << "��" << ptm->tm_mday << "�� ";
	out << ptm->tm_hour << ":" << ptm->tm_min << ":" << ptm->tm_sec << " ";
	//out << " ����" << pxq[ptm->tm_wday] << " ����ĵ�" << ptm->tm_yday << "�� " << "\r\n";
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
			//MessageBoxW(NULL, L"HOOK�ɹ���", L"����", 0);
			std::cout << "HOOK �ɹ�" << std::endl;
		}
		else {
			//log("hook failed");
			//MessageBoxW(NULL, L"HOOKʧ�ܡ�", L"����", 0);
			std::cout << "HOOK ʧ��" << std::endl;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}