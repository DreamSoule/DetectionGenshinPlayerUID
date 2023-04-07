#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

class ImageHelper {
public:
    static bool SaveBitmapToFile(HBITMAP bitmap, const std::string& filename); // ����λͼ���ļ�
    static bool SaveBitmapToFile_PNG(HBITMAP bitmap, const std::wstring& filename); // ����λͼ���ļ�

private:
    static WORD GetBitmapBitCount(); // ����λͼ�ļ�ÿ��������ռ�ֽ���
    static void ProcessPalette(HBITMAP hBitmap, const BITMAP& bitmap, DWORD paletteSize, LPBITMAPINFOHEADER lpBmpInfoHeader); // �����ɫ��
};