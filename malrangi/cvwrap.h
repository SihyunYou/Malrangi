#pragma once
#include "low_util.h"

class CvWrappedException : public std::exception
{
public:
	CvWrappedException::CvWrappedException(void) :
		Message(__CLASSNAME__) {}
	explicit CvWrappedException::CvWrappedException(const char* Message) :
		Message(Message) {}
	explicit CvWrappedException::CvWrappedException(string Message) :
		Message(Message) {}
	virtual CvWrappedException::~CvWrappedException() throw () {}
	virtual const char* CvWrappedException::what(void) const throw()
	{
		return Message.c_str();
	}

protected:
	std::string Message;
};

class IntegerDivisionByZeroException : public CvWrappedException
{
public:
	IntegerDivisionByZeroException::IntegerDivisionByZeroException(void) :
		CvWrappedException(__CLASSNAME__ + "Integer division by zero.") {}
	virtual const char* IntegerDivisionByZeroException::what(void) const throw()
	{
		return Message.c_str();
	}
};
class OutOfIndexException : public CvWrappedException
{
public:
	OutOfIndexException::OutOfIndexException(void) :
		CvWrappedException(__CLASSNAME__ + "Out of index.") {}
	virtual const char* OutOfIndexException::what(void) const throw()
	{
		return Message.c_str();
	}
};
class EmptyMatException : public CvWrappedException
{
public:
	EmptyMatException::EmptyMatException(void) :
		CvWrappedException(__CLASSNAME__ + "Mat image is empty. It seems that imread function fails to " \
			"read a file or your handle does not allow you to get bitmap pointer because of a memory leak.") {}
	virtual const char* EmptyMatException::what(void) const throw()
	{
		return Message.c_str();
	}
};
class MatchFailedException : public CvWrappedException
{
public:
	MatchFailedException(void) :
		CvWrappedException()
	{
		;
	}
	MatchFailedException::MatchFailedException(string MatchFileName) :
		CvWrappedException(__CLASSNAME__ + "Template matching is failed. File to match : " + MatchFileName + "\n") 
	{
		;
	}
	virtual const char* MatchFailedException::what(void) const throw()
	{
		return Message.c_str();
	}
};

typedef struct _Valloc
{
	_Valloc::_Valloc(void) :
		MinimumValue(0), MaximumValue(0), MinimumLocation(0, 0), MaximumLocation(0, 0) {}
	_Valloc::_Valloc(DOUBLE MinimumValue, DOUBLE MaximumValue, cv::Point MinimumLocation, cv::Point MaximumLocation) :
		MinimumValue(this->MinimumValue),
		MaximumValue(this->MaximumValue),
		MinimumLocation(this->MinimumLocation),
		MaximumLocation(this->MaximumLocation) {}

	DOUBLE MinimumValue;
	DOUBLE MaximumValue;
	Point MinimumLocation;
	Point MaximumLocation;
	BOOL IsMatched;
} Valloc;


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
			throw EmptyMatException();
		}
		return Image;
	}
	Mat Capture(
		CONST RECT Area,
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
}


/****************************************************************************
* Mat Template Matching
****************************************************************************/
namespace Cvw
{
	__forceinline Valloc MatchTemplate(
		Mat SourceImage,
		Mat TargetImage,
		DOUBLE Threshold = 0.9)
	{
		Mat ResultImage;
		Valloc MatchInfo;
		cv::matchTemplate(SourceImage, TargetImage, ResultImage, TM_CCOEFF_NORMED);
		cv::minMaxLoc(ResultImage,
			&MatchInfo.MinimumValue,
			&MatchInfo.MaximumValue,
			&MatchInfo.MinimumLocation,
			&MatchInfo.MaximumLocation);
	//	Cvw::Show(SourceImage);
	//	cout << MatchInfo.MaximumValue << endl;
		if (MatchInfo.MaximumValue < Threshold)
		{
			throw MatchFailedException();
		}
		return MatchInfo;
	}
	__forceinline Point ClickMatchedTemplate(
		Mat SourceImage,
		Mat TargetImage,
		CHAR Flag,
		Point AdjustmentValue = { 0, 0 },
		DWORD MilliSeconds = INTERVAL_MOUSEEVENT,
		DOUBLE Threshold = 0.9)
	{
		Point Location = MatchTemplate(SourceImage, TargetImage, Threshold).MaximumLocation;
		MouseEvent(Location + AdjustmentValue, Flag, MilliSeconds);

		return Location;
	}
#define NONWORK					[](void) -> void {;}
	template <class LAMBDA>
	__forceinline Point DoUntilMatchingTemplate(
		RECT Area,
		Mat TargetImage,
		LAMBDA Func,
		INT LimitTime,
		INT IdleTime = 256,
		DOUBLE Threshold = 0.9)
	{
		INT SumOfIdleTime = 0;
		while (SumOfIdleTime < LimitTime)
		{
			try
			{
				Point LocationTemplate = Cvw::MatchTemplate(Cvw::Capture(Area), TargetImage, Threshold).MaximumLocation;
				Func();
				Sleep(IdleTime);

				return LocationTemplate;
			}
			catch (MatchFailedException & CvwException)
			{
				Func();
				Sleep(IdleTime);
			}

			SumOfIdleTime += IdleTime;
		}

		throw MatchFailedException();
	}
	template<SIZE_T ArrSize>
	pair<SIZE_T, Valloc> GetHighestMatchedTemplate(
		Mat SourceImage,
		array<Mat, ArrSize>& ArrTargetImage)
	{
		Valloc HighestValloc;
		SIZE_T CountArrTargetImage = 0;
		SIZE_T SeqHighestMatchedTemplate = 0;

		for each (auto & TargetImage in ArrTargetImage)
		{
			Valloc CurrentValloc = Cvw::MatchTemplate(SourceImage, TargetImage, 0);
			if (CurrentValloc.MaximumValue > HighestValloc.MaximumValue)
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
