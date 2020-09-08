#include "stdafx.h"
#include <Windows.h>
#include "resource1.h"
#include <string>
#include <stdio.h>
#include "malloc.h"
#include "inc.h"
#include <atlbase.h>
#include <atlconv.h>

#define ReciveMessageParam 0x1131AF8		//接收消息的参数偏移

DWORD RETURN_AFTER_HOOK;
DWORD RECV_ESP;

DWORD dwParam = GetModuleWeChatWinAddress() + ReciveMessageParam;

HWND hWxZhuShouDlg;

struct WxRecvMessage {

};

std::wstring GetMsgByAddress(DWORD memAddress)
{
	std::wstring tmp;
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength > 0) {
		WCHAR* msg = new WCHAR[msgLength + 1]{ 0 };
		wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
		tmp = msg;
		delete[]msg;
	}
	return  tmp;
}

void sendRecvWxMsg() {

	std::wstring receivedMessage = L"";
	BOOL isFriendMsg = FALSE;
	//[[esp]]
	//信息块位置
	DWORD** msgAddress = (DWORD * *)RECV_ESP;

	//消息类型[[esp]]+0x30
	//[01文字] [03图片] [31转账XML信息] [22语音消息] [02B视频信息]
	//感谢：.順唭_自嘫ɑ、unravel提供类型消息。
	DWORD msgType = *((DWORD*)(**msgAddress + 0x30));
	receivedMessage.append(L"消息类型:");
	switch (msgType)
	{
	case 0x01:
		receivedMessage.append(L"文字 ");
		break;
	case 0x03:
		receivedMessage.append(L"图片 ");
		break;

	case 0x22:
		receivedMessage.append(L"语音 ");
		break;
	case 0x25:
		receivedMessage.append(L"好友确认 ");
		break;
	case 0x28:
		receivedMessage.append(L"POSSIBLEFRIEND_MSG ");
		break;
	case 0x2A:
		receivedMessage.append(L"名片 ");
		break;
	case 0x2B:
		receivedMessage.append(L"视频 ");
		break;
	case 0x2F:
		//石头剪刀布
		receivedMessage.append(L"表情 ");
		break;
	case 0x30:
		receivedMessage.append(L"位置 ");
		break;
	case 0x31:
		//共享实时位置
		//文件
		//转账
		//链接
		receivedMessage.append(L"共享实时位置、文件、转账、链接 ");
		break;
	case 0x32:
		receivedMessage.append(L"VOIPMSG ");
		break;
	case 0x33:
		receivedMessage.append(L"微信初始化 ");
		break;
	case 0x34:
		receivedMessage.append(L"VOIPNOTIFY ");
		break;
	case 0x35:
		receivedMessage.append(L"VOIPINVITE ");
		break;
	case 0x3E:
		receivedMessage.append(L"小视频 ");
		break;
	case 0x270F:
		receivedMessage.append(L"SYSNOTICE ");
		break;
	case 0x2710:
		//系统消息
		//红包
		receivedMessage.append(L"红包、系统消息 ");
		break;
	case 0x2712:
		receivedMessage.append(L"撤回消息 ");
		break;
	default:
		break;
	}
	receivedMessage.append(L"\r\n");

	//dc [[[esp]] + 0x114]
	//判断是群消息还是好友消息
	//相关信息
	std::wstring msgSource2 = L"<msgsource />\n";
	std::wstring msgSource = L"";
	msgSource.append(GetMsgByAddress(**msgAddress + 0x168));

	if (msgSource.length() <= msgSource2.length())
	{
		receivedMessage.append(L"收到好友消息:\r\n");
		isFriendMsg = TRUE;
	}
	else
	{
		receivedMessage.append(L"收到群消息:\r\n");
		isFriendMsg = FALSE;
	}

	//好友消息
	if (isFriendMsg == TRUE)
	{
		receivedMessage.append(L"好友wxid：\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x40))
			.append(L"\r\n\r\n");
	}
	else
	{
		receivedMessage.append(L"群号：\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x40))
			.append(L"\r\n\r\n");

		receivedMessage.append(L"消息发送者：\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x114))
			.append(L"\r\n\r\n");

		receivedMessage.append(L"相关信息：\r\n");
		receivedMessage += msgSource;
		receivedMessage.append(L"\r\n\r\n");
	}

	receivedMessage.append(L"消息内容：\r\n")
		.append(GetMsgByAddress(**msgAddress + 0x68))
		.append(L"\r\n\r\n");

	if (NULL == hWxZhuShouDlg) {
		hWxZhuShouDlg = ::FindWindow(NULL, L"我的微信助手（beta）");
	}

	if (NULL == hWxZhuShouDlg) {
		::MessageBox(NULL, L"未查找到微信助手窗口", L"错误提示", 0);
		return;
	}

	//文本框输出信息
	USES_CONVERSION;
	SetWindowTextA(GetDlgItem(hWxZhuShouDlg, EDT_RECVMXMSG_INPUT), W2A(receivedMessage.c_str()));
	//::MessageBoxA(NULL, W2A(receivedMessage.c_str()), "提示", 0);
}


/*申明一个裸函数*/
void _declspec(naked)recvWxMsg() {

	__asm
	{
		mov ecx, dwParam
		//提取esp寄存器内容，放在一个变量中
		mov RECV_ESP, esp
		pushad
		pushf
	}
	sendRecvWxMsg();
	RETURN_AFTER_HOOK = GetModuleWeChatWinAddress() + 0x305758;
	__asm
	{
		popf
		popad
		jmp RETURN_AFTER_HOOK
	}
}

/*
0FAF5734    C700 00000000   mov dword ptr ds : [eax], 0x0
0FAF573A    C740 04 0000000>mov dword ptr ds : [eax + 0x4], 0x0
0FAF5741    C740 08 0000000>mov dword ptr ds : [eax + 0x8], 0x0
0FAF5748    A3 8C669310     mov dword ptr ds : [0x1093668C], eax
0FAF574D    50              push eax
0FAF574E    A1 F81A9210     mov eax, dword ptr ds : [0x10921AF8]
0FAF5753    B9 F81A9210     mov ecx, WeChatWi.10921AF8
0FAF5758    FF50 08         call dword ptr ds : [eax + 0x8]
0FAF575B    8B1D B0669310   mov ebx, dword ptr ds : [0x109366B0]
*/

void hookReplace() {
	DWORD baseAddr = GetModuleWeChatWinAddress();
	if (NULL == baseAddr) {
		MessageBox(NULL, L"baseAddr is null", L"hook recvMsg", 0);
		return;
	}
	DWORD hookedAddr = GetModuleWeChatWinAddress() + 0x305758 - 5;	//相当于“ mov ecx, WeChatWi.10921AF8”的地址。
	RETURN_AFTER_HOOK = hookedAddr + 5;

	DWORD flOldProtect = 0;

	VirtualProtect((PVOID)hookedAddr, 5, PAGE_EXECUTE_READWRITE, &flOldProtect);	//改变HOOK的内存属性
	*(PBYTE)hookedAddr = 0xE9;														//jmp远跳转的机器指令的操作码
	*(DWORD*)((DWORD)hookedAddr + 1) = (DWORD)recvWxMsg - (DWORD)hookedAddr - 5;	//jmp远跳转的偏移地址																					
	VirtualProtect((LPVOID)hookedAddr, 5, flOldProtect, &flOldProtect);				//恢复HOOK的内存属性

}
