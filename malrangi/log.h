#pragma once
#include <windows.h>
#include <ctime>
#include <mutex>
using namespace std;

enum CONSOLE_COLOR
{
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GRAY,
	DARK_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	YELLOW,
	WHITE
};
enum LOG_LEVEL
{
	INFO = WHITE,
	WARNING = YELLOW,
	CRITICAL = RED,
	SUCCESS = LIGHT_CYAN,
	FAILURE = LIGHT_RED
};

mutex MutexLocker;
INT NumberOfLine;
LPCSTR lpFileName;
#define __FILENAME__		(strrchr(__FILE__,'\\')+1)
#define WriteLog(_lv, _fm, ...)	{																									\
											MutexLocker.lock();																			\
											NumberOfLine = __LINE__;															\
											lpFileName = __FILENAME__;															\
											VWriteLog(_lv, _fm, __VA_ARGS__);												\
											MutexLocker.unlock();																		\
										}
void VWriteLog(LOG_LEVEL LogLevel, LPCSTR lpFormat, ...)
{
	auto GetCurrentDateTimeString = [](void) -> string
	{
		time_t Now = time(nullptr);
		struct tm* TimeInfo;
		CHAR Buf[0x40];
		TimeInfo = localtime(&Now);

		strftime(Buf, sizeof(Buf), "%X", TimeInfo);
		return Buf;
	};
	auto SetConsoleTextColor = [](INT ColorValue) -> void
	{
		WORD wColor;
		CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
		HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if (GetConsoleScreenBufferInfo(hStdOutput, &ConsoleScreenBufferInfo))
		{
			wColor = (ConsoleScreenBufferInfo.wAttributes & 0xF0) + (ColorValue & 0x0F);
			SetConsoleTextAttribute(hStdOutput, wColor);
		}
	};
#define SetConsolePosition(_x, _y)	SetConsoleCursorPosition(hStdOutput, {_x, _y})

	SetConsoleTextColor(2);
	cout << GetCurrentDateTimeString() << ", " << NumberOfLine << " [" << lpFileName << "] ";

	va_list Args;
	va_start(Args, lpFormat);

	SetConsoleTextColor(LogLevel);
	switch (LogLevel)
	{
	case INFO:
		cout << "INFO: ";
		break;

	case WARNING:
		cout << "WARNING: ";
		break;

	case CRITICAL:
		cout << "CRITICAL: ";
		break;
	}
	vprintf(lpFormat, Args);

	va_end(Args);
}