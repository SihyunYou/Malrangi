#pragma once
#include "malrangi.h"

enum
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
enum NIVEAU_DE_LOG
{
	INFO = WHITE,
	WARNING = YELLOW,
	CRITICAL = RED,
	SUCCESS = LIGHT_CYAN,
	FAILURE = LIGHT_RED
};


#define __FILENAME__		(strrchr(__FILE__,'\\')+1)
#define WriteLog(_lv, _fm, ...)	{																									\
											NumberOfLine = __LINE__;															\
											FileName = __FILENAME__;															\
											VWriteLog(_lv, _fm, __VA_ARGS__);												\
										}
INT NumberOfLine;
LPCSTR FileName;
ofstream LogFile{ SNAP_DIR "log.txt", ios::out || ios::trunc };

void VWriteLog(NIVEAU_DE_LOG LogLevel, LPCSTR lpFormat, ...)
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

	SetConsoleTextColor(GREEN);
	string LogContent = GetCurrentDateTimeString() + ", " + to_string(NumberOfLine) + " [" + FileName + "] ";

	SetConsoleTextColor(LogLevel);
	switch (LogLevel)
	{
	case INFO:
		LogContent += "INFO: "; break;
	case WARNING:
		LogContent += "WARNING: "; break;
	case CRITICAL:
		LogContent += "CRITICAL: "; break;
	}

	va_list Args;
	char Buffer[0x100];

	va_start(Args, lpFormat);
	vsprintf(Buffer, lpFormat, Args);
	va_end(Args);

	LogContent += Buffer;

	LogFile << LogContent;
	cout << LogContent;
}