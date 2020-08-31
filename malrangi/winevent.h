#pragma once
#include "malrangi.h"

#define STD_MSEVT_INTERVALLE (720)
#define STD_KBEVT_INTERVALLE (640)

/****************************************************************************
* Mouse Event Util
****************************************************************************/
#define CURSOR_MOVE	(-1)
#define RIGHT_CLICK		(0)
#define LEFT_CLICK		(1)
#define DLEFT_CLICK		(2)

VOID
WINAPI
MouseEvent(
	_In_ Point Location,
	_In_ CHAR chFlag,
	_In_ DWORD dwMilliseconds = STD_MSEVT_INTERVALLE)
{
	Location.x = Location.x * 65535 / GetSystemMetrics(SM_CXSCREEN);
	Location.y = Location.y * 65535 / GetSystemMetrics(SM_CYSCREEN);

	mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, Location.x, Location.y, 0, NULL);
	switch (chFlag)
	{
	case RIGHT_CLICK:
		mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE, Location.x, Location.y, 0, NULL);
		mouse_event(MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE, Location.x, Location.y, 0, NULL);
		break;
	case LEFT_CLICK:
	case DLEFT_CLICK:
		for (int i = 0; i < chFlag; i++)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, Location.x, Location.y, 0, NULL);
			mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, Location.x, Location.y, 0, NULL);
		}
		break;
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
	_In_ DWORD dwMilliseconds = STD_KBEVT_INTERVALLE)
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
KeybdEvent(
	_In_ BYTE bVirtualKey,
	_In_ milliseconds dwMilliseconds)
{
	KeybdEventDown(bVirtualKey);
	KeybdEventUp(bVirtualKey);

	this_thread::sleep_for(dwMilliseconds);
}
VOID
WINAPI
KeybdEvent(
	_In_ initializer_list<BYTE> lVirtualKey,
	_In_ DWORD dwMilliseconds = STD_KBEVT_INTERVALLE)
{
	for each (const auto Key in lVirtualKey)
	{
		KeybdEvent(Key, dwMilliseconds);
	}
}
VOID
WINAPI
KeybdEventWithSubKey(
	_In_ BYTE bVirtualKey,
	_In_ BYTE bSubVirtualKey,
	_In_ DWORD dwMilliseconds = STD_KBEVT_INTERVALLE)
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
KeybdEventContinued(
	_In_ BYTE bVirtualKey,
	_In_ milliseconds dwMilliseconds)
{
	KeybdEventDown(bVirtualKey);
	this_thread::sleep_for(dwMilliseconds);
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
	for (int i = 0; i < dwMilliseconds / 32; ++i)
	{
		KeybdEventDown(bSubVirtualKey);
		Sleep(32);
	}
	KeybdEventUp(bSubVirtualKey);
	KeybdEventUp(bVirtualKey);
}
