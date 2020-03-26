#pragma once
#include "malrangi.h"
#include "winevent.h"

class EmptyMatException : public MalrangiException
{
public:
	EmptyMatException() :
		MalrangiException(__CLASSNAME__) {}
	EmptyMatException(string FileName) :
		MalrangiException(__CLASSNAME__ + "! " + FileName){}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};

struct VALLOC
{
	VALLOC() : Value(0), Location(0, 0) {}
	VALLOC(double Value, Point Location) : Value(Value), Location(Location) {}

	double Value;
	Point Location;
};

/****************************************************************************
* Mat I/O
****************************************************************************/
inline void Show(
	Mat Image)
{
	imshow("Image", Image);
	waitKey();
}

inline Mat Read(
	const String& FileName,
	INT Flag = 0)
{
	Mat Image = imread(FileName, Flag);
	if (Image.empty())
	{
		throw EmptyMatException(FileName);
	}
	return Image;
}

Mat Capture(
	const RECT Area,
	INT Flag = IMREAD_GRAYSCALE)
{
	HBITMAP hBitmap;
	Mat BitmapImage;
	HDC hdcSys = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcSys);
	LPVOID ptrBitmapPixels;
	BITMAPINFO BitmapInfo;
	HDC hdc;
	INT cx = Area.right - Area.left;
	INT cy = Area.bottom - Area.top;

	ZeroMemory(&BitmapInfo, sizeof(BITMAPINFO));
	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = cx;
	BitmapInfo.bmiHeader.biHeight = -cy;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	hdc = GetDC(NULL);
	hBitmap = CreateDIBSection(hdc, &BitmapInfo, DIB_RGB_COLORS, &ptrBitmapPixels, NULL, 0);

	SelectObject(hdcMem, hBitmap);
	BitmapImage = Mat(cy, cx, CV_8UC4, ptrBitmapPixels);
	BitBlt(hdcMem, 0, 0, cx, cy, hdcSys, Area.left, Area.top, SRCCOPY);

	Mat ConvertedImage;
	switch (Flag)
	{
	case 0:
		cvtColor(BitmapImage, ConvertedImage, COLOR_BGRA2GRAY);
		break;

	case 1:
		cvtColor(BitmapImage, ConvertedImage, COLOR_BGRA2BGR);
		break;
	}

	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdc);
	ReleaseDC(NULL, hdcSys);

	return ConvertedImage;
}

#define INT_TO_PNG(x) (to_string(x) + ".png")
void Write(
	const string& FilePath,
	const Mat& Image)
{
	imwrite(FilePath, Image);
}


/****************************************************************************
* Mat Template Matching
****************************************************************************/
bool MatchTemplate(
	Mat SourceImage,
	Mat TargetImage,
	VALLOC* MatchInfo = nullptr,
	double Threshold = 0.9)
{
	Mat ResultImage;
	double Value;

	cv::matchTemplate(SourceImage, TargetImage, ResultImage, TM_CCOEFF_NORMED);
	cv::minMaxLoc(ResultImage,
		nullptr,
		MatchInfo == nullptr ? &Value : &MatchInfo->Value,
		nullptr,
		MatchInfo == nullptr ? nullptr : &MatchInfo->Location);

	return (nullptr == MatchInfo ? Value : MatchInfo->Value) > Threshold;
}
template <class T_DUR = seconds>
bool WaitUntilMatchingTemplate(
	const RECT& Area,
	Mat TargetImage,
	T_DUR Duration,
	int IdleTime = 256,
	double Threshold = 0.9)
{
	const auto StartTime = system_clock::now();
	if (IdleTime < 1)
	{
		IdleTime = 1;
	}

	do
	{
		Sleep(IdleTime);
		if (MatchTemplate(Capture(Area), TargetImage, nullptr, Threshold))
		{
			return true;
		}
	} while (duration_cast<T_DUR>(system_clock::now() - StartTime) < Duration);

	return false;
}
template <class T_LAMBDA, class T_DUR = seconds>
bool DoUntilMatchingTemplate(
	const RECT& Area,
	Mat TargetImage,
	T_LAMBDA Func,
	T_DUR Duration,
	int IdleTime = 256,
	double Threshold = 0.9)
{
	const auto StartTime = system_clock::now();
	if (IdleTime < 1)
	{
		IdleTime = 1;
	}

	do
	{
		Func();
		Sleep(IdleTime);
		if (MatchTemplate(Capture(Area), TargetImage, nullptr, Threshold))
		{
			return true;
		}
	} while (duration_cast<T_DUR>(system_clock::now() - StartTime) < Duration);

	return false;
}
template<SIZE_T ArrSize>
auto GetHighestMatchedTemplate(
	Mat SourceImage,
	const array<Mat, ArrSize>& ArrTargetImage)
{
	VALLOC HighestValloc;
	SIZE_T CountArrTargetImage = 0;
	SIZE_T SeqHighestMatchedTemplate = 0;

	for each (auto & TargetImage in ArrTargetImage)
	{
		VALLOC CurrentValloc;

		MatchTemplate(SourceImage, TargetImage, &CurrentValloc);
		if (CurrentValloc.Value > HighestValloc.Value)
		{
			HighestValloc = CurrentValloc;
			SeqHighestMatchedTemplate = CountArrTargetImage;
		}

		++CountArrTargetImage;
	}

	return make_tuple(SeqHighestMatchedTemplate, HighestValloc.Value, HighestValloc.Location);
}


/****************************************************************************
* Image Processing
****************************************************************************/
inline bool InRange(
	InputArray src,
	InputArray lowerb,
	InputArray upperb,
	OutputArray dst)
{
	__try
	{
		inRange(src, lowerb, upperb, dst);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
}