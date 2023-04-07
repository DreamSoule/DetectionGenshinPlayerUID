#include <windows.h>
#include <stdio.h>

/*
颜色属性由两个十六进制数字指定 -- 第一个为背景，第二个则为前景。每个数字可以为以下任何值之一:

    0 = 黑色       8 = 灰色
    1 = 蓝色       9 = 淡蓝色
    2 = 绿色       A = 淡绿色
    3 = 湖蓝色     B = 淡浅绿色
    4 = 红色       C = 淡红色
    5 = 紫色       D = 淡紫色
    6 = 黄色       E = 淡黄色
    7 = 白色       F = 亮白色
system("color 02");比如这个就是黑底绿字
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
// 设置控制台默认窗口字体颜色及背景颜色
void ClearConsoleToColors(int ForgC, int BackC);

// This will clear the console.
void ClearConsole();

// This will set the position of the cursor
// 设置光标位置
void gotoXY(int x, int y);

// This will set the forground color for printing in a console window.
// 仅设置字体颜色
void SetColor(int ForgC);

// This will set the forground and background color for printing in a console window.
// 仅打印出的文本字体颜色及背景
void SetColorAndBackground(int ForgC, int BackC);

// Direct console output
// 打印字符串
void ConPrint(const char* CharBuffer, int len = -1);

// Direct Console output at a particular coordinate.
// 在某个位置打印字符串
void ConPrintAt(int x, int y, const char* CharBuffer, int len = -1);

// Hides the console cursor
// 隐藏控制台光标[>_] -> [> ]
void HideTheCursor();

// Shows the console cursor
// 显示控制台光标[> ] -> [>_]
void ShowTheCursor();