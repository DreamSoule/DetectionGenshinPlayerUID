#include <windows.h>

enum class Log_Level : char{
	Null = 0,
	Normal,
	Info,
	Waring,
	Error,
	Max,
};

void Print(int StrColor, int BackColor, const char* fmt, ...);
void Print_Log(int StrColor, int BackColor, Log_Level LogLevel, const char* fmt, ...);


#define log(LogLevel, ...) Print_Log(-1, -1, LogLevel, __VA_ARGS__)
#define log_info(...) Print_Log(-1, -1, Log_Level::Info, __VA_ARGS__)
#define log_waring(...) Print_Log(-1, -1, Log_Level::Waring, __VA_ARGS__)
#define log_error(...) Print_Log(-1, -1, Log_Level::Error, __VA_ARGS__)