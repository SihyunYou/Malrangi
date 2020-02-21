/****************************************************************************
*
* exercise.h -- Macro Applications Optimized to Maple
*
****************************************************************************/

#pragma once
#include "cvwrap.h"
#include "user.h"

#define WAIT_UNTIL_STABLE Sleep(512)

class ClientApi
{
public:
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

public:
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


	class ClientException : public std::exception
	{
	public:
		ClientException(void) :
			Message(__CLASSNAME__) {}
		explicit ClientException(const char* Message) :
			Message(Message) {}
		explicit ClientException(string Message) :
			Message(Message) {}
		virtual ~ClientException() throw () {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}

	protected:
		std::string Message;
	};


	/****************************************************************************
	* Exceptions
	****************************************************************************/
	class BootFailedException : public ClientException
	{
	public:
		BootFailedException(void) :
			ClientException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class ServerDisconnectedException : public ClientException
	{
	public:
		ServerDisconnectedException(void) :
			ClientException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class GameEntryException : public ClientException
	{
	public:
		GameEntryException(void) :
			ClientException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class GameExitException : public ClientException
	{
	public:
		GameExitException(void) :
			ClientException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class UnhandledException : public ClientException
	{
	public:
		UnhandledException(void) :
			ClientException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};



	/****************************************************************************
	* External Client Control
	****************************************************************************/
	static void BootClient(void);
	static void Login(
		const CONF_INFO::NEXONAC_INFO&,
		const CONF_INFO::NEXONAC_INFO::MAPLEID_INFO&);
	static void SelectServer(
		const CONF_INFO::NEXONAC_INFO::MAPLEID_INFO::SERVER_INFO&);
	static void SelectCharacter(unsigned int);
	static void UnlockSecondPassword(
		const string&);
	static void ExitCharacterWindow(void);
	static void Logout(void);
	static void TerminateClient(void);


	/****************************************************************************
	* Internal Game Window Control
	****************************************************************************/
	static void EnterGame(
		const CONF_INFO::NEXONAC_INFO::MAPLEID_INFO&);
	enum class JUMP_T
	{
		CYGNUS,
		DEMON,
		NOVA,
		V_MATRIX,
		ZERO,
		WIZARD,
	};
	static void Jump(
		JUMP_T,
		DWORD);
	static void DownJump(
		DWORD);
	static void MakeParty(void);
	static void BreakParty(void);
	static void MoveServer(bool);
	static void RemoveAllIngameWindows(void);
	static void ExitGame(void);
};

void ClientApi::SET_CLIENT_STDPOS(void)
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
CONST RECT ClientApi::RECT_CLIENT1 = { 0, 0, 800, 600 };
CONST RECT ClientApi::RECT_CLIENT4 = { 0, 0, 1366, 768 };
CONST Mat ClientApi::TARGETIMAGE_E1 = Cvw::Read(TARGET_DIR "initial_logo.jpg");
CONST Mat ClientApi::TARGETIMAGE_E2 = Cvw::Read(TARGET_DIR "world-channel_select.jpg");
CONST Mat ClientApi::TARGETIMAGE_E3 = Cvw::Read(TARGET_DIR "character_select.jpg");
CONST Mat ClientApi::TARGETIMAGE_I1 = Cvw::Read(TARGET_DIR "button_mileage.jpg");
CONST RECT ClientApi::RECT_E1 = { 700, 0, 850, 100 };
CONST RECT ClientApi::RECT_E2 = { 30, 0, 190, 50 };
CONST RECT ClientApi::RECT_E3 = { 0, 0, 200, 70 };
CONST RECT ClientApi::RECT_I1 = { 0, 280, 100, 500 };

void ClientApi::BootClient(
	void)
{
	ShellExecute(NULL, TEXT("open"), TEXT("C:\\Nexon\\Maple\\MapleStory.exe"), TEXT("GameLaunching"), NULL, SW_SHOW);
	try
	{
		Cvw::DoUntilMatchingTemplate(
			ClientApi::RECT_E1,
			ClientApi::TARGETIMAGE_E1,
			[]()
			{
				ClientApi::SET_CLIENT_STDPOS();
			},
			180000);
	}
	catch (MatchFailedException & cwe)
	{
		throw ClientApi::ClientException("BootFailedException");
	}
}
void ClientApi::TerminateClient(
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
void ClientApi::Login(
	CONST CONF_INFO::NEXONAC_INFO& AccountInfo,
	CONST CONF_INFO::NEXONAC_INFO::MAPLEID_INFO& MapleIdInfo)
{
	static Mat TargetImageWindowMapleId = Cvw::Read(TARGET_DIR "window_mapleid.jpg");
	auto NexonLogin = [&](void) -> void
	{
		auto WriteString = [](const string& s) -> void
		{
			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] >= 'a' && s[i] <= 'z') KeybdEvent(s[i] - 32, 0x40);
				else if ('!' == s[i]) KeybdEventWithSubKey('1', VK_SHIFT, 0x40);
				else if ('@' == s[i]) KeybdEventWithSubKey('2', VK_SHIFT, 0x40);
				else if ('.' == s[i]) KeybdEvent(VK_OEM_PERIOD, 0x40);
				else KeybdEvent(s[i], 0x40);
			}
		};

		WriteString(AccountInfo.Id);
		KeybdEvent(VK_TAB);
		WriteString(AccountInfo.Password);
		KeybdEvent(VK_RETURN, 0x400);
	};

	ClientApi::SET_CLIENT_STDPOS();
	NexonLogin();
	try
	{
		Cvw::MatchTemplate(Cvw::Capture(ClientApi::RECT_CLIENT1), TargetImageWindowMapleId);
	}
	catch (MatchFailedException& ce)
	{
		KeybdEvent(VK_RETURN);
		MouseEvent({ 334, 256 }, LEFT_CLICK);
		MouseEvent({ 452, 256 }, LEFT_CLICK);
		NexonLogin();
		try
		{
			Cvw::MatchTemplate(Cvw::Capture(ClientApi::RECT_CLIENT1), TargetImageWindowMapleId);
		}
		catch (MatchFailedException & ce)
		{
			throw ClientApi::ClientException("NexonLoginFailedException");
		}
	}

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

	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_E2, ClientApi::TARGETIMAGE_E2, NONWORK, 15000);
	}
	catch (MatchFailedException & cwe)
	{
		throw ClientApi::ClientException("MapleLoginFailedException");
	}
	WAIT_UNTIL_STABLE;
}
void ClientApi::SelectServer(
	const CONF_INFO::NEXONAC_INFO::MAPLEID_INFO::SERVER_INFO& ServerInfo)
{
	MouseEvent(ServerInfo.CoorServer, LEFT_CLICK);
	KeybdEvent(VK_RETURN);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_E3, ClientApi::TARGETIMAGE_E3, NONWORK, 30000);
	}
	catch (MatchFailedException)
	{
		throw ClientApi::ClientException("SelectServerFailedException");
	}
	WAIT_UNTIL_STABLE;
}
void ClientApi::UnlockSecondPassword(
	const string& lpPw)
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
			MouseEvent({ 0, 0 }, LEFT_CLICK, 300);
			Cvw::ClickMatchedTemplate(
				Cvw::Capture(ClientApi::RECT_CLIENT4, 0),
				Cvw::Read(string(TARGET_DIR "button_") + Character + ".jpg"),
				LEFT_CLICK,
				{ 5, 5 });
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
void ClientApi::EnterGame(
	CONST CONF_INFO::NEXONAC_INFO::MAPLEID_INFO& MapleIdInfo)
{
	KeybdEvent(VK_RETURN, 0x400);
	try
	{
		Cvw::MatchTemplate(Cvw::Capture(ClientApi::RECT_CLIENT1), Cvw::Read(TARGET_DIR "button_1.jpg"));
		ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);
	}
	catch (MatchFailedException & cwe)
	{
		; // Second-password unrequired
	}

	try
	{
		Cvw::DoUntilMatchingTemplate(
			ClientApi::RECT_I1,
			ClientApi::TARGETIMAGE_I1,
			[]()
			{
				ClientApi::SET_CLIENT_STDPOS();
			},
			60000);
	}
	catch (MatchFailedException & cwe)
	{
		try
		{
			Cvw::MatchTemplate(Cvw::Capture(ClientApi::RECT_CLIENT1), ClientApi::TARGETIMAGE_E1);
			throw ClientApi::ServerDisconnectedException();
		}
		catch (MatchFailedException & mfe)
		{
			throw ClientApi::ClientException("GameEntryFailedException");
		}
	}
	WAIT_UNTIL_STABLE;

	// Remove tooltips
	MouseEvent({ 902, 337 }, LEFT_CLICK);
	KeybdEvent(VK_ESCAPE);
	KeybdEvent(VK_ESCAPE);
}
void ClientApi::SelectCharacter(
	unsigned int Seq)
{
	for (int i = 0; i < 46; i++)
	{
		KeybdEvent(VK_LEFT, 120);
	}
	for (int i = 0; i < Seq - 1; i++)
	{
		KeybdEvent(VK_RIGHT, 240);
	}
}
void ClientApi::RemoveAllIngameWindows(
	void)
{
	for (int i = 0; i < 4; i++)
	{
		KeybdEvent(VK_RETURN);
	}
	for (int i = 0; i < 4; i++)
	{
		KeybdEvent(VK_ESCAPE);
	}
	MouseEvent({ 64, 64 }, LEFT_CLICK);
}
void ClientApi::ExitGame(
	void)
{
	KeybdEvent(VK_ESCAPE);
	KeybdEvent(VK_UP);
	KeybdEvent(VK_RETURN);
	KeybdEvent(VK_RETURN);

	try
	{
		Cvw::DoUntilMatchingTemplate(
			ClientApi::RECT_E3,
			ClientApi::TARGETIMAGE_E3,
			[]()
			{
				ClientApi::SET_CLIENT_STDPOS();
			},
			30000);
	}
	catch (MatchFailedException & mfe)
	{
		try
		{
			Cvw::MatchTemplate(Cvw::Capture(ClientApi::RECT_CLIENT1), ClientApi::TARGETIMAGE_E1);
			throw ClientApi::ServerDisconnectedException();
		}
		catch (MatchFailedException & mfe)
		{
			throw ClientApi::GameExitException();
		}
	}
	WAIT_UNTIL_STABLE;
}
void ClientApi::Logout(
	void)
{
	KeybdEvent(VK_ESCAPE);
	KeybdEvent(VK_RETURN);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_E1, ClientApi::TARGETIMAGE_E1, NONWORK, 30000);
	}
	catch (MatchFailedException & mfe)
	{
		throw ClientApi::ClientException("LogoutFailedException");
	}
	WAIT_UNTIL_STABLE;
}
void ClientApi::ExitCharacterWindow(void)
{
	KeybdEvent(VK_ESCAPE);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_E2, ClientApi::TARGETIMAGE_E2, NONWORK, 30000);
	}
	catch (MatchFailedException & mfe)
	{
		throw;
	}
}
void ClientApi::MakeParty(
	void)
{
	KeybdEvent(VK_OEM_4);
	MouseEvent({ 0, 0 }, LEFT_CLICK);

	Mat SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
	static array<Mat, 2> ArrTargetImage = { Cvw::Read(TARGET_DIR "button_make.jpg"), Cvw::Read(TARGET_DIR "button_break.jpg") };
	auto TemplateInfo = Cvw::GetHighestMatchedTemplate(SourceImage, ArrTargetImage);

	if (0 == TemplateInfo.first)
	{
		MouseEvent(TemplateInfo.second.MaximumLocation, LEFT_CLICK);
		KeybdEvent(VK_RETURN);
	}
	KeybdEvent(VK_OEM_4);
}
void ClientApi::BreakParty(
	void)
{
	KeybdEvent(VK_OEM_4);
	MouseEvent({ 0, 0 }, LEFT_CLICK);

	Mat SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
	static array<Mat, 2> ArrTargetImage = { Cvw::Read(TARGET_DIR "button_make.jpg"), Cvw::Read(TARGET_DIR "button_break.jpg") };
	auto TemplateInfo = Cvw::GetHighestMatchedTemplate(SourceImage, ArrTargetImage);

	if (1 == TemplateInfo.first)
	{
		MouseEvent(TemplateInfo.second.MaximumLocation, LEFT_CLICK);
	}
	KeybdEvent(VK_OEM_4);
}
void ClientApi::MoveServer(
	bool IsForward)
{
	KeybdEvent(VK_ESCAPE, 400);
	KeybdEvent(VK_RETURN, 400);
	KeybdEvent(IsForward ? VK_RIGHT : VK_LEFT, 400);
	KeybdEvent(VK_RETURN, 1000);
}
void ClientApi::Jump(
	JUMP_T JumpType,
	DWORD MillisecondsRestTime = 0x400)
{
	switch (JumpType)
	{
	case JUMP_T::DEMON:
		KeybdEvent(VK_MENU, 60);
		KeybdEvent(VK_UP, 60);
		KeybdEvent(VK_UP, 2500);
		break;

	case JUMP_T::CYGNUS:
		KeybdEvent(VK_MENU, 60);

		KeybdEventDown(VK_UP);
		KeybdEvent(VK_MENU, 60);
		KeybdEventUp(VK_UP);
		break;

	case JUMP_T::NOVA:
		KeybdEvent('C', 2000);
		break;

	case JUMP_T::V_MATRIX:
		KeybdEvent('L', 180);
		break;

	case JUMP_T::ZERO:
		KeybdEventDown(VK_UP);
		KeybdEvent('F', 100);
		KeybdEventUp(VK_UP);
		break;

	case JUMP_T::WIZARD:
		KeybdEventDown(VK_UP);
		KeybdEvent(VK_SHIFT, 100);
		KeybdEventUp(VK_UP);
		break;
	}
	Sleep(MillisecondsRestTime);
}
void ClientApi::DownJump(
	DWORD MilliSecondsRestTime = 0x400)
{
	keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
	KeybdEvent(VK_MENU, MilliSecondsRestTime);
	keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), KEYEVENTF_KEYUP, 0);
}
