#pragma once
#include "low_util.h"

namespace Tsw
{
	string ImageToString(Mat SourceImage)
	{
#define IMAGENAME	"tmp\\temp_img.jpg"
#define OUTPUTBASE	"tmp\\temp_result"

		imwrite(IMAGENAME, SourceImage);

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = TEXT("tesseract.exe");
		ShExecInfo.lpParameters = TEXT(IMAGENAME " " OUTPUTBASE " -l eng" );
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		CloseHandle(ShExecInfo.hProcess);

		ifstream File;
		CHAR Line[0x100];

		File.open(OUTPUTBASE ".txt");
		if (File.is_open())
		{
			File.getline(Line, sizeof(Line));
			return Line;
		}
		return "";

#undef IMAGENAME
#undef OUTPUTBASE	
	}
};