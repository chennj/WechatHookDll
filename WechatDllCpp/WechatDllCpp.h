#pragma once

wchar_t * UTF8ToUnicode(const char* str);
void SendTextMessage(wchar_t * wxidOrQun, wchar_t * qunWxid, wchar_t * message);
void sendPicMessage(wchar_t* wxid, wchar_t* filepath);//·¢ËÍÍ¼Æ¬ÏûÏ¢