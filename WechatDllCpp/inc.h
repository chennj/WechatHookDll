#pragma once
#include <string>

DWORD GetModuleWeChatWinAddress();
std::wstring GetMsgByAddress(DWORD memAddress);
extern HWND hWxZhuShouDlg;
BOOL IsWxVersionValid();	//检查当前微信版本