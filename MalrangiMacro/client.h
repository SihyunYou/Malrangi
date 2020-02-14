/****************************************************************************
*
* exercise.h -- Macro Applications Optimized to Maple
*
****************************************************************************/

#pragma once
#include "cvwrap.h"
#include "conf.h"


/****************************************************************************
* Information of Client
****************************************************************************/
typedef struct _CLIENT_ELEM
{
#define HWND_MAPLESTORY (::FindWindow(NULL, TEXT("MapleStory")))
	static void SET_CLIENT_STDPOS(void);
	static CONST RECT RECT_CLIENT1;
	static CONST RECT RECT_CLIENT4;
	static CONST Mat TARGETIMAGE_E1;
	static CONST Mat TARGETIMAGE_E2;
	static CONST Mat TARGETIMAGE_E3;
	static CONST Mat TARGETIMAGE_I1;
	static CONST RECT RECT_E1;
	static CONST RECT RECT_E2;
	static CONST RECT RECT_E3;
	static CONST RECT RECT_I1;
}CLIENT_ELEM;
void CLIENT_ELEM::SET_CLIENT_STDPOS(void)
{
	SetWindowPos(GetConsoleWindow(),
		0,
		1362, 0,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(HWND_MAPLESTORY,
		HWND_TOP,
		-3, -26,
		0, 0,
		SWP_NOSIZE);
}
CONST RECT CLIENT_ELEM::RECT_CLIENT1 = { 0, 0, 800, 600 };
CONST RECT CLIENT_ELEM::RECT_CLIENT4 = { 0, 0, 1366, 768 };
CONST Mat CLIENT_ELEM::TARGETIMAGE_E1 = Cvw::Read("res\\initial_logo.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_E2 = Cvw::Read("res\\world-channel_select.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_E3 = Cvw::Read("res\\character_select.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_I1 = Cvw::Read("res\\button_mileage.jpg");
CONST RECT CLIENT_ELEM::RECT_E1 = { 700, 0, 850, 100 };
CONST RECT CLIENT_ELEM::RECT_E2 = { 30, 0, 190, 50 };
CONST RECT CLIENT_ELEM::RECT_E3 = { 0, 0, 200, 70 };
CONST RECT CLIENT_ELEM::RECT_I1 = { 0, 280, 100, 500 };





/****************************************************************************
* Character's Act Control
****************************************************************************/
namespace Exc
{
	enum class JUMP_TYPE
	{
		CYGNUS,
		DEMON,
		NOVA,
		V_MATRIX,
		ZERO,
		WIZARD,
	};
	void Jump(
		JUMP_TYPE JumpType,
		DWORD MillisecondsRestTime = 1000)
	{
		switch (JumpType)
		{
		case JUMP_TYPE::DEMON:
			KeybdEvent(VK_MENU, 60);
			KeybdEvent(VK_UP, 60);
			KeybdEvent(VK_UP, 2500);
			break;

		case JUMP_TYPE::CYGNUS:
			KeybdEvent(VK_MENU, 60);

			KeybdEventDown(VK_UP);
			KeybdEvent(VK_MENU, 60);
			KeybdEventUp(VK_UP);
			break;

		case JUMP_TYPE::NOVA:
			KeybdEvent('C', 2000);
			break;

		case JUMP_TYPE::V_MATRIX:
			KeybdEvent('L', 180);
			break;

		case JUMP_TYPE::ZERO:
			KeybdEventDown(VK_UP);
			KeybdEvent('F', 100);
			KeybdEventUp(VK_UP);
			break;

		case JUMP_TYPE::WIZARD:
			KeybdEventDown(VK_UP);
			KeybdEvent(VK_SHIFT, 100);
			KeybdEventUp(VK_UP);
			break;
		}
		Sleep(MillisecondsRestTime);
	}
	void DownJump(
		DWORD MilliSecondsRestTime = 1000)
	{
		keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
		KeybdEvent(VK_MENU, MilliSecondsRestTime);
		keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), KEYEVENTF_KEYUP, 0);
	}


	class MinimapRecognizer
	{
	public:
		class CharacterNotFoundException : public CvWrappedException
		{
		public:
			CharacterNotFoundException::CharacterNotFoundException(void) :
				CvWrappedException(__CLASSNAME__ + "A specified object cannot be recognized in the minimap.\n") {}
			virtual const char* CharacterNotFoundException::what(void)
			{
				return Message.c_str();
			}
		};

	public:
		MinimapRecognizer::MinimapRecognizer(
			RECT RectMinimap) :
			RectMinimap(RectMinimap)
		{
			;
		}
		MinimapRecognizer operator=(
			RECT RectMinimap)
		{
			return MinimapRecognizer(RectMinimap);
		}

	protected:
		enum class OBJECT
		{
			YELLOW,
			RED,
		};

	public:
		void MoveToRelativeCriteria(
			INT MinimapRelativeCriteria)
		{
			auto GetCurrentMiniMapPosition = [this](MinimapRecognizer::OBJECT ObjectToGet) ->INT
			{
				try
				{
					Mat SourceMiniMapImage = Cvw::Capture(RectMinimap, IMREAD_COLOR);
					//Cvw::Show(SourceMiniMapImage);
					Mat Mask;
					if (OBJECT::YELLOW == ObjectToGet)
					{
						Cvw::InRange(SourceMiniMapImage, Scalar(20, 235, 245), Scalar(40, 245, 255), Mask);
					}
					//Cvw::Show(Mask);
					for (int y = 0; y < Mask.rows; ++y)
					{
						for (int x = 0; x < Mask.cols; ++x)
						{
							if (Mask.at<BYTE>(y, x) == 255)
							{
								return x;
							}
						}
					}
				}
				catch (EmptyMatException & cwexception)
				{
					throw;
				}
				catch (IntegerDivisionByZeroException & cwexception)
				{
					throw;
				}

				throw MinimapRecognizer::CharacterNotFoundException();
			};
			INT RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria;

			if (RelativeDistance < 0)
			{

				KeybdEventDown(VK_RIGHT);
				while (RelativeDistance <= 0)
				{
					std::cout << GetCurrentMiniMapPosition(OBJECT::YELLOW) << endl;
					RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria + 2;
				}
				KeybdEventUp(VK_RIGHT);
			}
			else if (RelativeDistance > 0)
			{
				KeybdEventDown(VK_LEFT);
				while (RelativeDistance >= 0)
				{
					std::cout << GetCurrentMiniMapPosition(OBJECT::YELLOW) << endl;
					RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria - 2;
				}
				KeybdEventUp(VK_LEFT);
			}
		}

	private:
		RECT RectMinimap;
	};
}


/****************************************************************************
* External Game Client Control
****************************************************************************/
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
					CLIENT_ELEM::SET_CLIENT_STDPOS();
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


/****************************************************************************
* Internal Game Window Control
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
		CONST CONF_INFO::ACCOUNT_INFO& AccountInfo,
		CONST CONF_INFO::ACCOUNT_INFO::MAPLEID_INFO& MapleIdInfo)
	{
		auto LoginNexonId = [&AccountInfo](BOOL IsBlank) -> void
		{
			MouseEvent({ 300, 296 }, LEFT_CLICK);
			TypingMessageWithClipboard(AccountInfo.Id.c_str());
			if (!IsBlank)
			{
				for (int i = 0; i < 20; i++)
				{
					KeybdEvent(VK_BACK, 250);
				}
				for (int i = 0; i < 40; i++)
				{
					KeybdEvent(VK_DELETE, 250);
				}
			}

			MouseEvent({ 300, 321 }, LEFT_CLICK);
			TypingMessageWithClipboard(AccountInfo.Password.c_str());
			if (!IsBlank)
			{
				for (int i = 0; i < 20; i++)
				{
					KeybdEvent(VK_BACK, 250);
				}
				for (int i = 0; i < 40; i++)
				{
					KeybdEvent(VK_DELETE, 250);
				}
			}

			KeybdEvent(VK_RETURN);
		};
		auto SelectMapldId = [&AccountInfo, &MapleIdInfo](void) -> void
		{
			DWORD SeqId = 0;
			for each (auto & MapleId in AccountInfo.VecMapleId)
			{
				if (MapleId.Id == MapleIdInfo.Id)
				{
					break;
				}
				++SeqId;
			}
			
			for (int i = 0; i < SeqId + 1; i++)
			{
				KeybdEvent(VK_DOWN);
			}
			KeybdEvent(VK_RETURN);
		};
		
		CLIENT_ELEM::SET_CLIENT_STDPOS();
		LoginNexonId(TRUE);
		SelectMapldId();

		try
		{
			Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E2, CLIENT_ELEM::TARGETIMAGE_E2, NONWORK, 20000);
		}
		catch (TimeOutException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			KeybdEvent(VK_ESCAPE);
			LoginNexonId(FALSE);
			SelectMapldId();

			try
			{
				Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_E2, CLIENT_ELEM::TARGETIMAGE_E2, NONWORK, 20000);
			}
			catch (TimeOutException & cwe)
			{
				throw;
			}
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
		CONST CONF_INFO::ACCOUNT_INFO::MAPLEID_INFO& MapleIdInfo)
	{
		KeybdEvent(VK_RETURN, 2500);
		try
		{
			Cvw::MatchTemplate(Cvw::Capture(CLIENT_ELEM::RECT_CLIENT1), Cvw::Read("res\\button_1.jpg"));
			Exc::UnlockSecondPassword(MapleIdInfo.SecondPassword);
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
					CLIENT_ELEM::SET_CLIENT_STDPOS();
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

	void ResetIngameWindow(
		void)
	{
		for (int i = 0; i < 3; i++)
		{
			KeybdEvent(VK_ESCAPE);
		}
		MouseEvent({ 0, 0 }, LEFT_CLICK);
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
				CLIENT_ELEM::SET_CLIENT_STDPOS();
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

