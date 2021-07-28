#pragma once

#include <Windows.h>
#include <iostream>
#include <assert.h>

extern bool isHooked;

typedef int(*ExecuteMAXScriptScript)(const WCHAR* s, BOOL quietErrors, void* fpv);
BOOL HookApi_Script();
BOOL UnhookApi_Script();