#pragma once
#include "exercise.h"

namespace Exc
{
	void BootClient(
		void)
	{
		ShellExecute(NULL, TEXT("open"), TEXT("C:\\Nexon\\Maple\\MapleStory.exe"), TEXT("GameLaunching"), NULL, SW_SHOW);
		try
		{
			Cvw::DoUntilMatchingTemplate(
				CLIENT_ELEM::RECT_E1,
				CLIENT_ELEM::TARGETIMAGE_E1,
				[]()
				{
					CLIENT_ELEM::SetClientStandardPosition();
				},
				180000);
		}
		catch (CvWrappedException & cwe)
		{
			throw;
		}
	}
	void TerminateClient(
		void)
	{
		DWORD ProcessId;
		GetWindowThreadProcessId(HWND_MAPLESTORY, &ProcessId);
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessId);
		if (hProcess)
		{
			TerminateProcess(hProcess, -1);
			CloseHandle(hProcess);
		}
	}
}