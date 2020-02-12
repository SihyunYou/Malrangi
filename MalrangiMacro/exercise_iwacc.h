#pragma once
#include "exercise.h"

/****************************************************************************
* Game Window & Application Access
****************************************************************************/
namespace Exc
{	
	void UnlockSecondPassword(
		string lpPw)
	{
		for each (auto Character in lpPw)
		{
			switch (Character)
			{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '0':
				Cvw::ClickMatchedTemplate(
					Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4, 0),
					Cvw::Read(string("res\\button_") + Character + ".jpg"),
					LEFT_CLICK,
					{ 5, 5 });
				MouseEvent({ 0, 0 }, LEFT_CLICK);
				break;

			case '!':
				KeybdEventWithSubKey('1', VK_SHIFT, 300); break;
			case '@':
				KeybdEventWithSubKey('2', VK_SHIFT, 300); break;
			case '#':
				KeybdEventWithSubKey('3', VK_SHIFT, 300); break;
			case '$':
				KeybdEventWithSubKey('4', VK_SHIFT, 300); break;
			case '%':
				KeybdEventWithSubKey('5', VK_SHIFT, 300); break;
			case '^':
				KeybdEventWithSubKey('6', VK_SHIFT, 300); break;
			case '&':
				KeybdEventWithSubKey('7', VK_SHIFT, 300); break;
			case '*':
				KeybdEventWithSubKey('8', VK_SHIFT, 300); break;
			case '(':
				KeybdEventWithSubKey('9', VK_SHIFT, 300); break;
			case ')':
				KeybdEventWithSubKey('0', VK_SHIFT, 300); break;
			}
		}
		KeybdEvent(VK_RETURN);
	}
	void LoginInInitialStage(
		CONST CONF_INFO::ACCOUNT_INFO& AccountInfo)
	{
		CLIENT_ELEM::SetClientStandardPosition();

		MouseEvent({ 442, 296 }, LEFT_CLICK);
		TypingMessageWithClipboard(AccountInfo.Id.c_str());
		MouseEvent({ 442, 321 }, LEFT_CLICK);
		TypingMessageWithClipboard(AccountInfo.Password.c_str());

		KeybdEvent(VK_RETURN);
		for (int i = 0; i < AccountInfo.Sequence; i++)
		{
			KeybdEvent(VK_DOWN);
		}
		KeybdEvent(VK_RETURN);

		try
		{
			Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E2, CLIENT_ELEM::TARGETIMAGE_E2, NONWORK, 20000);
		}
		catch (TimeOutException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			KeybdEvent(VK_ESCAPE);
			MouseEvent({ 442, 296 }, LEFT_CLICK);
			for (int i = 0; i < 50; i++)
			{
				KeybdEvent(VK_BACK, 125);
			}
			TypingMessageWithClipboard(AccountInfo.Id.c_str());
			MouseEvent({ 442, 321 }, LEFT_CLICK);
			for (int i = 0; i < 50; i++)
			{
				KeybdEvent(VK_BACK, 125);
			}
			TypingMessageWithClipboard(AccountInfo.Password.c_str());

			KeybdEvent(VK_RETURN);
			for (int i = 0; i < AccountInfo.Sequence; i++)
			{
				KeybdEvent(VK_DOWN);
			}
			KeybdEvent(VK_RETURN);
		}

		KeybdEvent(VK_RETURN);
		KeybdEvent(VK_RETURN);

		try
		{
			Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E3, CLIENT_ELEM::TARGETIMAGE_E3, NONWORK, 60000);
		}
		catch (CvWrappedException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			throw;
		}
	}
	void EnterGame(
		CONST CONF_INFO::ACCOUNT_INFO& AccountInfo)
	{
		KeybdEvent(VK_RETURN, 2500);
		try
		{
			Cvw::MatchTemplate(Cvw::Capture(CLIENT_ELEM::RECT_CLIENT1), Cvw::Read("res\\button_1.jpg"));
			Exc::UnlockSecondPassword(AccountInfo.SecondPassword);
		}
		catch (MatchFailedException & cwe)
		{
			;
		}

		try
		{
			Cvw::DoUntilMatchingTemplate(
				CLIENT_ELEM::RECT_I1,
				CLIENT_ELEM::TARGETIMAGE_I1,
				[]()
				{
					CLIENT_ELEM::SetClientStandardPosition();
				},
				90000);
		}
		catch (CvWrappedException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			throw;
		}
	}
	void SelectCharacter(
		UINT Sequence)
	{
		for (int i = 0; i < 46; i++)
		{
			KeybdEvent(VK_LEFT, 120);
		}
		for (int i = 0; i < Sequence - 1; i++)
		{
			KeybdEvent(VK_RIGHT, 240);
		}
	}

	void ExitGame(
		void)
	{
		KeybdEvent(VK_ESCAPE);
		KeybdEvent(VK_UP);
		KeybdEvent(VK_RETURN);
		KeybdEvent(VK_RETURN);
		Cvw::DoUntilMatchingTemplate(
			CLIENT_ELEM::RECT_E3,
			CLIENT_ELEM::TARGETIMAGE_E3,
			[]()
			{
				CLIENT_ELEM::SetClientStandardPosition();
			},
			30000);
	}
	void Logout(
		void)
	{
		KeybdEvent(VK_ESCAPE);
		Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E2, CLIENT_ELEM::TARGETIMAGE_E2, NONWORK, 30000);

		KeybdEvent(VK_ESCAPE);
		KeybdEvent(VK_RETURN);
		Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E1, CLIENT_ELEM::TARGETIMAGE_E1, NONWORK, 30000);
	}
	void RemoveToolTip(
		void)
	{
		MouseEvent({ 902, 337 }, LEFT_CLICK);
		KeybdEvent(VK_ESCAPE);
		KeybdEvent(VK_ESCAPE);
	}
	void MakeParty(
		void)
	{
		KeybdEvent(VK_OEM_4);
		MouseEvent({ 0, 0 }, LEFT_CLICK);

		Mat SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
		static array<Mat, 2> ArrTargetImage = { Cvw::Read("res\\button_make.jpg"), Cvw::Read("res\\button_break.jpg") };
		auto TemplateInfo = Cvw::GetHighestMatchedTemplate(SourceImage, ArrTargetImage);

		if (0 == TemplateInfo.first)
		{
			MouseEvent(TemplateInfo.second.MaximumLocation, LEFT_CLICK);
			KeybdEvent(VK_RETURN);
		}
		KeybdEvent(VK_OEM_4);
	}
	void BreakParty(
		void)
	{
		KeybdEvent(VK_OEM_4);
		MouseEvent({ 0, 0 }, LEFT_CLICK);

		Mat SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
		static array<Mat, 2> ArrTargetImage = { Cvw::Read("res\\button_make.jpg"), Cvw::Read("res\\button_break.jpg") };
		auto TemplateInfo = Cvw::GetHighestMatchedTemplate(SourceImage, ArrTargetImage);

		if (1 == TemplateInfo.first)
		{
			MouseEvent(TemplateInfo.second.MaximumLocation, LEFT_CLICK);
		}
		KeybdEvent(VK_OEM_4);
	}
	void MoveServerNext(
		DWORD dwMilliSeconds = 5000)
	{
		KeybdEvent(VK_ESCAPE, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_RIGHT, 400);
		KeybdEvent(VK_RETURN, 400);
		Sleep(dwMilliSeconds);
	}
	void MoveServerBack(
		DWORD dwMilliSeconds = 5000)
	{
		KeybdEvent(VK_ESCAPE, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_LEFT, 400);
		KeybdEvent(VK_RETURN, 400);
		Sleep(dwMilliSeconds);
	}
}
namespace EXC = Exc;