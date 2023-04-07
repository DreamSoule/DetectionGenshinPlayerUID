#include <windows.h>
#include <iostream>
#include "ImageHelper.h"
#include "GenshinUID.h"
#include "log.h"

struct Vctor2i {
    int x, y;
};

//BOOL GetRealWindowRect(HWND hWnd, LPRECT lpRect) {
//    RECT ClientRect;
//    RECT WindowRect;
//    RECT WindowRect_Fix;
//    RECT RealRect;
//    RECT Rect_Offset;
//
//    if (!lpRect) return false;
//    if (!GetWindowRect(hWnd, &WindowRect)) return false;
//    if (!GetClientRect(hWnd, &ClientRect)) return false;
//
//    WindowRect_Fix.right  = WindowRect.right  - WindowRect.left;
//    WindowRect_Fix.bottom = WindowRect.bottom - WindowRect.top;
//
//    Rect_Offset.left   = 0;
//    Rect_Offset.top    = 0;
//    Rect_Offset.right  = (WindowRect_Fix.right  - ClientRect.right);
//    Rect_Offset.bottom = (WindowRect_Fix.bottom - ClientRect.bottom);
//
//    RealRect.left   = WindowRect.left   + Rect_Offset.left;
//    RealRect.top    = WindowRect.top    + Rect_Offset.top;
//    RealRect.right  = WindowRect.right  - Rect_Offset.right;
//    RealRect.bottom = WindowRect.bottom - Rect_Offset.bottom;
//
//    *lpRect = RealRect;
//    return true;
//}


int GetGenshinPlayerUID(RECT Location) {
    static auto RemoveCacheFile = []() {
        remove("genshin_uid_output.txt");
        remove("screenshot_uid.bmp");
        remove("screenshot_uid_color.png");
    };

    RemoveCacheFile();

    Vctor2i uid_Size = { Location.right - Location.left, Location.bottom - Location.top };


    HDC hdcScreen = GetDC(NULL);
    HDC hdcCompatible = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmpScreen = CreateCompatibleBitmap(hdcScreen, uid_Size.x, uid_Size.y);
    SelectObject(hdcCompatible, hbmpScreen);
    BitBlt(hdcCompatible, 0, 0, uid_Size.x, uid_Size.y, hdcScreen, Location.left, Location.top, SRCCOPY);
    ImageHelper::SaveBitmapToFile(hbmpScreen, "screenshot_uid.bmp");
    DeleteDC(hdcScreen);
    DeleteDC(hdcCompatible);
    DeleteObject(hdcCompatible);


    // 设置要替换的颜色
    COLORREF fromColor = RGB(255, 255, 255);

    // 设置替换后的颜色
    COLORREF toColor = RGB(0, 255, 255);
    COLORREF otherColor = RGB(0, 0, 0);

    COLORREF scopeColor = RGB(30, 30, 30);

    // 加载要处理的图像
    HBITMAP hBmp = (HBITMAP)LoadImageA(NULL, "screenshot_uid.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    //HBITMAP hBmp = hbmpScreen;
    HDC hdc = CreateCompatibleDC(NULL);
    SelectObject(hdc, hBmp);

    // 获取图像尺寸
    BITMAP bmp;
    GetObject(hBmp, sizeof(bmp), &bmp);
    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    // 处理每个像素
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            COLORREF pixelColor = GetPixel(hdc, x, y);

            auto pixelColor_R = GetRValue(pixelColor);
            auto pixelColor_G = GetGValue(pixelColor);
            auto pixelColor_B = GetBValue(pixelColor);

            auto fromColor_R = GetRValue(fromColor);
            auto fromColor_G = GetGValue(fromColor);
            auto fromColor_B = GetBValue(fromColor);

            auto scopeColor_R = GetRValue(scopeColor);
            auto scopeColor_G = GetGValue(scopeColor);
            auto scopeColor_B = GetBValue(scopeColor);

            if (abs((char)fromColor_R - (char)pixelColor_R) <= scopeColor_R &&
                abs((char)fromColor_G - (char)pixelColor_G) <= scopeColor_G &&
                abs((char)fromColor_B - (char)pixelColor_B) <= scopeColor_B) {
                SetPixel(hdc, x, y, toColor);
            }
            else {
                SetPixel(hdc, x, y, otherColor);
            }
        }
    }

    // 保存为新的位图文件
    HBITMAP hNewBmp = CreateCompatibleBitmap(hdc, width, height);
    HDC hdcNew = CreateCompatibleDC(hdc);
    SelectObject(hdcNew, hNewBmp);
    BitBlt(hdcNew, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
    DeleteDC(hdc);
    DeleteObject(hBmp);

    ImageHelper::SaveBitmapToFile_PNG(hNewBmp, L"screenshot_uid_color.png");

    DeleteDC(hdcNew);
    DeleteObject(hNewBmp);

    STARTUPINFO si;
    RtlZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    PROCESS_INFORMATION pi;
    if (!CreateProcess(L".\\Tesseract-OCR\\tesseract.exe", (wchar_t*)L"tesseract .\\screenshot_uid_color.png genshin_uid_output -l eng", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        log_error("[%s.%s:%d] create tesseract process error\n", __FILE__, __FUNCTION__, __LINE__);
        return -6;
    }
    WaitForSingleObject(pi.hThread, -1);
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    char uid_buff[16] = {};
    FILE* fp = fopen(".\\genshin_uid_output.txt", "r");
    if (!fp) {
        log_error("[%s.%s:%d] read uid\n", __FILE__, __FUNCTION__, __LINE__);
        return -7;
    }
    fread_s(uid_buff, sizeof(uid_buff) - 1, 9, 1, fp);
    fclose(fp);

    int tmp_uid = atoi(uid_buff);
    if (tmp_uid < 100000000 || tmp_uid > 999999999)
        tmp_uid = 0;

    RemoveCacheFile();
    return tmp_uid;
}
int GetGenshinPlayerUID(HWND hWnd) {
    if (!hWnd) return -1;
    if (!IsWindowVisible(hWnd)) return -2;

    static auto GetAutoScreenScale = [](Vctor2i _BaseScreenSize, Vctor2i _CurScreenSize, Vctor2i _BaseLocation) {
        Vctor2i Result = _BaseLocation;
        if (_BaseScreenSize.x == _CurScreenSize.x && _BaseScreenSize.y == _CurScreenSize.y) {
            return Result;
        }
        Result.x = _BaseLocation.x / _BaseScreenSize.x * _CurScreenSize.x;
        Result.y = _BaseLocation.y / _BaseScreenSize.y * _CurScreenSize.y;
        return Result;
    };

    RECT ClientRect;
    if (!GetClientRect(hWnd, &ClientRect)) return -3;

    auto BaseScreenSize = Vctor2i{ 2560, 1440 }; // game window size
    auto CurrentScreenSize = Vctor2i{ ClientRect.right, ClientRect.bottom }; // game window size
    auto BaseLocation = Vctor2i{ 832, 265 };
    auto BaseSize = Vctor2i{ 152, 25 };

    auto AutoScreenLocation = GetAutoScreenScale(BaseScreenSize, CurrentScreenSize, BaseLocation);
    auto AutoScreenSize = GetAutoScreenScale(BaseScreenSize, CurrentScreenSize, BaseSize);

    log_info("Uid.Location: %d %d\n", AutoScreenLocation.x, AutoScreenLocation.y);
    log_info("Uid.Size: %d %d\n", AutoScreenSize.x, AutoScreenSize.y);

    RECT uid_location;
    //RECT RealWindowRect;
    //RECT WindowRect;
    uid_location.left   = AutoScreenLocation.x;
    uid_location.top    = AutoScreenLocation.y;
    uid_location.right  = uid_location.left + AutoScreenSize.x;
    uid_location.bottom = uid_location.top  + AutoScreenSize.y;

    //GetWindowRect(hWnd, &WindowRect);
    //GetClientRect(hWnd, &ClientRect);
    //GetRealWindowRect(hWnd, &RealWindowRect);

    //printf("WindowRect: %d %d %d %d\n", WindowRect.left, WindowRect.top, WindowRect.right, WindowRect.bottom);
    //printf("WindowRecT: %d %d %d %d\n", WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top);
    //printf("ClientRect: %d %d %d %d\n", ClientRect.left, ClientRect.top, ClientRect.right, ClientRect.bottom);
    //printf("RealWindowRect: %d %d %d %d\n", RealWindowRect.left, RealWindowRect.top, RealWindowRect.right, RealWindowRect.bottom);
    log_info("uid_location: %d %d %d %d\n", uid_location.left, uid_location.top, uid_location.right, uid_location.bottom);

    int tmp_uid = GetGenshinPlayerUID(uid_location);

    return tmp_uid;
}