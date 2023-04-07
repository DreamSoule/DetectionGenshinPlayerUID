#include <windows.h>
#include <iostream>
#include <time.h>

#include "log.h"
#include "ConsoleColor.h"

void Print(int StrColor, int BackColor, const char* fmt, ...) {
    char buf[1024] = { 0 };
    va_list va_alist;
    va_start(va_alist, fmt);
    vsprintf_s(buf, fmt, va_alist);
    va_end(va_alist);
    if (StrColor == -1)  StrColor = CmdCol::White;
    if (BackColor == -1) BackColor = CmdCol::Black;
    SetColorAndBackground(StrColor, BackColor);
    // ConPrint(buf);

    struct tm stime;
    time_t now = time(NULL);;
    localtime_s(&stime, &now);

    char tmp[32] = { NULL };
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &stime);
    std::string date(tmp);
    char buf2[1200] = { 0 };
    sprintf_s(buf2, "[%s] %s", date.c_str(), buf);
    printf("%s", buf2);

    // log file

    char aFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, aFilePath, MAX_PATH);

    std::string RunningPath(aFilePath);
    int nFindLast = RunningPath.find_last_of("\\");
    RunningPath = RunningPath.substr(0, nFindLast + 1);

    char FileName_log[265];
    sprintf_s(FileName_log, "%soutput_log.log", RunningPath.c_str());

    FILE* fp = fopen(FileName_log, "a+");
    if (fp) {
        fwrite(buf2, strlen(buf2), 1, fp);
        fclose(fp);
    }
}

void Print_Log(int StrColor, int BackColor, Log_Level LogLevel, const char* fmt, ...) {
    char buf[1024] = { 0 };
    va_list va_alist;
    va_start(va_alist, fmt);
    vsprintf_s(buf, fmt, va_alist);
    va_end(va_alist);
    if (StrColor == -1)  StrColor = CmdCol::White;
    if (BackColor == -1) BackColor = CmdCol::Black;
    SetColorAndBackground(StrColor, BackColor);
    // ConPrint(buf);

    const char* LogLevel_Str = NULL;
    switch (LogLevel)
    {
    case Log_Level::Normal:
    case Log_Level::Info:
        LogLevel_Str = "Info";
        SetColorAndBackground(CmdCol::Green, BackColor);
        break;
    case Log_Level::Waring:
        LogLevel_Str = "Waring";
        SetColorAndBackground(CmdCol::Yellow, BackColor);
        break;
    case Log_Level::Error:
        LogLevel_Str = "Error";
        SetColorAndBackground(CmdCol::Red, BackColor);
        break;
    default:
        LogLevel_Str = "unknown";
        SetColorAndBackground(CmdCol::White, BackColor);
        break;
    }

    struct tm stime;
    time_t now = time(NULL);;
    localtime_s(&stime, &now);

    char tmp[32] = { NULL };
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", &stime);
    std::string date(tmp);
    char buf2[1200] = { 0 };
    sprintf_s(buf2, "[%s] [%s] %s", date.c_str(), LogLevel_Str, buf);

    // log file
    char aFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameA(NULL, aFilePath, MAX_PATH);

    std::string RunningPath(aFilePath);
    int nFindLast = RunningPath.find_last_of("\\");
    RunningPath = RunningPath.substr(0, nFindLast + 1);

    char FileName_log[265];
    sprintf_s(FileName_log, "%soutput_log.log", RunningPath.c_str());

    FILE* fp = fopen(FileName_log, "a+");
    if (fp) {
        fwrite(buf2, strlen(buf2), 1, fp);
        fclose(fp);
    }


    printf("%s", buf2);
}
