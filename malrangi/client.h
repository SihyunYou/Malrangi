/****************************************************************************
*
* client.h -- Application Interface for Client
*
****************************************************************************/

#pragma once
#include "winevent.h"
#include "cvwrap.h"
#include "user.h"

class ClientApi
{
public:
#define HWND_MAPLESTORY (::FindWindow(NULL, TEXT("MapleStory")))
#define SourceImageClient1				(Capture(ClientApi::RECT_CLIENT1))
#define SourceImageClient4				(Capture(ClientApi::RECT_CLIENT4))
#define SourceImageClient4Colored		(Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR))
	static void SET_CLIENT_STDPOS(void);
	inline static const RECT RECT_CLIENT1 = { 0, 0, 800, 600 + 26 };
	inline static const RECT RECT_CLIENT4 = { 0, 0, 1366, 768 + 26 };
	inline static const Mat TARGETIMAGE_EXTERN1 = Read(TARGET_DIR "window//initial_logo.jpg");
	inline static const Mat TARGETIMAGE_EXTERN2 = Read(TARGET_DIR "window//world-channel_select.jpg");
	inline static const Mat TARGETIMAGE_EXTERN3 = Read(TARGET_DIR "window//character_select.jpg");
	inline static const Mat TARGETIMAGE_INTERN = Read(TARGET_DIR "button//meilege.jpg");

public:
	class MinimapRecognizer
	{
	public:
		class CharacterNotFoundException : public MalrangiException
		{
		public:
			CharacterNotFoundException(void) 
				: MalrangiException(__CLASSNAME__) {}
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

	public:
		int WhereAmI(void)
		{
			Mat Mask;

			if (InRange(Capture(RectMinimap, IMREAD_COLOR), Scalar(20, 235, 245), Scalar(40, 245, 255), Mask))
			{
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
			return -1;
		};
		void MoveToRelativeCriteria(
			int MinimapRelativeCriteria)
		{
			int CurrentPosition = WhereAmI();
			int RelativeDistance = CurrentPosition - MinimapRelativeCriteria;

			if (CurrentPosition == -1)
			{
				throw CharacterNotFoundException();
			}
			else
			{
				if (RelativeDistance < 0)
				{
					KeybdEventDown(VK_RIGHT);
					while (RelativeDistance <= 0)
					{
						RelativeDistance = WhereAmI() - MinimapRelativeCriteria + 2;
					}
					KeybdEventUp(VK_RIGHT);
				}
				else if (RelativeDistance > 0)
				{
					KeybdEventDown(VK_LEFT);
					while (RelativeDistance >= 0)
					{
						RelativeDistance = WhereAmI() - MinimapRelativeCriteria - 2;
					}
					KeybdEventUp(VK_LEFT);
				}
			}
		}

	private:
		RECT RectMinimap;
	};

	class Exception : public MalrangiException
	{
	public:
		Exception(void) :
			MalrangiException(__CLASSNAME__) {}
		Exception(string Description) :
			MalrangiException(__CLASSNAME__ + "! " + Description) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};

	/****************************************************************************
	* Excetpions handled specifically
	****************************************************************************/
	class BootFailedException : public ClientApi::Exception
	{
	public:
		BootFailedException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class ServerDelayException : public ClientApi::Exception
	{
	public:
		ServerDelayException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};

	/****************************************************************************
	* Exceptions handled commonly
	****************************************************************************/
	static void ThrowLowException(const char* DefaultWhat)
	{
		static const Mat TargetImageWindowServerDisconnection = Read(TARGET_DIR "window//server-disconnection.jpg");

		if (HWND_MAPLESTORY == NULL)
		{
			throw ClientApi::ClientAbnormalTerminationException();
		}
		else if (MatchTemplate(SourceImageClient1, TargetImageWindowServerDisconnection))
		{
			throw ClientApi::ServerDisconnectedException();
		}
		else
		{
			throw MalrangiException(DefaultWhat);
		}
	}
	class ServerDisconnectedException : public ClientApi::Exception
	{
	public:
		ServerDisconnectedException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class ClientAbnormalTerminationException : public ClientApi::Exception
	{
	public:
		ClientAbnormalTerminationException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
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
		const USERCONF::NEXONAC_INFO&,
		const USERCONF::MAPLEID_INFO&);
	static void SelectServer(
		const USERCONF::SERVER_INFO&,
		int); 
	static void SelectCharacter(
		unsigned int); 
	static void UnlockSecondPassword(
		const string&); 
	static void ExitCharacterWindow(void);
	static void Logout(void);
	static void TerminateClient(void);


	/****************************************************************************
	* Internal Game Window Control
	****************************************************************************/
	static void EnterGame(
		const USERCONF::MAPLEID_INFO&);
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
	SetWindowPos(
		GetConsoleWindow(),
		0,
		1362, 0,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(
		HWND_MAPLESTORY,
		HWND_TOPMOST,
		-3, 0,
		0, 0,
		SWP_NOSIZE);
}

void ClientApi::BootClient(
	void)
{
	ShellExecute(NULL, TEXT("open"), TEXT("C:\\Nexon\\Maple\\MapleStory.exe"), TEXT("GameLaunching"), NULL, SW_SHOW);
	if (DoUntilMatchingTemplate(
		ClientApi::RECT_CLIENT1,
		ClientApi::TARGETIMAGE_EXTERN1,
		[](){
			ClientApi::SET_CLIENT_STDPOS();
		},
		seconds(180)))
	{
		Sleep(0x200);
	}
	else
	{
		throw BootFailedException();
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
	CONST USERCONF::NEXONAC_INFO& AccountInfo,
	CONST USERCONF::MAPLEID_INFO& MapleIdInfo)
{
	static const Mat TargetImageWindowMapleId = Read(TARGET_DIR "window//mapleid.jpg");
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
	
	MouseEvent({ 334, 282 }, LEFT_CLICK);
	MouseEvent({ 452, 282 }, LEFT_CLICK);
	WriteString(AccountInfo.Id);
	KeybdEvent(VK_TAB);
	WriteString(AccountInfo.Password);
	KeybdEvent(VK_RETURN);
	SetCursorPos(100, 100);

	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, TargetImageWindowMapleId, seconds(20)))
	{
		Sleep(0x400);
	}
	else
	{
		throw MalrangiException("NexonLoginFailedException");
	}

	
	VALLOC MatchInfo;
	for(int i = 0; i < 3; i++)
	{
		if (MatchTemplate(SourceImageClient1, Read(TARGET_DIR "mapleid//" + MapleIdInfo.Id + ".jpg"), &MatchInfo, 0.96))
		{
			break;
		}
		else
		{
			MouseEvent({ 513, 396 }, LEFT_CLICK);
		}
	}
	MouseEvent(MatchInfo.Location + Point{ 10, 4 }, DLEFT_CLICK);

	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN2, seconds(15)))
	{
		Sleep(0x400);
	}
	else
	{
		throw MalrangiException("MapleLoginFailedException");
	}
}
void ClientApi::SelectServer(
	const USERCONF::SERVER_INFO& ServerInfo,
	int ChannelNumber)
{
	static const Mat TargetImageWindowServerDelay = Read(TARGET_DIR "window//server-delay.jpg");
	
	if(VALLOC MatchInfo;
		MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "server//" + ServerInfo.ServerName + ".jpg"), &MatchInfo))
	{
		MouseEvent(MatchInfo.Location, LEFT_CLICK);
		MouseEvent({ 291 + 70 * (ChannelNumber % 5), 255 + 30 * (ChannelNumber / 5) }, DLEFT_CLICK);
	}
	else
	{
		throw MalrangiException("SelectServerFailedException");
	}

	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN3, seconds(60)))
	{
		Sleep(0x400);
	}
	else
	{
		if (MatchTemplate(SourceImageClient1, TargetImageWindowServerDelay))
		{
			throw ServerDelayException();
		}
		else
		{
			ThrowLowException(__FEWHAT__);
		}
	}
}
void ClientApi::UnlockSecondPassword(
	const string& Password)
{
	ClientApi::SET_CLIENT_STDPOS();

	for each (auto Character in Password)
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
		{
			VALLOC MatchInfo;
			if (MatchTemplate(SourceImageClient4, Read(string(TARGET_DIR "button//2pwd//") + Character + ".jpg"), &MatchInfo))
			{
				MouseEvent(MatchInfo.Location + Point{5, 5}, LEFT_CLICK);
			}
				
			break;
		}
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
	CONST USERCONF::MAPLEID_INFO& MapleIdInfo)
{
	KeybdEvent(VK_RETURN, 0x400);

	if(MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "button//2pwd//1.jpg")))
	{
		ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);
	}

	if (DoUntilMatchingTemplate(
		ClientApi::RECT_CLIENT4,
		ClientApi::TARGETIMAGE_INTERN,
		[]()
		{
			ClientApi::SET_CLIENT_STDPOS();
		},
		seconds(150)))
	{
		Sleep(0x400);

		// Remove tooltips
		MouseEvent({ 902, 363 }, LEFT_CLICK);
		KeybdEvent(VK_ESCAPE);
		KeybdEvent(VK_ESCAPE);
	}
	else
	{
		ThrowLowException(__FEWHAT__);
	}
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
	KeybdEventUp(VK_LEFT);
	KeybdEventUp(VK_RIGHT);
	KeybdEventUp(VK_UP);
	KeybdEventUp(VK_DOWN);

	for (int i = 0; i < 4; i++)
	{
		KeybdEvent(VK_RETURN);
	}
	for (int i = 0; i < 4; i++)
	{
		KeybdEvent(VK_ESCAPE);
	}
	MouseEvent({ 10, 476 }, LEFT_CLICK);
}
void ClientApi::ExitGame(
	void)
{
	KeybdEvent(VK_ESCAPE);
	KeybdEvent(VK_UP);
	KeybdEvent(VK_RETURN);
	KeybdEvent(VK_RETURN);

	if (DoUntilMatchingTemplate(
		ClientApi::RECT_CLIENT1,
		ClientApi::TARGETIMAGE_EXTERN3,
		[]()
		{
			ClientApi::SET_CLIENT_STDPOS();
		},
		seconds(30)))
	{
		Sleep(0x400);
	}
	else
	{
		ThrowLowException(__FEWHAT__);
	}
}
void ClientApi::Logout(
	void)
{
	KeybdEvent(VK_ESCAPE);
	KeybdEvent(VK_RETURN);
	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN1, seconds(30)))
	{
		Sleep(0x400);
	}
	else
	{
		ThrowLowException(__FEWHAT__);
	}
}
void ClientApi::ExitCharacterWindow(void)
{
	KeybdEvent(VK_ESCAPE);
	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN2, seconds(30)))
	{
		Sleep(0x400);
	}
	else
	{
		ThrowLowException(__FEWHAT__);
	}
}
void ClientApi::MakeParty(
	void)
{
	static const array<Mat, 2> ArrTargetImage = { Read(TARGET_DIR "button//make.jpg"), Read(TARGET_DIR "button//break.jpg") };

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
	ClientApi::SET_CLIENT_STDPOS();

	const auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(ClientApi::RECT_CLIENT4), ArrTargetImage);
	if (0 == Seq)
	{
		MouseEvent(Location, LEFT_CLICK);
		KeybdEvent(VK_RETURN);
	}
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
}
void ClientApi::BreakParty(
	void)
{
	static const array<Mat, 2> ArrTargetImage = { Read(TARGET_DIR "button//make.jpg"), Read(TARGET_DIR "button//break.jpg") };

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
	ClientApi::SET_CLIENT_STDPOS();

	const auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(ClientApi::RECT_CLIENT4), ArrTargetImage);
	if (1 == Seq)
	{
		MouseEvent(Location, LEFT_CLICK);
	}
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
}
void ClientApi::MoveServer(
	bool IsForward)
{
	static const Mat TargetImage = Read(TARGET_DIR "button//channel-change.jpg");

	KeybdEvent(VK_ESCAPE, 0x200);
	KeybdEvent(VK_RETURN, 0x400);

	if (WaitUntilMatchingTemplate(
		ClientApi::RECT_CLIENT4,
		TargetImage,
		seconds(10)))
	{
		KeybdEvent(IsForward ? VK_RIGHT : VK_LEFT, 0x200);
		KeybdEvent(VK_RETURN, 0x400);
	}
	else
	{
		ThrowLowException(__FEWHAT__);
	}
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
		KeybdEvent(VK_UP, 1800);
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