#pragma once
#include "low_util.h"

class TsWrappedException : public std::exception
{
public:
	TsWrappedException::TsWrappedException(void) :
		Message(__CLASSNAME__ + "\n") {}
	explicit TsWrappedException::TsWrappedException(const char* Message) :
		Message(Message) {}
	explicit TsWrappedException::TsWrappedException(string Message) :
		Message(Message) {}
	virtual TsWrappedException::~TsWrappedException() throw () {}
	virtual const char* TsWrappedException::what(void) const throw()
	{
		return Message.c_str();
	}

protected:
	std::string Message;
};

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
		ShExecInfo.lpParameters = TEXT(IMAGENAME " " OUTPUTBASE " -l eng --oem 0 --psm 7" );
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
	int levenshtein(string input1, string input2) {
		int dist[100][100];
		for (int i = 1; i <= input1.length(); i++)
			dist[i][0] = i;
		for (int j = 1; j <= input2.length(); j++)
			dist[0][j] = j;

		for (int j = 1; j <= input2.length(); j++) {
			for (int i = 1; i <= input1.length(); i++) {
				if (input1[i - 1] == input2[j - 1]) dist[i][j] = dist[i - 1][j - 1];
				else dist[i][j] = min(dist[i - 1][j - 1] + 1, min(dist[i][j - 1] + 1, dist[i - 1][j] + 1));
			}
		}

		return dist[input1.length()][input2.length()];
	}

	DOUBLE GetSimilarityBetweenString(string s1, string s2)
	{
		CONST DOUBLE MAX_STR = MAX(s1.length(), s2.length());
		return (1 - (levenshtein(s1, s2) / MAX_STR));
	}
};