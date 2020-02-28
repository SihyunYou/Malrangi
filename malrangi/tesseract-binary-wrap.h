#pragma once
#include "malrangi.h"

class MyException : public MalrangiException
{
public:
	MyException(void) :
		MalrangiException(__CLASSNAME__) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};


namespace Tsw
{
	template <class F>
	string TranslateToString(const string& ImagePath, F StrProcRoutine = [](const string&) -> string{})
	{
#define OUTPUTBASE	TMP_DIR "res"
		CreateDirectoryA("tmp", NULL);

		SHELLEXECUTEINFOA ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = "tesseract.exe";
		ShExecInfo.lpParameters = (ImagePath + " " OUTPUTBASE " -l eng").c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;

		ShellExecuteExA(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		CloseHandle(ShExecInfo.hProcess);

		ifstream File(OUTPUTBASE ".txt");
		if (File.is_open())
		{
			CHAR Line[0x100];
			File.getline(Line, sizeof(Line));

			return StrProcRoutine(Line);
		}
		return "";

#undef OUTPUTBASE	
	}
};