#include "stdafx.h"
#include <Windows.h>
#include "resource1.h"
#include <stdio.h>
#include "malloc.h"

//消息结构体
struct WxSendText
{
	wchar_t * pStr;
	int strLen;
	int iStrLen;
	int fill;
	int fill2;
};

//获取模块基址
DWORD GetModuleWeChatWinAddress()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}


/*
5B8B2128  |.  8B55 CC       |mov edx,[local.13]                      ;  wxid 结构体
5B8B212B  |.  8D43 14       |lea eax,dword ptr ds:[ebx+0x14]
5B8B212E  |.  6A 01         |push 0x1                                ;  0x1
5B8B2130  |.  50            |push eax                                ;  0x0 如果是@那么需要一个结构体指针指向一个wxid
5B8B2131  |.  53            |push ebx                                ;  发送的消息内容结构体被@人wxid
5B8B2132  |.  8D8D E4F7FFFF |lea ecx,[local.519]                     ;  0x81C缓冲区
5B8B2138  |.  E8 A31F2100   |call WeChatWi.5BAC40E0
5B8B213D  |.  83C4 0C       |add esp,0xC
*/
//发送文本消息函数
VOID SendTextMessage(wchar_t * wxid, wchar_t * qunWxid, wchar_t * message)
{
	WxSendText pWxid = { 0 };
	pWxid.pStr			= wxid;
	pWxid.strLen		= wcslen(wxid);
	pWxid.iStrLen		= wcslen(wxid) * 2;

	WxSendText pQCYWxid = { 0 };
	pQCYWxid.pStr		= qunWxid;
	pQCYWxid.strLen		= wcslen(qunWxid);
	pQCYWxid.iStrLen	= wcslen(qunWxid) * 2;

	WxSendText pMsg		= { 0 };
	pMsg.pStr			= message;
	pMsg.strLen			= wcslen(message);
	pMsg.iStrLen		= wcslen(message) * 2;

	char* asmWxid		= (char*)&pWxid.pStr;
	char* asmQCYWxid	= (char*)&pQCYWxid.pStr;
	char* asmMsg		= (char*)&pMsg.pStr;
	//缓冲区
	char buf[0x81C]		= { 0 };	

	//call地址
	DWORD callAdd = GetModuleWeChatWinAddress() + 0x2DA0F0;
	__asm {
		mov edx, asmWxid
		mov eax, pQCYWxid
		push 0x1
		push eax
		mov ebx, asmMsg
		push ebx
		lea ecx, buf
		call callAdd
		add esp, 0xC
	}	
}
/*
0FFD4504    53              push ebx
//////////////////////////////////////////////////////////////////////////////			
0D42C4EC  11408DE8  UNICODE "C:\Users\chenn\AppData\Local\Temp\1599527257.png"
0D42C4F0  00000030
0D42C4F4  00000040
//////////////////////////////////////////////////////////////////////////////		
0FFD4505    FF75 CC         push dword ptr ss : [ebp - 0x34]
//////////////////////////////////////////////////////////////////////////////	
0D3C05E0  11ED0E08  UNICODE "filehelper"
0D3C05E4  0000000A
0D3C05E8  00000010
//////////////////////////////////////////////////////////////////////////////	
0FFD4508    8D85 A4FBFFFF   lea eax, dword ptr ss : [ebp - 0x45C]
0FFD450E    50              push eax
0FFD450F    51              push ecx
0FFD4510    E8 3B8DFBFF     call WeChatWi.0FF8D250					//第1个函数
0FFD4515    83C4 04         add esp, 0x4
0FFD4518    8BC8            mov ecx, eax
0FFD451A    E8 E1552100     call WeChatWi.101E9B00					//第2个函数
*/
VOID sendPicMessage(wchar_t * wxid, wchar_t * filepath)
{
	WxSendText pWxid = { 0 };
	pWxid.pStr = wxid;
	pWxid.strLen = wcslen(wxid);
	pWxid.iStrLen = wcslen(wxid) * 2;

	WxSendText pPic = { 0 };
	pPic.pStr = filepath;
	pPic.strLen = wcslen(filepath);
	pPic.iStrLen = wcslen(filepath) * 2;

	char buff[0x45C] = { 0 };

	char* asmPic = (char*)&pPic.pStr;
	char* asmWxid = (char*)&pWxid.pStr;

	DWORD callAdd1 = GetModuleWeChatWinAddress() + 0x7D250;
	DWORD callAdd2 = GetModuleWeChatWinAddress() + 0x2D9B00;
	DWORD myEsp = 0;

	//char testa[0x200] = { 0 };
	//sprintf_s(testa, "输入框地址:%p  写死地址:%p  缓冲区地址：%p wxid:%p path:%p", filepath, filepath, buff, asmWxid, asmPic);
	//MessageBoxA(NULL, testa, "发图片", 0);

	__asm {
		//保存esp
		mov myEsp, esp;

		mov ebx, asmPic
		push ebx
		push asmWxid
		lea eax, buff
		push eax
		push 0x0
		call callAdd1
		add esp, 0x4
		mov ecx, eax;
		call callAdd2;

		//恢复堆栈
		mov eax, myEsp;
		mov esp, eax;
	}
}

//修改好友备注
VOID editNick(wchar_t * wxid, wchar_t * nick)
{
	__asm {
		push 0x0

	}
}

