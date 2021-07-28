#pragma once
#include <string>

struct WX_LOGIN_USER_INFO
{
	wchar_t * wxid;
	wchar_t * account;
	wchar_t * avatar;
	wchar_t * nickname;
	wchar_t * phone;
	wchar_t * address;
	wchar_t * mobileType;
	wchar_t * qrcode;
};

struct ChatRoomUserInfo
{
	wchar_t wxid[0x100];
	wchar_t username[0x100];
	wchar_t nickname[0x100];
	wchar_t v1[0x100];
	//�Ƿ��ȡ�ɹ�
	bool success = FALSE;
};

DWORD GetModuleWeChatWinAddress();
std::wstring GetMsgByAddress(DWORD memAddress);
extern HWND hWxZhuShouDlg;
BOOL IsWxVersionValid();	//��鵱ǰ΢�Ű汾
BOOL CheckWxIsOnline();
ChatRoomUserInfo GetUserInfoByWxId(wchar_t* wid);
WX_LOGIN_USER_INFO WxGetCurrentUserInfo();

/*����*/
wchar_t* UTF8ToUnicode(const char* str);
char* UnicodeToUTF8(const WCHAR* str);

//utf8 תGBK
char* Tool_GbkToUtf8(const char* src_str);
char* Tool_UTF8ToGBK(const char* src_str);

wchar_t* Tool_CharToWchar(const char* c, size_t m_encode);
//CP_ACP
char* Tool_WcharToChar(const wchar_t* wp, size_t m_encode);
char * UnicodeToANSI(const wchar_t *str);