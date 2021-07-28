// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "WechatDllCpp.h"
#include <stdio.h>
#include "resource1.h"
#include <Windows.h>
#include "httpServer.h"
#include "RecvMessage.h"
#include "FriendListBTree.h"
#include "inc.h"

INT_PTR CALLBACK Dlgproc(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);

DWORD ThreadProc(HMODULE hModule);
DWORD ThreadProc_http();
DWORD ThreadProc_redis();

int m_radio = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		//DialogBox(hModule, MAKEINTRESOURCE(DLG_MAIN), NULL, &Dlgproc);直接弹框会卡住主线程，应该放在线程中。
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
	case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK Dlgproc(
	HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	wchar_t wxidOrQun[0x100]	= { 0 };
	wchar_t qunWxid[0x100]		= { 0 };
	wchar_t message[0x300]		= { 0 };

	switch (msg)
	{
	case WM_INITDIALOG:
		::CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);//程序启动时默认为单选按钮1选中
		InitFriendListControl(hwnd);
		break;
	case WM_CLOSE:
		//EndDialog(hwnd, 0);
		MessageBoxA(hwnd, "微信关闭自动关闭", "提示", 0);
		break;
	case WM_COMMAND:
		if (wParam == BTN_SEND)
		{
			GetDlgItemText(hwnd, EDT_WXID, wxidOrQun, sizeof(wxidOrQun));
			GetDlgItemText(hwnd, EDT_WXSENDMSG_INPUT, message, sizeof(message));
			GetDlgItemText(hwnd, EDT_QCY_WXID, qunWxid, sizeof(qunWxid));
			if (m_radio == 0) { 
				SendTextMessage(wxidOrQun, qunWxid, message); 
			}
			if (m_radio == 1) {
				sendPicMessage(wxidOrQun, message);
			}
		}

		if (wParam == IDC_RADIO1)
		{
			::CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
			m_radio = 0;
			//MessageBoxA(hwnd, "文本", "提示", 0);
		}

		if (wParam == IDC_RADIO3) 
		{
			::CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO3);
			m_radio = 1;
			//MessageBoxA(hwnd, "图片", "提示", 0);
		}

		if (wParam == IDC_BUTTON_HOOK) {
			GetFriendList();
		}

		if (wParam == IDC_BUTTON_ISLOGIN) {
			if (CheckWxIsOnline()) {
				SetWindowText(GetDlgItem(hwnd, IDC_STATIC_ISLOGIN), L"在线");
			}
			else {
				SetWindowText(GetDlgItem(hwnd, IDC_STATIC_ISLOGIN), L"离线");
			}
		}
		break;
	}

	return false;
}

DWORD ThreadProc(HMODULE hModule)
{
	if (!IsWxVersionValid()) {
		MessageBoxA(NULL, "当前微信版本不匹配，请下载WeChat2.6.6.28", "错误", MB_OK);
		return 0;
	}
	hookReplace();
	DialogBox(hModule, MAKEINTRESOURCE(DLG_MAIN), NULL, &Dlgproc);
	return TRUE;
}

DWORD ThreadProc_redis()
{
	return 0;
}

DWORD ThreadProc_http()
{
	httpServer();
	return TRUE;
}

