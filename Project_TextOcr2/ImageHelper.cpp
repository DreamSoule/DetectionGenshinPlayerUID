#include <windows.h>
#include <iostream>
#include "ImageHelper.h"


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0;          // number of image encoders  
    UINT  size = 0;         // size of the image encoder array in bytes  

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure  

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (!pImageCodecInfo)
        return -1;  // Failure  

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
    if (!pImageCodecInfo) return -1;

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success  
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure  
}

bool ImageHelper::SaveBitmapToFile(HBITMAP hBitmap, const std::string& filename) {
    // 1. ����λͼ�ļ�
    const auto file = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    // 2. ����λͼ�ļ�ÿ��������ռ�ֽ���
    const auto bitCount = GetBitmapBitCount();

    // 3. ��ȡλͼ�ṹ
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(bitmap), reinterpret_cast<LPSTR>(&bitmap));

    // λͼ�������ֽڴ�С(32�ֽڶ���)
    const DWORD bmBitsSize = ((bitmap.bmWidth * bitCount + 31) / 32) * 4 * bitmap.bmHeight;

    // ��ɫ���С
    const DWORD paletteSize = 0;

    // 4. ����λͼ��Ϣͷ
    BITMAPINFOHEADER  bmpInfoHeader; // λͼ��Ϣͷ�ṹ
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biWidth = bitmap.bmWidth;
    bmpInfoHeader.biHeight = bitmap.bmHeight;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = bitCount;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biSizeImage = 0;
    bmpInfoHeader.biXPelsPerMeter = 0;
    bmpInfoHeader.biYPelsPerMeter = 0;
    bmpInfoHeader.biClrImportant = 0;
    bmpInfoHeader.biClrUsed = 0;

    // 5. ����λͼ�ļ�ͷ
    BITMAPFILEHEADER bmpFileHeader;
    bmpFileHeader.bfType = 0x4D42; // "BM"
    // λͼ�ļ���С
    const DWORD dibSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize + bmBitsSize;
    bmpFileHeader.bfSize = dibSize;
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER)) + static_cast<DWORD>(sizeof(BITMAPINFOHEADER)) + paletteSize;

    // 6. Ϊλͼ���ݷ����ڴ� 
    const auto dib = GlobalAlloc(GHND, bmBitsSize + paletteSize + sizeof(BITMAPINFOHEADER)); // �ڴ���
    if (!dib) return false;

    const auto lpBmpInfoHeader = static_cast<LPBITMAPINFOHEADER>(GlobalLock(dib)); // ָ��λͼ��Ϣͷ�ṹ
    if (!lpBmpInfoHeader) return false;

    *lpBmpInfoHeader = bmpInfoHeader;

    // 7. �����ɫ��
    ProcessPalette(hBitmap, bitmap, paletteSize, lpBmpInfoHeader);

    // 8. д���ļ�
    DWORD written = 0; //д���ļ��ֽ���   
    WriteFile(file, reinterpret_cast<LPSTR>(&bmpFileHeader), sizeof(BITMAPFILEHEADER), &written, nullptr); // д��λͼ�ļ�ͷ
    WriteFile(file, reinterpret_cast<LPSTR>(lpBmpInfoHeader), dibSize, &written, nullptr); // д��λͼ�ļ���������

    // 9. ������Դ
    GlobalUnlock(dib);
    GlobalFree(dib);
    CloseHandle(file);

    return true;
}

bool ImageHelper::SaveBitmapToFile_PNG(HBITMAP hBitmap, const std::wstring& filename) {
    bool bResult = true;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
        return false;
    }
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    Gdiplus::Bitmap* pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBitmap, NULL);
    if (pbmSrc->Save(filename.c_str(), &pngClsid) != Gdiplus::Ok) {
        bResult = false;
    }
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return bResult;
}

// ����λͼ�ļ�ÿ��������ռ�ֽ���
WORD ImageHelper::GetBitmapBitCount() {
    const auto dc = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    // ��ǰ�ֱ�����ÿ������ռ�ֽ���
    const auto bits = GetDeviceCaps(dc, BITSPIXEL) * GetDeviceCaps(dc, PLANES);
    DeleteDC(dc);

    // λͼ��ÿ������ռ�ֽ���
    WORD bitCount;
    if (bits <= 1)
        bitCount = 1;
    else if (bits <= 4)
        bitCount = 4;
    else if (bits <= 8)
        bitCount = 8;
    else
        bitCount = 24;

    return bitCount;
}

// �����ɫ��
void ImageHelper::ProcessPalette(HBITMAP hBitmap, const BITMAP& bitmap, DWORD paletteSize, LPBITMAPINFOHEADER lpBmpInfoHeader) {
    HANDLE oldPalette = nullptr;
    HDC dc = nullptr;
    const auto palette = GetStockObject(DEFAULT_PALETTE);
    if (!palette) return;

    dc = GetDC(nullptr);
    if (!dc) return;

    oldPalette = SelectPalette(dc, static_cast<HPALETTE>(palette), FALSE);
    if (!oldPalette) return;

    // ʵ���豸��ɫ��
    RealizePalette(dc);

    // ��ȡ�õ�ɫ�����µ�����ֵ
    GetDIBits(dc, hBitmap, 0, static_cast<UINT>(bitmap.bmHeight),
        reinterpret_cast<LPSTR>(lpBmpInfoHeader) + sizeof(BITMAPINFOHEADER) + paletteSize,
        reinterpret_cast<BITMAPINFO*>(lpBmpInfoHeader), DIB_RGB_COLORS);

    // �ָ���ɫ��
    SelectPalette(dc, static_cast<HPALETTE>(oldPalette), TRUE);
    RealizePalette(dc);
    ReleaseDC(nullptr, dc);
}