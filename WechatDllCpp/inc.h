#pragma once
#include <string>

DWORD GetModuleWeChatWinAddress();
std::wstring GetMsgByAddress(DWORD memAddress);
extern HWND hWxZhuShouDlg;
BOOL IsWxVersionValid();	//��鵱ǰ΢�Ű汾