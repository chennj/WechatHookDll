
#include "MyHook.h"

BYTE g_pOldData[12] = { 0 };
bool isHooked = false;

int NewScript(const WCHAR* s, BOOL quietErrors = FALSE, void* fpv = NULL) {

	MessageBoxW(NULL, s, L"�ٳֵ��Ĳ�������", MB_ICONINFORMATION);
	return 888;
}

// Hook Api
BOOL HookApi_Script()
{
	// ��ȡ user32.dll ģ����ػ�ַ
	HMODULE hDll = ::GetModuleHandle("CommonDll.dll");
	if (NULL == hDll)
	{
		MessageBoxW(NULL, L"�Ҳ���ģ���ַ��", L"����", 0);
		return FALSE;
	}
	// ��ȡ exprot_test_str �����ĵ�����ַ
	ExecuteMAXScriptScript OldScript = (ExecuteMAXScriptScript)::GetProcAddress(hDll, "exprot_test_str");
	if (NULL == OldScript)
	{
		MessageBoxW(NULL, L"�Ҳ���������ַ��", L"����", 0);
		return FALSE;
	}
	// ����д���ǰ���ֽ�����, 32λ��5�ֽ�, 64λ��12�ֽ�
#ifndef _WIN64
	// 32λ
	// �����룺jmp _dwNewAddress
	// ������λ��e9 _dwOffset(��תƫ��)
	//        addr1 --> jmp _dwNewAddressָ�����һ��ָ��ĵ�ַ����eip��ֵ
	//        addr2 --> ��ת��ַ��ֵ����_dwNewAddress��ֵ
	//        ��תƫ�� _dwOffset = addr2 - addr1
	BYTE pNewData[5] = { 0xe9, 0, 0, 0, 0 };
	DWORD dwNewDataSize = 5;
	DWORD dwOffset = 0;
	// ������תƫ��
	dwOffset = (DWORD)NewScript - ((DWORD)OldScript + 5);
	::RtlCopyMemory(&pNewData[1], &dwOffset, sizeof(dwOffset));
#else
	// 64λ
	// �����룺mov rax, _dwNewAddress(0x1122334455667788)
	//         jmp rax
	// �������ǣ�
	//    48 b8 _dwNewAddress(0x1122334455667788)
	//    ff e0
	BYTE pNewData[12] = { 0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xe0 };
	DWORD dwNewDataSize = 12;
	ULONGLONG ullNewFuncAddr = (ULONGLONG)NewScript;
	::RtlCopyMemory(&pNewData[2], &ullNewFuncAddr, sizeof(ullNewFuncAddr));
#endif
	// ����ҳ��ı�������Ϊ �ɶ�����д����ִ��
	DWORD dwOldProtect = 0;
	::VirtualProtect(OldScript, dwNewDataSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// ����ԭʼ����
	::RtlCopyMemory(g_pOldData, OldScript, dwNewDataSize);
	// ��ʼ�޸� MessageBoxA ������ǰ���ֽ�����, ʵ�� Inline Hook API
	::RtlCopyMemory(OldScript, pNewData, dwNewDataSize);
	// ��ԭҳ�汣������
	::VirtualProtect(OldScript, dwNewDataSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}

// Unhook Api
BOOL UnhookApi_Script()
{
	// ��ȡ user32.dll ģ����ػ�ַ
	HMODULE hDll = ::GetModuleHandle("user32.dll");
	if (NULL == hDll)
	{
		return FALSE;
	}
	// ��ȡ MessageBoxA �����ĵ�����ַ
	PVOID OldScript = ::GetProcAddress(hDll, "exprot_test_str");
	if (NULL == OldScript)
	{
		return FALSE;
	}
	// ����д���ǰ���ֽ�����, 32λ��5�ֽ�, 64λ��12�ֽ�
#ifndef _WIN64
	DWORD dwNewDataSize = 5;
#else
	DWORD dwNewDataSize = 12;
#endif
	// ����ҳ��ı�������Ϊ �ɶ�����д����ִ��
	DWORD dwOldProtect = 0;
	::VirtualProtect(OldScript, dwNewDataSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// �ָ�����
	::RtlCopyMemory(OldScript, g_pOldData, dwNewDataSize);
	// ��ԭҳ�汣������
	::VirtualProtect(OldScript, dwNewDataSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}