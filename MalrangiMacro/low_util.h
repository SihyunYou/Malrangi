/****************************************************************************
*                                                                           
* util.h -- Basic Macro Utilities for Imitating Devices.        
*                                                                           
****************************************************************************/
#pragma once

#include <iostream>
#include <fstream>
#include <windows.h>
#include <winuser.h>
#include <mmsystem.h>
#include <conio.h>
#include <string>
#include <array>
#include <vector>
#include <exception>

using namespace std;

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#define INTERVAL_MOUSEEVENT		(1000)
#define INTERVAL						(750)

/****************************************************************************
* Mouse Event Util
****************************************************************************/

#define LEFT_CLICK		(1)
#define RIGHT_CLICK		(0)

VOID
WINAPI
MouseEvent(
	_In_ CONST Point Coordinate,
	_In_ CHAR chFlag,
	_In_ DWORD dwMilliseconds = INTERVAL_MOUSEEVENT)
{
	DWORD PositionX = Coordinate.x * 65535 / GetSystemMetrics(SM_CXSCREEN);
	DWORD PositionY = Coordinate.y * 65535 / GetSystemMetrics(SM_CYSCREEN);

	::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, 
		PositionX, PositionY,
		0, ::GetMessageExtraInfo());

	if (chFlag == LEFT_CLICK)
	{
		::mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE,
			PositionX, PositionY, 
			0, ::GetMessageExtraInfo());
		::mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE,
			PositionX, PositionY, 
			0, ::GetMessageExtraInfo());
	}
	else if(chFlag == RIGHT_CLICK)
	{
		::mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE,
			PositionX, PositionY, 
			0, ::GetMessageExtraInfo());
		::mouse_event(MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE,
			PositionX, PositionY, 
			0, ::GetMessageExtraInfo());
	}
	else
	{
		::mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
				PositionX, PositionY,
				0, ::GetMessageExtraInfo());
	}
	if (0 < dwMilliseconds)
	{
		Sleep(dwMilliseconds);
	}
}



/****************************************************************************                                                                   
* Keyboard Event Util
****************************************************************************/

#define KeybdEventDown(_vk)	(keybd_event(_vk, MapVirtualKey(_vk, 0), 0, 0))							
#define KeybdEventUp(_vk)		(keybd_event(_vk, MapVirtualKey(_vk, 0), KEYEVENTF_KEYUP, 0))

VOID
WINAPI
KeybdEvent(
	_In_ BYTE bVirtualKey,
	_In_ DWORD dwMilliseconds = INTERVAL)
{
	KeybdEventDown(bVirtualKey);
	KeybdEventUp(bVirtualKey);

	if (0 < dwMilliseconds)
	{
		Sleep(dwMilliseconds);
	}
}
VOID
WINAPI
KeybdEventWithSubKey(
	_In_ BYTE bVirtualKey,
	_In_ BYTE bSubVirtualKey,
	_In_ DWORD dwMilliseconds = INTERVAL)
{
	KeybdEventDown(bSubVirtualKey);
	KeybdEventDown(bVirtualKey);
	Sleep(100);
	KeybdEventUp(bVirtualKey);
	KeybdEventUp(bSubVirtualKey);

	if (0 < dwMilliseconds)
	{
		Sleep(dwMilliseconds);
	}
}
VOID
WINAPI
KeybdEventContinued(
	_In_ BYTE bVirtualKey,
	_In_ DWORD dwMilliseconds)
{
	if (bVirtualKey == VK_LEFT || bVirtualKey == VK_RIGHT)
	{
		if (dwMilliseconds < 0)
		{
			bVirtualKey = (bVirtualKey == VK_LEFT) ? VK_RIGHT : VK_LEFT;
			dwMilliseconds *= -1;
		}
	}
	KeybdEventDown(bVirtualKey);
	if (0 < dwMilliseconds)
	{
		Sleep(dwMilliseconds);
	}
	KeybdEventUp(bVirtualKey);
}
VOID
WINAPI
KeybdEventContinuedWithSubKey(
	_In_ BYTE bVirtualKey,
	_In_ BYTE bSubVirtualKey,
	_In_ DWORD dwMilliseconds)
{
	if (bVirtualKey == VK_LEFT || bVirtualKey == VK_RIGHT)
	{
		if (dwMilliseconds < 0)
		{
			bVirtualKey = (bVirtualKey == VK_LEFT) ? VK_RIGHT : VK_LEFT;
			dwMilliseconds *= -1;
		}
	}
	KeybdEventDown(bVirtualKey);
	for (int i = 0; i < dwMilliseconds / 50; ++i)
	{
		KeybdEventDown(bSubVirtualKey);
		Sleep(50);
	}
	KeybdEventUp(bSubVirtualKey);
	KeybdEventUp(bVirtualKey);
}

int CopyTextToClipboard(LPCSTR ap_string)
{
	int string_length = strlen(ap_string) + 1;
	// Ŭ������� ���ڿ��� �����ϱ� ���Ͽ� �޸𸮸� �Ҵ��Ѵ�.
	// Ŭ�����忡�� �ڵ��� �ִ� �����̶� HeapAlloc �Լ� ����� �����ϴ�.
	HANDLE h_data = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, string_length);
	// �Ҵ�� �޸𸮿� ���ڿ��� �����ϱ� ���ؼ� ��� ������ �ּҸ� ��´�.
	char* p_data = (char*)::GlobalLock(h_data);
	if (NULL != p_data) {
		// �Ҵ�� �޸� ������ ������ ���ڿ��� �����Ѵ�.
		memcpy(p_data, ap_string, string_length);
		// ���ڿ��� �����ϱ� ���ؼ� Lock �ߴ� �޸𸮸� �����Ѵ�. 
		::GlobalUnlock(h_data);
		if (::OpenClipboard(NULL)) {
			// Ŭ�����带 ����. 
			::EmptyClipboard();
			// Ŭ�����忡 ����� ���� ���ڿ��� �����Ѵ�. 
			::SetClipboardData(CF_TEXT, h_data);
			// Ŭ������� ���ڿ��� �����Ѵ�. 
			::CloseClipboard(); // Ŭ�����带 �ݴ´�.
		}
	}
	return 0;
}
VOID
TypingMessageWithClipboard(
	LPCSTR Message)
{
	CopyTextToClipboard(Message);
	KeybdEventWithSubKey('V', VK_CONTROL);
}

/****************************************************************************
* Screen Util
****************************************************************************/



/****************************************************************************
* Log Util
****************************************************************************/

#include <ctime>
#include <mutex>

void SetConsoleTextColor(INT ColorValue)
{
	WORD wColor;
	CONSOLE_SCREEN_BUFFER_INFO ConsoleScreenBufferInfo;
	HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	if (GetConsoleScreenBufferInfo(hStdOutput, &ConsoleScreenBufferInfo))
	{
		wColor = (ConsoleScreenBufferInfo.wAttributes & 0xF0) + (ColorValue & 0x0F);
		SetConsoleTextAttribute(hStdOutput, wColor);
	}
}
#define SetConsolePosition(_x, _y)	SetConsoleCursorPosition(hStdOutput, {_x, _y})


string GetCurrentDateTimeString(
	void)
{
	time_t Now = time(nullptr);
	struct tm* TimeInfo;
	CHAR Buf[0x40];
	TimeInfo = localtime(&Now);

	strftime(Buf, sizeof(Buf), "%X", TimeInfo);
	return Buf;
}
string GetClassTypeId(
	LPCSTR lpFunctionMacro)
{
	string ClassTypeId = lpFunctionMacro;
	for (int i = ClassTypeId.length(); i >= 0; --i)
	{
		if (ClassTypeId[i] == ':')
		{
			return ClassTypeId.substr(0, i - 1);
		}
	}
}
#define __CLASSNAME__ (GetClassTypeId(__FUNCTION__) + "! ")
#define __FILENAME__ (strrchr(__FILE__,'\\')+1)

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
	FAILURE = LIGHT_RED,
};

mutex MutexLocker;
INT NumberOfLine;
LPCSTR lpFileName;
#define WriteLog(_lv, _fm, ...)	{																									\
											MutexLocker.lock();																			\
											NumberOfLine = __LINE__;															\
											lpFileName = __FILENAME__;															\
											VWriteLog(_lv, _fm, __VA_ARGS__);												\
											MutexLocker.unlock();																		\
										}
void VWriteLog(LOG_LEVEL LogLevel, LPCSTR lpFormat, ...)
{
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
	SetConsoleTextColor(2);

	va_end(Args);
}



