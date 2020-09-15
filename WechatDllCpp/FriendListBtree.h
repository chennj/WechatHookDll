#pragma once
#include "stdafx.h"
#include  <list>

#define HOOK_COMMAND_LEN 5

//发送消息函数的偏移量
#define SEND_CALL_RL_ADDRESS 0x2DA2D7

//发送消息函数的偏移量
#define WECHAT_SEND_MSG_CALL_RL_ADDRESS 0x2DA0F0
//#define WECHAT_SEND_MSG_CALL_RL_ADDRESS 0x2990320

//微信收消息的回调call
#define WECHAT_RECV_MSG_CALL_RL_ADDRESS 0x305753


//数据基址
#define WXADDR_DATA_BASE *((PDWORD)(WECHAT_WIN_BASE+0x113227C))

//群组链表入口
#define WXADDR_ALL_GROUPS (WECHAT_WIN_BASE+0x678+0x64)

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
