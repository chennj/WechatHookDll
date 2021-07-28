#include "stdafx.h"
#include "resource1.h"
#include "malloc.h"
#include "inc.h"
#include "FriendListBtree.h"
#include <tchar.h>
#include  <list>
#include <commctrl.h>
#include <exception>  

HWND hwnd_list;

typedef void(__stdcall *PLFUN)(void);

typedef std::list<DWORD> DWORD_LIST;
typedef std::list<std::wstring> WSTRING_LIST;

typedef struct _RAW_WX_NODE
{
	union
	{
		DWORD dwStart;
		PDWORD pStartNode;
	} ul;

	DWORD dwTotal;
} RAW_WX_NODE, *RAW_WXNODE;

void InitFriendListControl(HWND hwnd) {

	//InitCommonControls();

	hwnd_list = GetDlgItem(hwnd, IDC_LIST_FRIEND);

	if (NULL == hwnd_list) {
		MessageBox(NULL, L"list is null", L"错误提示", 0);
		return;
	}

	LVCOLUMN lv_col = { 0 };

	LPCWSTR title[] = { L"wxid", L"微信账号", L"昵称" };
	lv_col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lv_col.fmt = LVCFMT_LEFT;
	lv_col.cx = 140;

	for (int i = 0; i < 3; i++) {
		lv_col.pszText = (LPWSTR)title[i];
		ListView_InsertColumn(hwnd_list, i, &lv_col);
	}
}

void TraversalWxUserNode(PDWORD pWxNode, DWORD_LIST& dwListAddr, DWORD dwStart, DWORD dwTotal)
{
	if (dwListAddr.size() >= dwTotal)return;
	for (int i = 0; i < 3; i++)
	{
		DWORD dwAddr = pWxNode[i];
		DWORD_LIST::iterator iterator;
		iterator = std::find(dwListAddr.begin(), dwListAddr.end(), dwAddr);
		if (iterator == dwListAddr.end() && dwAddr != dwStart)
		{
			//递归查找
			dwListAddr.push_back(dwAddr);
			TraversalWxUserNode((PDWORD)dwAddr, dwListAddr, dwStart, dwTotal);
		}
	}
}

typedef struct _WXTEXT
{
	PWSTR buffer;
	UINT length;
	UINT maxLen;
	UINT fill;
	UINT fill2;
} WXTEXT, *PWXTEXT;

/*返回值暂时用不上，先留着*/
std::list<WxUser> GetFriendList()
{
	std::list<WxUser> list;
	auto pBase = (PDWORD)(GetModuleWeChatWinAddress() + 0x113227C);
	DWORD dwWxUserLinkedListAddr = (*pBase) + 0x24 + 0x68; //链表的地址
	RAW_WXNODE wxFriendsNode = (RAW_WXNODE)dwWxUserLinkedListAddr;
	DWORD_LIST listFriendAddr;
	if (wxFriendsNode == nullptr) {
		return list;
	}

	//if (0 != IsBadReadPtr((void*)wxFriendsNode, sizeof(DWORD))) {
	//	return list;
	//}

	TraversalWxUserNode(wxFriendsNode->ul.pStartNode, listFriendAddr, wxFriendsNode->ul.dwStart, wxFriendsNode->dwTotal);
	DWORD_LIST::iterator itor;
	for (itor = listFriendAddr.begin(); itor != listFriendAddr.end(); itor++)
	{
		WxUser user;
		DWORD dwAddr = *itor;
		WXTEXT wxId = *((PWXTEXT)(dwAddr + 0x10));
		WXTEXT account = *((PWXTEXT)(dwAddr + 0x30));
		WXTEXT remark = *((PWXTEXT)(dwAddr + 0x78));
		WXTEXT nickname = *((PWXTEXT)(dwAddr + 0x8C));
		WXTEXT avatar = *((PWXTEXT)(dwAddr + 0x11C));
		WXTEXT bigAvatar = *((PWXTEXT)(dwAddr + 0x130));
		WXTEXT addType = *((PWXTEXT)(dwAddr + 0x294));

		swprintf_s(user.wxid, L"%s", wxId.buffer);
		swprintf_s(user.account, L"%s", account.buffer);
		swprintf_s(user.username, L"%s", account.buffer);
		swprintf_s(user.remark, L"%s", remark.buffer);
		swprintf_s(user.nickname, L"%s", nickname.buffer);
		swprintf_s(user.avatar, L"%s", avatar.buffer);
		//swprintf_s(user.addType, L"%s", addType.buffer);
		/*if (user.remark == "(null)") {
		}*/

		LVITEM lvI;
		ZeroMemory(&lvI, sizeof(lvI));
		lvI.mask = LVIF_TEXT;
		lvI.iSubItem = 0;
		lvI.iItem = 0;
		lvI.pszText = wxId.buffer;
		int now = ListView_InsertItem(hwnd_list, (LPARAM)&lvI);
		if (now == -1)
			continue;

		ListView_SetItemText(hwnd_list, now, 0, wxId.buffer);
		ListView_SetItemText(hwnd_list, now, 1, account.buffer);
		ListView_SetItemText(hwnd_list, now, 2, nickname.buffer);

		list.push_back(user);
	}
	return list;
}