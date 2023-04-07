#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <io.h>
#include "GenshinUID.h"
// https://github.com/rxi/ini
#include "ini.h"
#include "log.h"

int  beep_error        = 3000;
int  beep_normal       = 200;
int  beep_time         = 200;
char config_file[]     = "config.ini";
char black_list_file[] = "find_uid_list.ini";

std::string utf8_to_ansi(std::string strUTF8) {
    UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, NULL, NULL);
    WCHAR* wszBuffer = new WCHAR[nLen + 1];
    nLen = MultiByteToWideChar(CP_UTF8, NULL, strUTF8.c_str(), -1, wszBuffer, nLen);
    wszBuffer[nLen] = 0;
    nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
    CHAR* szBuffer = new CHAR[nLen + 1];
    nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
    szBuffer[nLen] = 0;
    strUTF8 = szBuffer;
    delete[]szBuffer;
    delete[]wszBuffer;
    return strUTF8;
}
std::string ansi_to_utf8(std::string strAnsi) {
    UINT nLen = MultiByteToWideChar(936, NULL, strAnsi.c_str(), -1, NULL, NULL);
    WCHAR* wszBuffer = new WCHAR[nLen + 1];
    nLen = MultiByteToWideChar(936, NULL, strAnsi.c_str(), -1, wszBuffer, nLen);
    wszBuffer[nLen] = 0;
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
    CHAR* szBuffer = new CHAR[nLen + 1];
    nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
    szBuffer[nLen] = 0;
    strAnsi = szBuffer;
    delete[]wszBuffer;
    delete[]szBuffer;
    return strAnsi;
}
BOOL CopyToClipboard(std::string copy_str) {
    if (OpenClipboard(NULL)) {
        EmptyClipboard();
        HGLOBAL clipbuffer;
        char* buffer;
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, copy_str.size() + 1);
        if (!clipbuffer) return FALSE;
        buffer = (char*)GlobalLock(clipbuffer);
        memcpy_s(buffer, copy_str.size(), copy_str.data(), copy_str.size());
        GlobalUnlock(clipbuffer);
        SetClipboardData(CF_TEXT, clipbuffer);
        CloseClipboard();
        return TRUE;
    }
    return FALSE;
}

void real_main() {
    RECT config_uid_Location = { 0, 0, 0, 0 };
    if (_access(config_file, 4) == 0) { // R_OK
        auto config = ini_load(config_file);
        if (config) {
            ini_sget(config, "uid_location", "left", "%d", &config_uid_Location.left);
            ini_sget(config, "uid_location", "top", "%d", &config_uid_Location.top);
            ini_sget(config, "uid_location", "right", "%d", &config_uid_Location.right);
            ini_sget(config, "uid_location", "bottom", "%d", &config_uid_Location.bottom);
            ini_free(config);
        }
        else {
            log_error("load ini \"%s\" error !\n", config_file);
        }

        log_info("read cfg uid_location: %d %d %d %d\n", config_uid_Location.left, config_uid_Location.top, config_uid_Location.right, config_uid_Location.bottom);
    }
    else {
        log_waring("file \"%s\" not found !\n", config_file);
    }

    bool b_exit = false;
    while (!b_exit) {
        auto vk_Ctrl   = GetAsyncKeyState(VK_CONTROL);
        auto vk_Alt    = GetAsyncKeyState(VK_MENU);
        auto vk_End    = GetAsyncKeyState(VK_END);
        auto vk_F1     = GetAsyncKeyState(VK_F1);
        auto vk_F2     = GetAsyncKeyState(VK_F2);
        auto vk_Mouse1 = GetAsyncKeyState(VK_LBUTTON);
        auto vk_Mouse2 = GetAsyncKeyState(VK_RBUTTON);


        if (vk_Alt && vk_End & 1) {
            b_exit = true;
        }

        if (vk_Alt) {
        //    auto vk_F1   = GetAsyncKeyState(VK_F1);
        //    auto vk_F2   = GetAsyncKeyState(VK_F2);
            if (vk_F1 & 1 || vk_F2 & 1) {
                int result = -1;
                if (config_uid_Location.left && config_uid_Location.top && config_uid_Location.right && config_uid_Location.bottom) {
                    log_info("use cfg uid_location: %d %d %d %d\n", config_uid_Location.left, config_uid_Location.top, config_uid_Location.right, config_uid_Location.bottom);
                    result = GetGenshinPlayerUID(config_uid_Location);
                }
                else {
                    HWND hWnd = FindWindowA(NULL, "原神");
                    if (!hWnd) hWnd = FindWindowA(NULL, "Genshin Impact");

                    log_info("genshin impact hWnd: 0x%p\n", hWnd);
                    result = GetGenshinPlayerUID(hWnd);
                }
                int be_find_uid = result;
                log_info("GetGenshinPlayerUID: %d\n", be_find_uid);

                if (vk_F1) {
                    char uid_str[16] = {};
                    sprintf_s(uid_str, "%d", be_find_uid);
                    auto uid_str_len = strlen(uid_str);
                    if (uid_str_len == 9) {
                        CopyToClipboard(uid_str);
                    }
                    else {
                        log_error("copy uid_str.len(%d) error !\n", uid_str_len);
                    }
                }
                else if (vk_F2) {
                    if (_access(black_list_file, 4) == 0) { // R_OK
                        auto uid_list = ini_load(black_list_file);
                        if (uid_list) {
                            char uid_str[16] = {};
                            char note[256] = {};
                            sprintf_s(uid_str, "%d", be_find_uid);
                            auto uid_str_len = strlen(uid_str);
                            if (uid_str_len == 9) {
                                int sget_result = ini_sget(uid_list, uid_str, "note", "%s", note);
                                ini_free(uid_list);

                                if (sget_result) {
                                    log_waring("uid: %d | note: %s\n", be_find_uid, utf8_to_ansi(note).c_str());
                                    Beep(beep_error, beep_time);
                                }
                                else {
                                    log_info("uid: %d not found\n", be_find_uid);
                                    Beep(beep_normal, beep_time);
                                }
                            }
                            else {
                                log_error("uid_str.len(%d) error !\n", uid_str_len);
                            }
                        }
                        else {
                            log_error("load ini \"%s\" error !\n", black_list_file);
                        }
                    }
                    else {
                        log_error("file \"%s\" not found !\n", black_list_file);
                    }
                }
            }
        }

        if (vk_Ctrl && vk_Alt) {
            //auto vk_Mouse1 = GetAsyncKeyState(VK_LBUTTON);
            //auto vk_Mouse2 = GetAsyncKeyState(VK_RBUTTON);

            if (vk_Mouse1 & 1 || vk_Mouse2 & 1) {
                POINT Mouse;
                if (GetCursorPos(&Mouse)) {
                    log_info("GetCursorPos: %d %d\n", Mouse.x, Mouse.y);
                    if (vk_Mouse1) {
                        config_uid_Location.left   = Mouse.x;
                        config_uid_Location.top    = Mouse.y;
                    }
                    if (vk_Mouse2) {
                        config_uid_Location.right  = Mouse.x;
                        config_uid_Location.bottom = Mouse.y;
                    }
                    log_info("current uid_location: %d %d %d %d\n", config_uid_Location.left, config_uid_Location.top, config_uid_Location.right, config_uid_Location.bottom);
                }
                else {
                    log_error("GetCursorPos error !\n");
                }
            }
        }

        Sleep(100);
    }
}

int main(int argc, char* argv[]) {
    // log_waring("[%s.%s:%d] test waring\n", __FILE__, __FUNCTION__, __LINE__);
    // log_error("[%s.%s:%d] test error\n", __FILE__, __FUNCTION__, __LINE__);
    system("cls");
    log_info("文件编译时间: %s %s\n\n", __DATE__, __TIME__);
    log_info("使用说明:\n");
    log_info("  按下 Alt  + F1 快速复制uid\n");
    log_info("  按下 Alt  + F2 进行uid识别\n");
    log_info("  按下 Alt  + End 快速退出\n");
    log_info("  按下 Ctrl + Alt + 鼠标左键 设置UID左上角位置\n");
    log_info("  按下 Ctrl + Alt + 鼠标右键 设置UID右下角位置\n\n");

    real_main();

    return 0;
}
