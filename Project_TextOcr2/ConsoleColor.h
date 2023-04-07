#include <windows.h>
#include <stdio.h>

/*
��ɫ����������ʮ����������ָ�� -- ��һ��Ϊ�������ڶ�����Ϊǰ����ÿ�����ֿ���Ϊ�����κ�ֵ֮һ:

    0 = ��ɫ       8 = ��ɫ
    1 = ��ɫ       9 = ����ɫ
    2 = ��ɫ       A = ����ɫ
    3 = ����ɫ     B = ��ǳ��ɫ
    4 = ��ɫ       C = ����ɫ
    5 = ��ɫ       D = ����ɫ
    6 = ��ɫ       E = ����ɫ
    7 = ��ɫ       F = ����ɫ
system("color 02");����������Ǻڵ�����
*/

//enum CMD_Color : int {
//    CMD_Color__Black,
//    CMD_Color__Blue,
//    CMD_Color__Green,
//    CMD_Color__LakeBlue,
//    CMD_Color__Red,
//    CMD_Color__Purple,
//    CMD_Color__Yellow,
//    CMD_Color__White,
//    CMD_Color__Grey,
//    CMD_Color__LightBlue,
//    CMD_Color__LightGreen,  // 0xA
//    CMD_Color__LightGreenA,
//    CMD_Color__LightRed,
//    CMD_Color__LightPurple,
//    CMD_Color__LightYellow,
//    CMD_Color__LightWhite,
//};

namespace CmdCol {
    enum CMD_Color : int {
        Black,
        Blue,
        Green,
        LakeBlue,
        Red,
        Purple,
        Yellow,
        White,
        Grey,
        LightBlue,
        LightGreen,  // 0xA
        LightGreenA,
        LightRed,
        LightPurple,
        LightYellow,
        LightWhite,
    };
}

int Demo(int argc, char* argv[]);

//This will clear the console while setting the forground and background colors.
// ���ÿ���̨Ĭ�ϴ���������ɫ��������ɫ
void ClearConsoleToColors(int ForgC, int BackC);

// This will clear the console.
void ClearConsole();

// This will set the position of the cursor
// ���ù��λ��
void gotoXY(int x, int y);

// This will set the forground color for printing in a console window.
// ������������ɫ
void SetColor(int ForgC);

// This will set the forground and background color for printing in a console window.
// ����ӡ�����ı�������ɫ������
void SetColorAndBackground(int ForgC, int BackC);

// Direct console output
// ��ӡ�ַ���
void ConPrint(const char* CharBuffer, int len = -1);

// Direct Console output at a particular coordinate.
// ��ĳ��λ�ô�ӡ�ַ���
void ConPrintAt(int x, int y, const char* CharBuffer, int len = -1);

// Hides the console cursor
// ���ؿ���̨���[>_] -> [> ]
void HideTheCursor();

// Shows the console cursor
// ��ʾ����̨���[> ] -> [>_]
void ShowTheCursor();