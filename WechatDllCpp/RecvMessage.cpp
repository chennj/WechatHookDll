#include "stdafx.h"
#include <Windows.h>
#include "resource1.h"
#include <string>
#include <stdio.h>
#include "malloc.h"
#include "inc.h"
#include <atlbase.h>
#include <atlconv.h>

#define ReciveMessageParam 0x1131AF8		//������Ϣ�Ĳ���ƫ��

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
	//��Ϣ��λ��
	DWORD** msgAddress = (DWORD * *)RECV_ESP;

	//��Ϣ����[[esp]]+0x30
	//[01����] [03ͼƬ] [31ת��XML��Ϣ] [22������Ϣ] [02B��Ƶ��Ϣ]
	//��л��.혆�_�ԇY����unravel�ṩ������Ϣ��
	DWORD msgType = *((DWORD*)(**msgAddress + 0x30));
	receivedMessage.append(L"��Ϣ����:");
	switch (msgType)
	{
	case 0x01:
		receivedMessage.append(L"���� ");
		break;
	case 0x03:
		receivedMessage.append(L"ͼƬ ");
		break;

	case 0x22:
		receivedMessage.append(L"���� ");
		break;
	case 0x25:
		receivedMessage.append(L"����ȷ�� ");
		break;
	case 0x28:
		receivedMessage.append(L"POSSIBLEFRIEND_MSG ");
		break;
	case 0x2A:
		receivedMessage.append(L"��Ƭ ");
		break;
	case 0x2B:
		receivedMessage.append(L"��Ƶ ");
		break;
	case 0x2F:
		//ʯͷ������
		receivedMessage.append(L"���� ");
		break;
	case 0x30:
		receivedMessage.append(L"λ�� ");
		break;
	case 0x31:
		//����ʵʱλ��
		//�ļ�
		//ת��
		//����
		receivedMessage.append(L"����ʵʱλ�á��ļ���ת�ˡ����� ");
		break;
	case 0x32:
		receivedMessage.append(L"VOIPMSG ");
		break;
	case 0x33:
		receivedMessage.append(L"΢�ų�ʼ�� ");
		break;
	case 0x34:
		receivedMessage.append(L"VOIPNOTIFY ");
		break;
	case 0x35:
		receivedMessage.append(L"VOIPINVITE ");
		break;
	case 0x3E:
		receivedMessage.append(L"С��Ƶ ");
		break;
	case 0x270F:
		receivedMessage.append(L"SYSNOTICE ");
		break;
	case 0x2710:
		//ϵͳ��Ϣ
		//���
		receivedMessage.append(L"�����ϵͳ��Ϣ ");
		break;
	case 0x2712:
		receivedMessage.append(L"������Ϣ ");
		break;
	default:
		break;
	}
	receivedMessage.append(L"\r\n");

	//dc [[[esp]] + 0x114]
	//�ж���Ⱥ��Ϣ���Ǻ�����Ϣ
	//�����Ϣ
	std::wstring msgSource2 = L"<msgsource />\n";
	std::wstring msgSource = L"";
	msgSource.append(GetMsgByAddress(**msgAddress + 0x168));

	if (msgSource.length() <= msgSource2.length())
	{
		receivedMessage.append(L"�յ�������Ϣ:\r\n");
		isFriendMsg = TRUE;
	}
	else
	{
		receivedMessage.append(L"�յ�Ⱥ��Ϣ:\r\n");
		isFriendMsg = FALSE;
	}

	//������Ϣ
	if (isFriendMsg == TRUE)
	{
		receivedMessage.append(L"����wxid��\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x40))
			.append(L"\r\n\r\n");
	}
	else
	{
		receivedMessage.append(L"Ⱥ�ţ�\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x40))
			.append(L"\r\n\r\n");

		receivedMessage.append(L"��Ϣ�����ߣ�\r\n")
			.append(GetMsgByAddress(**msgAddress + 0x114))
			.append(L"\r\n\r\n");

		receivedMessage.append(L"�����Ϣ��\r\n");
		receivedMessage += msgSource;
		receivedMessage.append(L"\r\n\r\n");
	}

	receivedMessage.append(L"��Ϣ���ݣ�\r\n")
		.append(GetMsgByAddress(**msgAddress + 0x68))
		.append(L"\r\n\r\n");

	if (NULL == hWxZhuShouDlg) {
		hWxZhuShouDlg = ::FindWindow(NULL, L"�ҵ�΢�����֣�beta��");
	}

	if (NULL == hWxZhuShouDlg) {
		::MessageBox(NULL, L"δ���ҵ�΢�����ִ���", L"������ʾ", 0);
		return;
	}

	//�ı��������Ϣ
	USES_CONVERSION;
	SetWindowTextA(GetDlgItem(hWxZhuShouDlg, EDT_RECVMXMSG_INPUT), W2A(receivedMessage.c_str()));
	//::MessageBoxA(NULL, W2A(receivedMessage.c_str()), "��ʾ", 0);
}


/*����һ���㺯��*/
void _declspec(naked)recvWxMsg() {

	__asm
	{
		mov ecx, dwParam
		//��ȡesp�Ĵ������ݣ�����һ��������
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
	DWORD hookedAddr = GetModuleWeChatWinAddress() + 0x305758 - 5;	//�൱�ڡ� mov ecx, WeChatWi.10921AF8���ĵ�ַ��
	RETURN_AFTER_HOOK = hookedAddr + 5;

	DWORD flOldProtect = 0;

	VirtualProtect((PVOID)hookedAddr, 5, PAGE_EXECUTE_READWRITE, &flOldProtect);	//�ı�HOOK���ڴ�����
	*(PBYTE)hookedAddr = 0xE9;														//jmpԶ��ת�Ļ���ָ��Ĳ�����
	*(DWORD*)((DWORD)hookedAddr + 1) = (DWORD)recvWxMsg - (DWORD)hookedAddr - 5;	//jmpԶ��ת��ƫ�Ƶ�ַ																					
	VirtualProtect((LPVOID)hookedAddr, 5, flOldProtect, &flOldProtect);				//�ָ�HOOK���ڴ�����

}
