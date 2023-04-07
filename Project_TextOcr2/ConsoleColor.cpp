#include "ConsoleColor.h"

//int Demo(int argc, char* argv[]) {
//   HideTheCursor();
//   ClearConsoleToColors(CmdCol::White, CmdCol::Black); Sleep(1000);
//   ClearConsole(); Sleep(1000);
//   gotoXY(1, 1); Sleep(1000);
//   SetColor(14); Sleep(1000);
//   printf("This is a test...\n");
//   ShowTheCursor(); Sleep(1000);
//   SetColorAndBackground(15, 12); Sleep(1000);
//   ConPrint("This is also a test...\n", 23);
//   SetColorAndBackground(CmdCol::Blue, CmdCol::White); Sleep(1000);
//   ConPrintAt(22, 15, "This is also a test...\n");
//   printf("This is last test...\n"); Sleep(1000);
//   gotoXY(0, 24); Sleep(1000);
//   SetColorAndBackground(CmdCol::White, CmdCol::Blue); Sleep(1000);
//   return 0;
//}

//This will clear the console while setting the forground and
//background colors.
void ClearConsoleToColors(int ForgC, int BackC) {
   WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
   //Get the handle to the current output buffer...
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   //This is used to reset the carat/cursor to the top left.
   COORD coord = {0, 0};
   //A return value... indicating how many chars were written
   //not used but we need to capture this since it will be
   //written anyway (passing NULL causes an access violation).
   DWORD count;

   //This is a structure containing all of the console info
   // it is used here to find the size of the console.
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   //Here we will set the current color
   SetConsoleTextAttribute(hStdOut, wColor);
   if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
      //This fills the buffer with a given character (in this case 32=space).
      FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);

      FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
      //This will set our cursor position for the next print statement.
      SetConsoleCursorPosition(hStdOut, coord);
   }
}

void ClearConsole() {
   //Get the handle to the current output buffer...
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   //This is used to reset the carat/cursor to the top left.
   COORD coord = {0, 0};
   //A return value... indicating how many chars were written
   //   not used but we need to capture this since it will be
   //   written anyway (passing NULL causes an access violation).
   DWORD count;
   //This is a structure containing all of the console info
   // it is used here to find the size of the console.
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   //Here we will set the current color
   if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
      //This fills the buffer with a given character (in this case 32=space).
      FillConsoleOutputCharacter(hStdOut, (TCHAR) 32, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
      FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
      //This will set our cursor position for the next print statement.
      SetConsoleCursorPosition(hStdOut, coord);
   }
}

void gotoXY(int x, int y) {
   //Initialize the coordinates
   COORD coord = {x, y};
   //Set the position
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void SetColor(int ForgC) {
   WORD wColor;
   //We will need this handle to get the current background attribute
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   CONSOLE_SCREEN_BUFFER_INFO csbi;

   //We use csbi for the wAttributes word.
   if(GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
      //Mask out all but the background attribute, and add in the forgournd color
      wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
      SetConsoleTextAttribute(hStdOut, wColor);
   }
}

void SetColorAndBackground(int ForgC, int BackC) {
   WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);;
   SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
}

void ConPrint(const char *CharBuffer, int len) {
   DWORD count;
   if (len == -1) len = strlen(CharBuffer);
   WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), CharBuffer, len, &count, NULL);
}

void ConPrintAt(int x, int y, const char *CharBuffer, int len) {
   DWORD count;
   COORD coord = {x, y};
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
   if (len == -1) len = strlen(CharBuffer);
   SetConsoleCursorPosition(hStdOut, coord);
   WriteConsole(hStdOut, CharBuffer, len, &count, NULL);
}

void HideTheCursor() {
   CONSOLE_CURSOR_INFO cciCursor;
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

   if(GetConsoleCursorInfo(hStdOut, &cciCursor)) {
      cciCursor.bVisible = FALSE;
      SetConsoleCursorInfo(hStdOut, &cciCursor);
   }
}

void ShowTheCursor() {
   CONSOLE_CURSOR_INFO cciCursor;
   HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

   if(GetConsoleCursorInfo(hStdOut, &cciCursor)) {
      cciCursor.bVisible = TRUE;
      SetConsoleCursorInfo(hStdOut, &cciCursor);
   }
}