/****************************************************************************
*
* client.h -- Application Interface for Client
*
****************************************************************************/

#pragma once
#include "winevent.h"
#include "cvwrap.h"
#include "user.h"
#include "contrôler_ip.h"


class ClientApi
{
public:
#define HWND_MAPLESTORY (::FindWindow(NULL, TEXT("MapleStory")))
#define SourceImageClient1				(Capture(ClientApi::RECT_CLIENT1))
#define SourceImageClient4				(Capture(ClientApi::RECT_CLIENT4))
#define SourceImageClient4Colored		(Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR))
#define POS_VOID		(Point{793, 737})
#define GET_DURATION(_d, _s) (_d / _s)
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





	/****************************************************************************
	* Excetpions handled specifically
	****************************************************************************/
	class ServerDelayException : public ClientApi::Exception
	{
	public:
		ServerDelayException(void) :
			ClientApi::Exception("서버 지연") {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class BlockFromCapchaException : public ClientApi::Exception
	{
	public:
		BlockFromCapchaException(void) :
			ClientApi::Exception("네트워크 설정 장애로 인해 게임에 진입할 수 없음") {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};
	class SecondPasswordNotLiftException : public ClientApi::Exception
	{
	public:
		SecondPasswordNotLiftException(void) :
			ClientApi::Exception("2차 비밀번호 입력 실패") {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};

	/****************************************************************************
	* Exceptions handled commonly
	****************************************************************************/
	enum class ÉTAT_DE_LOGOUT
	{
		NORMAL,
		SERVEUR_DECONNECTE,
		CLIENT_ANORMALEMENT_TERMINE,
		FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU,
		INDISPONIBLE,
		INCOMPATIBLE
	};

	class ExceptionCommun : public ClientApi::Exception
	{
	public:
		explicit ExceptionCommun(const char* DefaultWhat) :
			ClientApi::Exception(DefaultWhat) {}
		ÉTAT_DE_LOGOUT CestQuoi()
		{
			static const Mat TargetImageWindowServerDisconnection = Read(TARGET_DIR "window//server-disconnection.jpg");

			if (HWND_MAPLESTORY == NULL)
			{
				Message = "비정상적인 클라이언트 종료";
				return ÉTAT_DE_LOGOUT::CLIENT_ANORMALEMENT_TERMINE;
			}
			else if (MatchTemplate(SourceImageClient1, TargetImageWindowServerDisconnection))
			{
				Message = "서버와의 연결 끊김";
				return ÉTAT_DE_LOGOUT::SERVEUR_DECONNECTE;
			}
			else if (IpManage Manager;
				!Manager.IsNetworkConnected())
			{
				Message = "네트워크 재설정 필요";
				return ÉTAT_DE_LOGOUT::FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU;
			}

			return ÉTAT_DE_LOGOUT::INDISPONIBLE;
		}
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
	static void Jump(
		const USERCONF::CODE_PROPRE&);
	static void DownJump(
		void);
	static void OpererParty(int);
	static void EffaceTousWindows(void);
	static void ExitGame(void);
	static void DiminuerChatWindow(void);
	static int OuvreInventaire(int);
	static int ObtenirNombreDeItem(Mat, int);
	static int JeteItem(Mat, int);
	static void SupprimerBuf(void);
	static void ClientApi::ChangerChaîne(bool);
};

#define METTRE_CURSEUR_À_STDVIDE (MouseEvent(POS_VOID, METTRE_CURSEUR, 100))
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
		[]() {
			ClientApi::SET_CLIENT_STDPOS();
		},
		seconds(180)))
	{
		Sleep(0x200);
	}
	else
	{
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
			throw ClientApi::Exception("넥슨 로그인 실패");
		}
	}


__MAPLE_LOGIN__:
	VALLOC MatchInfo;
	for (int i = 0; i < 3; i++)
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
		throw ClientApi::Exception("메이플 로그인 실패");
	}
}
void ClientApi::EntrerDansServeur(
	const USERCONF::SERVER_INFO& ServerInfo,
	int ChannelNumber)
{
	static const Mat TargetImageWindowServerDelay = Read(TARGET_DIR "window//server-delay.jpg");
	int q = 0;

	if (VALLOC MatchInfo;
		MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "server//" + ServerInfo.NomDeServeur + ".jpg"), &MatchInfo))
	{
		MouseEvent(MatchInfo.Location, CLIC_GAUCHE);
	__REESSAYER__:
		MouseEvent({ 291 + 70 * (ChannelNumber % 5), 255 + 30 * (ChannelNumber / 5) }, DOUBLECLIC_GAUCHE);
	}
	else
	{
		throw ClientApi::Exception("서버 이미지를 찾을 수 없음");
	}


	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN3, seconds(20)))
	{
		Sleep(0x400);
	}
	else
	{
		if (MatchTemplate(SourceImageClient1, TargetImageWindowServerDelay))
		{
			if (q < 3)
			{
				q++;
				KeybdEvent(VK_RETURN, 10000);

				goto __REESSAYER__;
			}
			throw ServerDelayException();
		}
		else
		{
			throw ExceptionCommun(__FUNCTION__);
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
				MouseEvent(MatchInfo.Location + Point{ 5, 5 }, CLIC_GAUCHE);
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
	if (MatchTemplate(Capture(ClientApi::RECT_CLIENT1), Read(TARGET_DIR "button//2pwd//1.jpg")))
	{
		if (t < 2)
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
		seconds(20)))
	{
		Sleep(0x400);

		// Remove tooltips
		KeybdEvent({ VK_ESCAPE, VK_ESCAPE, VK_ESCAPE }, 600);
		MouseEvent(POS_VOID, CLIC_GAUCHE);
		if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "button//setting.png")))
		{
			MouseEvent({ 1013, 750 }, CLIC_GAUCHE);
		}
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
			throw ExceptionCommun(__FUNCTION__);
		}
	}

	ClientApi::DiminuerChatWindow();
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
#elif defined(BUILD_BOSS) || defined(BUILD_COMPTE)
		i < ServerInfo.VecCharacter.size() + 2;
#endif
		i++)
	{
		KeybdEvent(VK_LEFT, 120);
	}

#if defined(BUILD_URUS)
	MouseEvent({ 135, 235 }, CLIC_GAUCHE);
#elif defined(BUILD_BOSS) || defined(BUILD_COMPTE)
	int Seq = 1;
	for (auto& CharacterInfo : ServerInfo.VecCharacter)
	{
		if (CharacterInfo.EstAccompli)
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
		MouseEvent({ 505, 545 }, CLIC_GAUCHE, 1000);
	}
	MouseEvent(
		{ (135 + (((Seq - 1) % 8) % 4) * 125), (235 + (((Seq % 8) <= 4 && (Seq % 8) >= 1) ? 0 : 1) * 200) },
		CLIC_GAUCHE);
#endif
}
void ClientApi::EffaceTousWindows(
	void)
{
	KeybdEventUp(VK_LEFT);
	KeybdEventUp(VK_RIGHT);
	KeybdEventUp(VK_UP);
	KeybdEventUp(VK_DOWN);

	for (int i = 0; i < 3; i++)
	{
		KeybdEvent(VK_RETURN, 256);
	}
	for (int i = 0; i < 3; i++)
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
		ExceptionCommun(__FUNCTION__);
	}
}
void ClientApi::Logout(
	void)
{
	KeybdEvent({ VK_ESCAPE, VK_RETURN });
	if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT1, ClientApi::TARGETIMAGE_EXTERN1, seconds(30)))
	{
		Sleep(0x400);
	}
	else
	{
		ExceptionCommun(__FUNCTION__);
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
		ExceptionCommun(__FUNCTION__);
	}
}
#define FAIRE 1
#define CASSER 2
#define REFAIRE 3
void ClientApi::OpererParty(int CodeOperation)
{
	if (NULL == CodeOperation)
	{
		return;
	}

	static const array<Mat, 2> ArrTargetImage = { Read(TARGET_DIR "button//make.jpg"), Read(TARGET_DIR "button//break.jpg") };
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party);
	ClientApi::SET_CLIENT_STDPOS();
	METTRE_CURSEUR_À_STDVIDE;

	const auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(ClientApi::RECT_CLIENT4), ArrTargetImage);
	switch (CodeOperation)
	{
	case FAIRE:
		if (0 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE, 400);
			KeybdEvent(VK_RETURN, 400);
		}
		break;
	case CASSER:
		if (1 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE, 400);
		}
		break;
	case REFAIRE:
		if (1 == Seq)
		{
			MouseEvent(Location, CLIC_GAUCHE, 400);
		}
		MouseEvent(Location, CLIC_GAUCHE, 400);
		KeybdEvent(VK_RETURN, 400);
		break;
	}

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Party, 400);
}
void ClientApi::Jump(
	const USERCONF::CODE_PROPRE& CodeDeCaractère)
{
	switch (CodeDeCaractère)
	{
	case USERCONF::CODE_PROPRE::듀블:
		KeybdEvent(VK_MENU, 200);
	case USERCONF::CODE_PROPRE::닼나:
	case USERCONF::CODE_PROPRE::메카닉:
		KeybdEvent('X', 2000);
		return;
	case USERCONF::CODE_PROPRE::나로:
		KeybdEvent(VK_MENU, 80);
		KeybdEvent('C', 1500);
		return;
	case USERCONF::CODE_PROPRE::메세:
		KeybdEvent('A', 1000);
		KeybdEvent('S', 1500);
		//	KeybdEvent('D', 1500);
		return;
	case USERCONF::CODE_PROPRE::보마:
	case USERCONF::CODE_PROPRE::신궁:
	case USERCONF::CODE_PROPRE::패파:
	case USERCONF::CODE_PROPRE::데벤:
	case USERCONF::CODE_PROPRE::데벤2:
	case USERCONF::CODE_PROPRE::데슬:
		KeybdEvent({ VK_MENU, VK_UP, VK_UP }, 60);
		break;

	case USERCONF::CODE_PROPRE::아란:
		KeybdEventDown(VK_UP);
		KeybdEvent('Q', 60);
		KeybdEventUp(VK_UP);
		break;

	case USERCONF::CODE_PROPRE::소마:
	case USERCONF::CODE_PROPRE::플위:
	case USERCONF::CODE_PROPRE::윈브:
	case USERCONF::CODE_PROPRE::나워:
	case USERCONF::CODE_PROPRE::스커:
		KeybdEvent(VK_MENU, 60);
		KeybdEventDown(VK_UP);
		KeybdEvent(VK_MENU, 60);
		KeybdEventUp(VK_UP);
		break;

	case USERCONF::CODE_PROPRE::키네:
	case USERCONF::CODE_PROPRE::아크:
	case USERCONF::CODE_PROPRE::아델:
		KeybdEventDown(VK_UP);
		KeybdEvent({ VK_MENU, VK_MENU, VK_MENU, VK_MENU, VK_MENU }, 40);
		KeybdEventUp(VK_UP);
		break;

	case USERCONF::CODE_PROPRE::팬텀:
		KeybdEvent('X', 1000);
		KeybdEventContinued(VK_UP, 200);
		Sleep(500);
		return;

	case USERCONF::CODE_PROPRE::호영:
		KeybdEventDown(VK_UP);
		KeybdEvent(VK_MENU, 60);
		KeybdEvent(VK_MENU, 60);
		KeybdEvent(VK_MENU, 60);
		KeybdEvent(VK_MENU, 60);
		KeybdEvent(VK_MENU, 60);
		KeybdEventContinued(VK_MENU, 800);
		KeybdEventUp(VK_UP);
		return;

	case USERCONF::CODE_PROPRE::배메:
	case USERCONF::CODE_PROPRE::썬콜:
	case USERCONF::CODE_PROPRE::루미:
	case USERCONF::CODE_PROPRE::제로:
		KeybdEventDown(VK_UP);
		KeybdEvent('C', 100);
		KeybdEventUp(VK_UP);
		break;
	}

	Sleep(0x700);
}
void ClientApi::DownJump(
	void)
{
	keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
	Sleep(100);
	KeybdEvent(VK_MENU, 200);
	Sleep(100);
	keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), KEYEVENTF_KEYUP, 0);
}
void ClientApi::DiminuerChatWindow(
	void)
{
	if (VALLOC MatchInfo;
		MatchTemplate(Capture(ClientApi::RECT_CLIENT4), Read(TARGET_DIR "button//diminuer_chat.png"), &MatchInfo))
	{
		MouseEvent({ MatchInfo.Location.x + 2, MatchInfo.Location.y + 2 }, CLIC_GAUCHE);
	}
}
int ClientApi::OuvreInventaire(
	int Case)
{
	METTRE_CURSEUR_À_STDVIDE;
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);

	if (VALLOC InfoMatchée;
		MatchTemplate(SourceImageClient4, Read(TARGET_DIR "button//coiffure.png"), &InfoMatchée))
	{
		MouseEvent(InfoMatchée.Location + Point{ -75, 5 }, CLIC_GAUCHE);
		METTRE_CURSEUR_À_STDVIDE;

		__RENDRE_VRAI;
	}

	__RENDRE_ERREUR("인벤토리 열기 실패");
}
int ClientApi::ObtenirNombreDeItem(
	Mat TargetImage,
	int Case = 3)
{
	if (int Valuer;
		VRAI != (Valuer = ClientApi::OuvreInventaire(Case)))
	{
		return Valuer;
	}

	VALLOC InfoMatchée;
	if (MatchTemplate(SourceImageClient4, 
		Read(TARGET_DIR "button//expanding_inventory.jpg"), 
		&InfoMatchée))
	{
		MouseEvent(InfoMatchée.Location + Point{ 6, 6 }, CLIC_GAUCHE);
	}
	
	Mat SourceImage = SourceImageClient4;
	int NombreDeTargetImage = 0;
	while (MatchTemplate(SourceImage, TargetImage, &InfoMatchée))
	{
		rectangle(SourceImage, Rect(InfoMatchée.Location, TargetImage.size()), Scalar(255, 255, 255), FILLED);
		++NombreDeTargetImage;
	}

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);
	return NombreDeTargetImage;
}
int ClientApi::JeteItem(
	Mat TargetImage,
	int Case = 3)
{
	if (int Valuer;
		VRAI != (Valuer = ClientApi::OuvreInventaire(Case)))
	{
		return Valuer;
	}

	auto PrendsEtJete = [](const VALLOC& InfoMatchée)
	{
		MouseEvent(InfoMatchée.Location + Point{ 10, 10 }, CLIC_GAUCHE, 400);
		MouseEvent(POS_VOID, CLIC_GAUCHE, 400);
		KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);
	};

	if (VALLOC InfoMatchée;
		MatchTemplate(SourceImageClient4, TargetImage, &InfoMatchée))
	{
		PrendsEtJete(InfoMatchée);
		__RENDRE_VRAI;
	}
	else
	{
		if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "button//expanding_inventory.jpg"), &InfoMatchée))
		{
			MouseEvent(InfoMatchée.Location + Point{ 6, 6 }, CLIC_GAUCHE);
			if (MatchTemplate(SourceImageClient4, TargetImage, &InfoMatchée))
			{
				PrendsEtJete(InfoMatchée);
				__RENDRE_VRAI;
			}
		}
	}

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Inventory);
	__RENDRE_FAUX;
}
void ClientApi::SupprimerBuf(void)
{
	for (int p = 10; p >= 0; p--)
	{
		MouseEvent({ 1350, 45 }, CLIC_DROIT);
	}
}
#define TEMPS_POUR_ATTENDRE_MAP (10)
void ClientApi::ChangerChaîne(
	bool EstDroit)
{
	BYTE CléDeDirection = EstDroit ? VK_RIGHT : VK_LEFT;
	KeybdEvent({ VK_ESCAPE, VK_RETURN, CléDeDirection, VK_RETURN });
	Sleep(3000);
}

class MinimapRecognizer
{
public:
	class CharacterNotFoundException : public ClientApi::Exception
	{
	public:
		CharacterNotFoundException(void)
			: ClientApi::Exception(__CLASSNAME__) {}
		virtual const char* what(void)
		{
			return Message.c_str();
		}
	};
	class FinDeTimeException : public ClientApi::Exception
	{
	public:
		FinDeTimeException(void)
			: ClientApi::Exception(__CLASSNAME__)
		{
			for (BYTE k : {VK_UP, VK_LEFT, VK_DOWN, VK_RIGHT})
			{
				KeybdEventUp(k);
			}
		}
		virtual const char* what(void)
		{
			return Message.c_str();
		}
	};

public:
	MinimapRecognizer(
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
	int OùSuisJe(void)
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
		throw CharacterNotFoundException();
	};
	void BougerÀCritèreRelatif(
		int MinimapRelativeCriteria)
	{
		int CurrentPosition = OùSuisJe();
		int RelativeDistance = CurrentPosition - MinimapRelativeCriteria;
		const auto StartTime = system_clock::now();

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
					RelativeDistance = OùSuisJe() - MinimapRelativeCriteria + 2;
					if (duration_cast<seconds>(system_clock::now() - StartTime) > seconds(90))
					{
						throw FinDeTimeException();
					}
				}
				KeybdEventUp(VK_RIGHT);
			}
			else if (RelativeDistance > 0)
			{
				KeybdEventDown(VK_LEFT);
				while (RelativeDistance >= 0)
				{
					RelativeDistance = OùSuisJe() - MinimapRelativeCriteria - 2;
					if (duration_cast<seconds>(system_clock::now() - StartTime) > seconds(90))
					{
						throw FinDeTimeException();
					}
				}
				KeybdEventUp(VK_LEFT);
			}
		}
	}

private:
	RECT RectMinimap;
};
