#pragma once
#include "stdafx.h"
#include  <list>

#pragma region 结构体部分


//微信用户
struct WxUser
{
	wchar_t wxid[0x100] = { 0 };
	wchar_t username[0x100] = { 0 };
	wchar_t nickname[0x100] = { 0 };
	wchar_t avatar[0x200]{};
	wchar_t account[0x200]{};
	wchar_t remark[0x200]{};
	wchar_t addType[0x200]{};
	bool success = false;
	int type = 1;
};


#pragma endregion

// 获取好友列表
std::list<WxUser>GetFriendList();
void InitFriendListControl(HWND hwnd);
