#pragma once
#include <Windows.h>
#include <string>

HMODULE	RemoteInjectDLL(DWORD pid, const char* path);
BOOL	RemoteFreeDLL(DWORD pid, HMODULE hModule);

BOOL	AdjustSeltTokenPrivileges();

int		GetProcessID(const std::string& p_name);
