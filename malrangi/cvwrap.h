#pragma once
#include "malrangi.h"
#include "winevent.h"

class IntegerDivisionByZeroException : public MalrangiException
{
public:
	IntegerDivisionByZeroException(void) :
		MalrangiException(__CLASSNAME__) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};
class OutOfIndexException : public MalrangiException
{
public:
	OutOfIndexException(void) :
		MalrangiException(__CLASSNAME__) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};
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
class MatchTimeoutException : public MalrangiException
{
public:
	MatchTimeoutException(void) :
		MalrangiException(__CLASSNAME__) {}
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
	bool IsMatched;
};

/****************************************************************************
* Mat I/O
****************************************************************************/
namespace Cvw
{
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
}


/****************************************************************************
* Mat Template Matching
****************************************************************************/
namespace Cvw
{
	VALLOC MatchTemplate(
		Mat SourceImage,
		Mat TargetImage,
		double Threshold = 0.9)
	{
		Mat ResultImage;
		VALLOC MatchInfo;

		cv::matchTemplate(SourceImage, TargetImage, ResultImage, TM_CCOEFF_NORMED);
		cv::minMaxLoc(ResultImage,
			nullptr,
			&MatchInfo.Value,
			nullptr,
			&MatchInfo.Location);
		MatchInfo.IsMatched = (MatchInfo.Value < Threshold) ? false : true;

		return MatchInfo;
	}
	VALLOC ClickMatchedTemplate(
		Mat SourceImage,
		Mat TargetImage,
		CHAR Flag,
		Point AdjustmentValue = { 0, 0 },
		DWORD MilliSeconds = INTERVAL_MOUSEEVENT,
		DOUBLE Threshold = 0.9)
	{
		VALLOC MatchInfo = MatchTemplate(SourceImage, TargetImage, Threshold);
		if (MatchInfo.IsMatched)
		{
			Point PositionToClick = MatchInfo.Location + AdjustmentValue;
			MouseEvent(PositionToClick.x, PositionToClick.y, Flag, MilliSeconds);
		}

		return MatchInfo;
	}
#define NONWORK					[](void) -> void {;}
	template <class T_LAMBDA, class T_DUR = seconds>
	void DoUntilMatchingTemplate(
		RECT Area,
		Mat TargetImage,
		T_LAMBDA Func,
		T_DUR Duration,
		int IdleTime = 256,
		double Threshold = 0.9)
	{
		const auto StartTime = system_clock::now();
		do
		{
			Func();
			Sleep(IdleTime);

			if (Cvw::MatchTemplate(Cvw::Capture(Area), TargetImage, Threshold).IsMatched)
			{
				return;
			}
		} while (duration_cast<T_DUR>(system_clock::now() - StartTime) < Duration);

		throw MatchTimeoutException();
	}
	template<SIZE_T ArrSize>
	pair<SIZE_T, VALLOC> GetHighestMatchedTemplate(
		Mat SourceImage,
		const array<Mat, ArrSize>& ArrTargetImage)
	{
		VALLOC HighestValloc;
		SIZE_T CountArrTargetImage = 0;
		SIZE_T SeqHighestMatchedTemplate = 0;

		for each (auto & TargetImage in ArrTargetImage)
		{
			VALLOC CurrentValloc = Cvw::MatchTemplate(SourceImage, TargetImage, 0);
			if (CurrentValloc.Value > HighestValloc.Value)
			{
				HighestValloc = CurrentValloc;
				SeqHighestMatchedTemplate = CountArrTargetImage;
			}

			++CountArrTargetImage;
		}

		return { SeqHighestMatchedTemplate, HighestValloc };
	}
}


/****************************************************************************
* Image Processing
****************************************************************************/
namespace Cvw
{
	inline void InRange(
		InputArray src,
		InputArray lowerb,
		InputArray upperb,
		OutputArray dst)
	{
		__try
		{
			inRange(src, lowerb, upperb, dst);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			throw IntegerDivisionByZeroException();
		}
	}

	void ToBinary(Mat& SourceImage, INT Threshold)
	{
		Mat BinaryImage;
		threshold(SourceImage, BinaryImage, Threshold, 255, 0);
		SourceImage = BinaryImage;
	}
	void Magnify(Mat& SourceImage, DOUBLE Percentage)
	{
		Mat Destination;
		cv::resize(SourceImage, Destination, cv::Size(SourceImage.cols * Percentage, SourceImage.rows * Percentage), 0, 0, INTER_CUBIC);

		SourceImage = Destination;
	}

}
