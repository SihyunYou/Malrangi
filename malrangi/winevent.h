#pragma once
#include <Windows.h>

#define INTERVAL_MOUSEEVENT		(0x400)
#define INTERVAL						(750)

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
	_In_ DWORD dx,
	_In_ DWORD dy,
	_In_ CHAR chFlag,
	_In_ DWORD dwMilliseconds = INTERVAL_MOUSEEVENT)
{
	dx = dx * 65535 / GetSystemMetrics(SM_CXSCREEN);
	dy = dy * 65535 / GetSystemMetrics(SM_CYSCREEN);

	mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, dx, dy, 0, NULL);
	switch (chFlag)
	{
	case RIGHT_CLICK:
		mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_ABSOLUTE, dx, dy, 0, NULL);
		mouse_event(MOUSEEVENTF_RIGHTUP | MOUSEEVENTF_ABSOLUTE, dx, dy, 0, NULL);
		break;
	case LEFT_CLICK:
	case DLEFT_CLICK:
		for (int i = 0; i < chFlag; i++)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_ABSOLUTE, dx, dy, 0, NULL);
			mouse_event(MOUSEEVENTF_LEFTUP | MOUSEEVENTF_ABSOLUTE, dx, dy, 0, NULL);
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
	for (int i = 0; i < dwMilliseconds / 32; ++i)
	{
		KeybdEventDown(bSubVirtualKey);
		Sleep(32);
	}
	KeybdEventUp(bSubVirtualKey);
	KeybdEventUp(bVirtualKey);
}
