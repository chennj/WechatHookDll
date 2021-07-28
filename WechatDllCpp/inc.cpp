#include "stdafx.h"
#include <string>
#include <strstream>
#include <iostream>
#pragma comment(lib,"Version.lib")
#include "MyMessage.h"
#include "inc.h"
using namespace std;

//5个填充
struct WX_STR_5
{
	wchar_t* str;
	int len;
	int maxLen;
	int f1;
	int f2;
};

BOOL IsWxVersionValid()
{
	DWORD wxBaseAddress = GetModuleWeChatWinAddress();
	const string wxVersoin = "2.6.6.28";

	WCHAR VersionFilePath[MAX_PATH];
	if (GetModuleFileName((HMODULE)wxBaseAddress, VersionFilePath, MAX_PATH) == 0)
	{
		return FALSE;
	}

	string asVer = "";
	VS_FIXEDFILEINFO* pVsInfo;
	unsigned int iFileInfoSize = sizeof(VS_FIXEDFILEINFO);
	int iVerInfoSize = GetFileVersionInfoSize(VersionFilePath, NULL);
	if (iVerInfoSize != 0) {
		char* pBuf = new char[iVerInfoSize];
		if (GetFileVersionInfo(VersionFilePath, 0, iVerInfoSize, pBuf)) {
			if (VerQueryValue(pBuf, TEXT("\\"), (void**)& pVsInfo, &iFileInfoSize)) {
				//主版本2.6.7.57
				//2
				int s_major_ver = (pVsInfo->dwFileVersionMS >> 16) & 0x0000FFFF;
				//6
				int s_minor_ver = pVsInfo->dwFileVersionMS & 0x0000FFFF;
				//7
				int s_build_num = (pVsInfo->dwFileVersionLS >> 16) & 0x0000FFFF;
				//57
				int s_revision_num = pVsInfo->dwFileVersionLS & 0x0000FFFF;

				//把版本变成字符串
				strstream wxVer;
				wxVer << s_major_ver << "." << s_minor_ver << "." << s_build_num << "." << s_revision_num;
				wxVer >> asVer;
			}
		}
		delete[] pBuf;
	}

	//版本匹配
	if (asVer == wxVersoin)
	{
		return TRUE;
	}

	//版本不匹配
	return FALSE;
}

BOOL CheckWxIsOnline()
{

	DWORD dwIsOnline = GetModuleWeChatWinAddress() + 0x1131D3C;
	if (*(DWORD*)dwIsOnline == 0)	
	{
		return FALSE;
	}
	else {
		return TRUE;
	}
	//WX_LOGIN_USER_INFO info = WxGetCurrentUserInfo();
	//return 0 != wcscmp(info.wxid, L"");
}

//ll基址 52AE0000
//WeChatWin.dll + 1131DC8   昵称地址
//WeChatWin.dll + 1131F2C   头像地址
//WeChatWin.dll + 1131C98   手机号码
//WeChatWin.dll + 1131DC8   微信ID
//WeChatWin.dll + 1132030   机型
//WeChatWin.dll + 1131D68   地区
//读取内存数据
WX_LOGIN_USER_INFO WxGetCurrentUserInfo()
{
	DWORD WINDLL_DW_ADDRESS = GetModuleWeChatWinAddress(); //获取基地址
	DWORD wxidAddr = WINDLL_DW_ADDRESS + 0x1131B90;

	//wxid 微信id的地址
	DWORD wxidAddr_2 = WINDLL_DW_ADDRESS + 0x1131B78;

	//1131DC8 微信username地址
	DWORD wxnoAddr = WINDLL_DW_ADDRESS + 0x1131DC8;

	DWORD WX_AVATAR_ADRESS = WINDLL_DW_ADDRESS + 0x1131F2C;
	DWORD WX_PHONE_ADRESS = WINDLL_DW_ADDRESS + 0x1131C98;

	DWORD WX_NICKNAME_ADRESS = WINDLL_DW_ADDRESS + 0x1131C64;

	DWORD WX_ADD_ADRESS = WINDLL_DW_ADDRESS + 0x1131D68;
	DWORD WX_QRSTR_ADRESS = WINDLL_DW_ADDRESS + 0x114658C; //二维码
	DWORD wx_mobile_type_addr = WINDLL_DW_ADDRESS + 0x1102218; //设备
	WX_LOGIN_USER_INFO info = { 0 };

	info.wxid = UTF8ToUnicode((const char*)wxidAddr_2);
	if (wcslen(info.wxid) < 0x6) {
		DWORD pWxid = WINDLL_DW_ADDRESS + 0x1131B78;
		pWxid = *((DWORD *)pWxid);
		info.wxid = UTF8ToUnicode((const char *)pWxid);
	}
	info.account = UTF8ToUnicode((const char*)wxnoAddr);
	//info.nickname = UTF8ToUnicode();
	//auto nick = (const char *)WX_NICKNAME_ADRESS;
	/*auto len = strlen(nick);
	if (len == 4) {
	DWORD pna = WX_NICKNAME_ADRESS + 0x0;
	pna = *((DWORD *)pna);
	auto st = (const char *)pna;
	nick = st;
	}*/
	auto my = GetUserInfoByWxId(info.wxid);
	if (wcslen(my.nickname) > 0) {
		char* name = UnicodeToUTF8(my.nickname);
		info.nickname = UTF8ToUnicode(name);
	}
	else {
		info.nickname = UTF8ToUnicode("");
	}
	info.phone = UTF8ToUnicode((const char*)WX_PHONE_ADRESS);
	info.address = UTF8ToUnicode((const char*)WX_ADD_ADRESS);
	info.phone = UTF8ToUnicode((const char*)WX_PHONE_ADRESS);
	info.mobileType = UTF8ToUnicode((const char*)wx_mobile_type_addr);

	DWORD rl = *reinterpret_cast<DWORD *>(WX_AVATAR_ADRESS); //获取真实的地址
	info.avatar = UTF8ToUnicode((const char*)rl);

	DWORD qrd = *reinterpret_cast<DWORD *>(WX_QRSTR_ADRESS); //二维码的虚拟地址
	info.qrcode = UTF8ToUnicode((const char*)qrd);
	return info;
}

//转码
wchar_t* UTF8ToUnicode(const char* str)
{
	int textlen = 0;
	wchar_t* result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t *)malloc((textlen + 1) * sizeof(wchar_t));
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return result;
}

//转码utf8
char* UnicodeToUTF8(const WCHAR* str)
{
	int nByte;
	char* zFilename;
	nByte = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
	zFilename = static_cast<char*>(malloc(nByte));
	if (zFilename == 0)
	{
		return 0;
	}
	nByte = WideCharToMultiByte(CP_UTF8, 0, str, -1, zFilename, nByte, 0, 0);
	if (nByte == 0)
	{
		free(zFilename);
		zFilename = 0;
	}
	return zFilename;
}
char * UnicodeToANSI(const wchar_t *str)
{
	char * result;
	int textlen = 0;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc((textlen + 1) * sizeof(char));
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}

char* Tool_GbkToUtf8(const char* src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	char* strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

char* Tool_UTF8ToGBK(const char* src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	char* strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}


wchar_t* Tool_CharToWchar(const char* c, size_t m_encode)
{
	wchar_t* str;
	int len = MultiByteToWideChar(m_encode, 0, c, strlen(c), NULL, 0);
	wchar_t* m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(m_encode, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	str = m_wchar;
	delete m_wchar;
	return str;
}

char* Tool_WcharToChar(const wchar_t* wp, size_t m_encode)
{
	char* str;
	int len = WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	char* m_char = new char[len + 1];
	WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	str = m_char;
	delete m_char;
	return str;
}

ChatRoomUserInfo GetUserInfoByWxId(wchar_t* wid)
{
	ChatRoomUserInfo user = {};
	DWORD base_addr = GetModuleWeChatWinAddress();

	DWORD callAdd = base_addr + 0x266D30;
	DWORD callAdd2 = base_addr + 0x4BE50;
	DWORD callAdd3 = base_addr + 0x47AE60;


	WX_STR_5 pWxid = { 0 };
	pWxid.str = wid;
	pWxid.len = wcslen(wid);
	pWxid.maxLen = wcslen(wid) * 2;
	char* asm_wid = (char*)&pWxid.str;

	//buffer to save info
	char buff[0xBF8] = { 0 };
	char* asmBuff = buff;

	__asm {
		mov edi, asmBuff
		push edi
		sub esp, 0x14
		mov eax, asm_wid
		mov ecx, esp
		push eax
		call callAdd3
		call callAdd2
		call callAdd
	}

	/*8 wxid
	1C 账号
	30 v1
	50 备注
	64 昵称*/
	LPVOID textWxid = *((LPVOID *)((DWORD)buff + 0x8));
	LPVOID textUser = *((LPVOID *)((DWORD)buff + 0x1C));
	LPVOID textNick = *((LPVOID *)((DWORD)buff + 0x64));
	LPVOID textV1 = *((LPVOID *)((DWORD)buff + 0x30));

	swprintf_s(user.wxid, L"%s", textWxid);
	swprintf_s(user.username, L"%s", textUser);
	swprintf_s(user.nickname, L"%s", textNick);
	swprintf_s(user.v1, L"%s", textV1);
	//test 检查是否是空,如果是空字符串就返回一个
	if (0 == wcscmp(L"(null)", user.wxid))
	{
		swprintf_s(user.wxid, L"");
		swprintf_s(user.username, L"");
		swprintf_s(user.nickname, L"");
		swprintf_s(user.v1, L"");
		user.success = false;
	}
	else
	{
		user.success = true;
	}
	//赋值
	return user;
}
