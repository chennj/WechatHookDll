#pragma once

#include "stdafx.h"

DWORD GetModuleWeChatWinAddress();
extern HWND hWxZhuShouDlg;
BOOL IsWxVersionValid();	//检查当前微信版本