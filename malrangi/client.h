/****************************************************************************
*
* client.h -- Application Interface for Client
*
****************************************************************************/

#pragma once
#include "winevent.h"
#include "cvwrap.h"
#include "user.h"
#include "ipmanage.h"

class ClientApi
{
public:
#define HWND_MAPLESTORY (::FindWindow(NULL, TEXT("MapleStory")))
#define SourceImageClient1				(Capture(ClientApi::RECT_CLIENT1))
#define SourceImageClient4				(Capture(ClientApi::RECT_CLIENT4))
#define SourceImageClient4Colored		(Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR))
#define POS_VOID		(Point{793, 737})
	static void SET_CLIENT_STDPOS(void);
	inline static const RECT RECT_CLIENT1 = { 0, 0, 800, 600 + 26 };
	inline static const RECT RECT_CLIENT4 = { 0, 0, 1366, 768 + 26 };
	inline static const Mat TARGETIMAGE_EXTERN1 = Read(TARGET_DIR "window//initial_logo.jpg");
	inline static const Mat TARGETIMAGE_EXTERN2 = Read(TARGET_DIR "window//world-channel_select.jpg");
	inline static const Mat TARGETIMAGE_EXTERN3 = Read(TARGET_DIR "window//character_select.jpg");
	inline static const Mat TARGETIMAGE_INTERN = Read(TARGET_DIR "button//quest.jpg");

public:
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

	class MinimapRecognizer
	{
	public:
		class CharacterNotFoundException : public ClientApi::Exception
		{
		public:
			CharacterNotFoundException(void) 
				: ClientApi::Exception(__CLASSNAME__) {}
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



	/****************************************************************************
	* Excetpions handled specifically
	****************************************************************************/
	class NetworkDisconnectedException : public ClientApi::Exception
	{
	public:
		NetworkDisconnectedException(void) :
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
	class BlockFromCapchaException : public ClientApi::Exception
	{
	public:
		BlockFromCapchaException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class SecondPasswordNotLiftException : public ClientApi::Exception
	{
	public:
		SecondPasswordNotLiftException(void) :
			ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};

	/****************************************************************************
	* Exceptions handled commonly
	****************************************************************************/
	[[ noreturn ]] static void ThrowLowException(const char* DefaultWhat)
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
		else if (IpManage Manager;
			!Manager.IsNetworkConnected())
		{
			throw ClientApi::NetworkDisconnectedException();
		}
		else
		{
			throw ClientApi::Exception(DefaultWhat);
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
	static void EntrerDansServeur(
		const USERCONF::SERVER_INFO&,
		int); 
	static void SelecterCaractere(
		const USERCONF::SERVER_INFO&);
	static bool UnlockSecondPassword(
		const string&); 
	static void ExitCharacterWindow(void);
	static void Logout(void);
	static void TerminateClient(void);


	/****************************************************************************
	* Internal Game Window Control
	****************************************************************************/
	static void EntrerDansGame(
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
	static void OpererParty(int);
	static void MoveServer(bool);
	static void RemoveAllIngameWindows(void);
	static void ExitGame(void);
	static int ThrowItem(Mat);
	static void SupprimerBuf(int);
};

#define METTRE_CURSEUR_A_STDVIDE (MouseEvent(POS_VOID, METTRE_CURSEUR, 100))
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
	ClientApi::TerminateClient();
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
		if (IpManage Manager;
			!Manager.IsNetworkConnected())
		{
			throw NetworkDisconnectedException();
		}

		throw ClientApi::Exception("BootFailedException");
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
	
	bool IsThrowable = false;

	__NEXON_LOGIN__:
	MouseEvent({ 334, 282 }, CLIC_GAUCHE);
	MouseEvent({ 452, 282 }, CLIC_GAUCHE);
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
		if (!IsThrowable)
		{
			IsThrowable = true;
			KeybdEvent(VK_RETURN);
			ClientApi::SET_CLIENT_STDPOS();

			goto __NEXON_LOGIN__;
		}
		else
		{
			throw ClientApi::Exception("NexonLoginFailedException");
		}
	}


__MAPLE_LOGIN__:
	VALLOC MatchInfo;
	for(int i = 0; i < 3; i++)
	{
		if (MatchTemplate(SourceImageClient1, Read(TARGET_DIR "mapleid//" + MapleIdInfo.Id + ".jpg"), &MatchInfo, 0.96))
		{
			break;
		}
		else
		{
			MouseEvent({ 513, 396 }, CLIC_GAUCHE);
		}
	}
	MouseEvent(MatchInfo.Location + Point{ 10, 4 }, DOUBLECLIC_GAUCHE);

	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN2, seconds(15)))
	{
		Sleep(0x400);
	}
	else
	{
		throw ClientApi::Exception("MapleLoginFailedException");
	}
}
void ClientApi::EntrerDansServeur(
	const USERCONF::SERVER_INFO& ServerInfo,
	int ChannelNumber)
{
	static const Mat TargetImageWindowServerDelay = Read(TARGET_DIR "window//server-delay.jpg");
	
	if(VALLOC MatchInfo;
		MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "server//" + ServerInfo.ServerName + ".jpg"), &MatchInfo))
	{
		MouseEvent(MatchInfo.Location, CLIC_GAUCHE);
		MouseEvent({ 291 + 70 * (ChannelNumber % 5), 255 + 30 * (ChannelNumber / 5) }, DOUBLECLIC_GAUCHE);
	}
	else
	{
		throw ClientApi::Exception("SelectServerFailedException");
	}

	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN3, seconds(180)))
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
bool ClientApi::UnlockSecondPassword(
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
				MouseEvent(MatchInfo.Location + Point{5, 5}, CLIC_GAUCHE);
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

	return true;
}
void ClientApi::EntrerDansGame(
	CONST USERCONF::MAPLEID_INFO& MapleIdInfo)
{
__REUNLOCK__:
	KeybdEvent(VK_RETURN, 0x400);
	if (MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "button//block.png")))
	{
		throw BlockFromCapchaException();
	}

	int t = 0;
	if(MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "button//2pwd//1.jpg")))
	{
		if(t < 2)
		{
			ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);
		}
		else
		{
			throw SecondPasswordNotLiftException();
		}
	}

	if (DoUntilMatchingTemplate(
		ClientApi::RECT_CLIENT4,
		ClientApi::TARGETIMAGE_INTERN,
		[]()
		{
			ClientApi::SET_CLIENT_STDPOS();
		},
		seconds(180)))
	{
		Sleep(0x400);

		// Remove tooltips
		KeybdEvent({ VK_ESCAPE, VK_ESCAPE, VK_ESCAPE }, 600);
		MouseEvent(POS_VOID, CLIC_GAUCHE);
	}
	else
	{
		static const Mat TargetImage = Read(TARGET_DIR "window//2pwd_not_equal.jpg");
		if (MatchTemplate(Capture(ClientApi::RECT_CLIENT1), TargetImage))
		{
			++t;
			KeybdEvent(VK_RETURN);

			goto __REUNLOCK__;
		}
		else
		{
			ThrowLowException(__FEWHAT__);
		}
	}

	if (VALLOC MatchInfo;
		MatchTemplate(Capture(ClientApi::RECT_CLIENT4), Read(TARGET_DIR "button//diminuer_chat.png"), &MatchInfo))
	{
		MouseEvent({ MatchInfo.Location.x + 2, MatchInfo.Location.y + 2 }, CLIC_GAUCHE);
	}
}
void ClientApi::SelecterCaractere(
	const USERCONF::SERVER_INFO& ServerInfo)
{
	static const array<Mat, 5> ArrTargetImageTextCharacterSelect =
	{
		Read(TARGET_DIR "text//character_select_p1.jpg"),
		Read(TARGET_DIR "text//character_select_p2.jpg"),
		Read(TARGET_DIR "text//character_select_p3.jpg"),
		Read(TARGET_DIR "text//character_select_p4.jpg"),
		Read(TARGET_DIR "text//character_select_p5.jpg")
	};

	for (int i = 0; 
#if defined(BUILD_URUS)
		i < ServerInfo.NombreDeCharacter + 2;
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
		i < ServerInfo.VecCharacter.size() + 2;
#endif
		i++)
	{
		KeybdEvent(VK_LEFT, 120);
	}

#if defined(BUILD_URUS)
	MouseEvent({ 135, 235 }, CLIC_GAUCHE);
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
	int Seq = 1;
	for (auto& CharacterInfo : ServerInfo.VecCharacter)
	{
		if (CharacterInfo.IsCompleted)
		{
			++Seq;
			continue;
		}
		else
		{
			break;
		}
	}

	for (int i = 0; i < (Seq - 1) / 8; i++)
	{
		MouseEvent({ 505, 545 }, CLIC_GAUCHE, 8000);
	}
	MouseEvent(
		{ (135 + (((Seq - 1) % 8) % 4) * 125), (235 + (((Seq % 8) <= 4 && (Seq % 8) >= 1) ? 0 : 1) * 200) },
		CLIC_GAUCHE);
#endif
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
		KeybdEvent(VK_RETURN, 256);
	}
	for (int i = 0; i < 4; i++)
	{
		KeybdEvent(VK_ESCAPE, 256);
	}
	MouseEvent(POS_VOID, CLIC_GAUCHE);
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
void ClientApi::OpererParty(int CodeOperation)
{
	static const array<Mat, 2> ArrTargetImage = { Read(TARGET_DIR "button//make.jpg"), Read(TARGET_DIR "button//break.jpg") };
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
	ClientApi::SET_CLIENT_STDPOS();
	METTRE_CURSEUR_A_STDVIDE;

	const auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(ClientApi::RECT_CLIENT4), ArrTargetImage);
	switch (CodeOperation)
	{
	case 1: // Faire
		if (0 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE);
			KeybdEvent(VK_RETURN);
		}
		break;
	case 2: // Casser
		if (1 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE);
		}
		break;
	case 3: // Refaire
		MouseEvent(Location, CLIC_GAUCHE);
		if (1 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE);
		}
		KeybdEvent(VK_RETURN);
		break;
	}

	KeybdEvent(VK_ESCAPE);
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
int ClientApi::ThrowItem(
	Mat TargetImage
)
{
	static const Mat TargetImageButtonEtc = Read(TARGET_DIR "button//etc.jpg");
	static const Mat TargetImageButtonExpandingInventory = Read(TARGET_DIR "button//expanding_inventory.jpg");

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);

	if (VALLOC MatchInfo;
		MatchTemplate(SourceImageClient4, TargetImage, &MatchInfo))
	{
		MouseEvent(MatchInfo.Location + Point{ 10, 10 }, CLIC_GAUCHE);
	}
	else
	{
		METTRE_CURSEUR_A_STDVIDE;
		if (MatchTemplate(SourceImageClient4, TargetImageButtonEtc, &MatchInfo))
		{
			MouseEvent(MatchInfo.Location + Point{ 5, 5 }, CLIC_GAUCHE);

			if (MatchTemplate(SourceImageClient4, TargetImage, &MatchInfo))
			{
				MouseEvent(MatchInfo.Location + Point{ 10, 10 }, CLIC_GAUCHE);
			}
			else
			{
				if (MatchTemplate(SourceImageClient4, TargetImageButtonExpandingInventory, &MatchInfo))
				{
					MouseEvent(MatchInfo.Location + Point{ 6, 6 }, CLIC_GAUCHE);

					if (MatchTemplate(SourceImageClient4, TargetImage, &MatchInfo))
					{
						MouseEvent(MatchInfo.Location + Point{ 10, 10 }, CLIC_GAUCHE);
					}
					else
					{
						return -1;
					}
				}
				else
				{
					return -1;
				}
			}
		}
		else
		{
			return -1;
		}
	}

	MouseEvent(POS_VOID, CLIC_GAUCHE);
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);

	return 0;
}
void ClientApi::SupprimerBuf(int n)
{
	for (int p = 0; p < n; p++)
	{
		MouseEvent({ 1350, 45 }, CLIC_DROIT, 1000);
	}
}