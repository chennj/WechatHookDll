#pragma once
#include "stdafx.h"
#include  <list>

#define HOOK_COMMAND_LEN 5

//������Ϣ������ƫ����
#define SEND_CALL_RL_ADDRESS 0x2DA2D7

//������Ϣ������ƫ����
#define WECHAT_SEND_MSG_CALL_RL_ADDRESS 0x2DA0F0
//#define WECHAT_SEND_MSG_CALL_RL_ADDRESS 0x2990320

//΢������Ϣ�Ļص�call
#define WECHAT_RECV_MSG_CALL_RL_ADDRESS 0x305753


//���ݻ�ַ
#define WXADDR_DATA_BASE *((PDWORD)(WECHAT_WIN_BASE+0x113227C))

//Ⱥ���������
#define WXADDR_ALL_GROUPS (WECHAT_WIN_BASE+0x678+0x64)

#pragma region �ṹ�岿��


//΢���û�
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

// ��ȡ�����б�
std::list<WxUser>GetFriendList();
void InitFriendListControl(HWND hwnd);
