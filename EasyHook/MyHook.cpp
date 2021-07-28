
#include "MyHook.h"

BYTE g_pOldData[12] = { 0 };
bool isHooked = false;

int NewScript(const WCHAR* s, BOOL quietErrors = FALSE, void* fpv = NULL) {

	MessageBoxW(NULL, s, L"劫持到的参数内容", MB_ICONINFORMATION);
	return 888;
}

// Hook Api
BOOL HookApi_Script()
{
	// 获取 user32.dll 模块加载基址
	HMODULE hDll = ::GetModuleHandle("CommonDll.dll");
	if (NULL == hDll)
	{
		MessageBoxW(NULL, L"找不到模块基址。", L"狗子", 0);
		return FALSE;
	}
	// 获取 exprot_test_str 函数的导出地址
	ExecuteMAXScriptScript OldScript = (ExecuteMAXScriptScript)::GetProcAddress(hDll, "exprot_test_str");
	if (NULL == OldScript)
	{
		MessageBoxW(NULL, L"找不到导出地址。", L"狗子", 0);
		return FALSE;
	}
	// 计算写入的前几字节数据, 32位下5字节, 64位下12字节
#ifndef _WIN64
	// 32位
	// 汇编代码：jmp _dwNewAddress
	// 机器码位：e9 _dwOffset(跳转偏移)
	//        addr1 --> jmp _dwNewAddress指令的下一条指令的地址，即eip的值
	//        addr2 --> 跳转地址的值，即_dwNewAddress的值
	//        跳转偏移 _dwOffset = addr2 - addr1
	BYTE pNewData[5] = { 0xe9, 0, 0, 0, 0 };
	DWORD dwNewDataSize = 5;
	DWORD dwOffset = 0;
	// 计算跳转偏移
	dwOffset = (DWORD)NewScript - ((DWORD)OldScript + 5);
	::RtlCopyMemory(&pNewData[1], &dwOffset, sizeof(dwOffset));
#else
	// 64位
	// 汇编代码：mov rax, _dwNewAddress(0x1122334455667788)
	//         jmp rax
	// 机器码是：
	//    48 b8 _dwNewAddress(0x1122334455667788)
	//    ff e0
	BYTE pNewData[12] = { 0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xe0 };
	DWORD dwNewDataSize = 12;
	ULONGLONG ullNewFuncAddr = (ULONGLONG)NewScript;
	::RtlCopyMemory(&pNewData[2], &ullNewFuncAddr, sizeof(ullNewFuncAddr));
#endif
	// 设置页面的保护属性为 可读、可写、可执行
	DWORD dwOldProtect = 0;
	::VirtualProtect(OldScript, dwNewDataSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// 保存原始数据
	::RtlCopyMemory(g_pOldData, OldScript, dwNewDataSize);
	// 开始修改 MessageBoxA 函数的前几字节数据, 实现 Inline Hook API
	::RtlCopyMemory(OldScript, pNewData, dwNewDataSize);
	// 还原页面保护属性
	::VirtualProtect(OldScript, dwNewDataSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}

// Unhook Api
BOOL UnhookApi_Script()
{
	// 获取 user32.dll 模块加载基址
	HMODULE hDll = ::GetModuleHandle("user32.dll");
	if (NULL == hDll)
	{
		return FALSE;
	}
	// 获取 MessageBoxA 函数的导出地址
	PVOID OldScript = ::GetProcAddress(hDll, "exprot_test_str");
	if (NULL == OldScript)
	{
		return FALSE;
	}
	// 计算写入的前几字节数据, 32位下5字节, 64位下12字节
#ifndef _WIN64
	DWORD dwNewDataSize = 5;
#else
	DWORD dwNewDataSize = 12;
#endif
	// 设置页面的保护属性为 可读、可写、可执行
	DWORD dwOldProtect = 0;
	::VirtualProtect(OldScript, dwNewDataSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// 恢复数据
	::RtlCopyMemory(OldScript, g_pOldData, dwNewDataSize);
	// 还原页面保护属性
	::VirtualProtect(OldScript, dwNewDataSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}