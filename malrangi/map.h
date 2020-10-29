#pragma once
#include "client_api.h"

class Pont
{
public:
	enum
	{
		Unknown = -1,
		Urus,
		Elnas,
		ElnasMarket,
		FreeMarket,
		Zacum1,
		Zacum2_2,
		Zacum3_2,
		RootAbyss1,
		RootAbyss2_1,
		RootAbyss3_1,
		RootAbyss2_3,
		RootAbyss3_3,
		RootAbyss2_4,
		RootAbyss3_4,
		VonLeon1,
		VonLeon2,
		VonLeon3_1,
		VonLeon3_2,
		VonLeon3_3,
		Kaung1,
		Kaung2,
		Papulatus1,
		Papulatus2_1,
		Papulatus2_2,
		MAX_MAPS
	};
	map<int, string> MapFileName;
	Pont()
	{
		MapFileName[Urus] = "urus";
		MapFileName[Elnas] = "elnas";
		MapFileName[ElnasMarket] = "elnas_market";
		MapFileName[FreeMarket] = "freemarket";
		MapFileName[Zacum1] = "z1";
		MapFileName[Zacum2_2] = "z2_2";
		MapFileName[Zacum3_2] = "z3_2";
		MapFileName[RootAbyss1] = "r1";
		MapFileName[RootAbyss2_1] = "r2_1";
		MapFileName[RootAbyss3_1] = "r3_1";
		MapFileName[RootAbyss2_3] = "r2_3";
		MapFileName[RootAbyss3_3] = "r3_3";
		MapFileName[RootAbyss2_4] = "r2_4";
		MapFileName[RootAbyss3_4] = "r3_4";
		MapFileName[VonLeon1] = "v1";
		MapFileName[VonLeon2] = "v2";
		MapFileName[VonLeon3_1] = "v3";
		MapFileName[VonLeon3_2] = "v3";
		MapFileName[VonLeon3_3] = "v3";
		MapFileName[Kaung1] = "k1";
		MapFileName[Kaung2] = "k2";
		MapFileName[Papulatus1] = "p1";
		MapFileName[Papulatus2_1] = "p2";
		MapFileName[Papulatus2_2] = "p2";
	}
	Mat GetMapMat(int NumberOfMap)
	{
		return Read(TARGET_DIR "map//" + MapFileName[NumberOfMap] + ".jpg");
	}
#define RECT_MAP { 0, 0, 300, 300 }
	int WhereAmI()
	{
		array<Mat, MAX_MAPS> ArrTargetImageMap;
		for (int i = 0; i < MAX_MAPS; i++)
		{
			ArrTargetImageMap[i] = GetMapMat(i);
		}

		int MapPrésent;
		for (int p = 0; p < 5; p++)
		{
			auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(RECT_MAP), ArrTargetImageMap);

			if (Unknown != (MapPrésent = Value > 0.9 ? Seq : Unknown))
			{
				break;
			}
			Sleep(0x100);
		}

		return MapPrésent;
	}

	enum
	{
		ZACUM = 1,
		URUS,
		KAUNG = 5,
		PAPULATUS,
		PIERRE,
		VONBAN,
		BLOODY_QUEEN,
		BELLUM,
		VON_LEON
	};

	enum class PONT_EXCEPTION_CODE
	{
		A_INVALID,
		B_MANQUÉ,
		B_INVALID,
		B_INACCESSIBLE,
		INPUT_ERRONÉ
	};

	void BougerDeAÀB(
		const int A,
		const int B,
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		auto CrossWithMirror = [this, B](void)
		{
			string FilePath = TARGET_DIR "pic//mirror//";
			switch (B)
			{
			case Urus:
				FilePath += "urus"; break;
			case RootAbyss1:
				FilePath += "rootabyss"; break;
			}
			FilePath += ".jpg";

			MouseEvent({ 58, 194 }, CLIC_GAUCHE);
			MouseEvent({ 566, 402 }, CLIC_GAUCHE);
			if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, Read(FilePath), &MatchInfo))
			{
				MouseEvent(MatchInfo.Location, CLIC_GAUCHE);
				MouseEvent({ 690, 622 }, CLIC_GAUCHE);
			}
		};
		auto TraverserÀBoss = [this](int FlagDeBoss)
		{
			for (int t = 0; t < 3; t++)
			{
				KeybdEvent(VK_F7);
				METTRE_CURSEUR_À_STDVIDE;

				if (VALLOC mi;
					MatchTemplate(SourceImageClient4, Read(TARGET_DIR "button//boss.jpg"), &mi))
				{
					for (int p = 0; p < FlagDeBoss / 5; p++)
					{
						MouseEvent(mi.Location + Point{ 132, 50 }, CLIC_GAUCHE);
					}
					MouseEvent(mi.Location + Point{ -50 + 40 * (FlagDeBoss % 5), 50 }, CLIC_GAUCHE);
					MouseEvent(mi.Location + Point{ 110, 310 }, CLIC_GAUCHE); // Commencer

					Mat Image = SourceImageClient4;
					if (VALLOC a;
						MatchTemplate(Image, Read(TARGET_DIR "window//rechercher.png"), &a))
					{
						KeybdEvent(VK_RETURN);
					}
					else if (MatchTemplate(Image, Read(TARGET_DIR "window//refus.png")))
					{
						throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
					}
					else
					{
						throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
					}
				}
				else
				{
					throw PONT_EXCEPTION_CODE::B_MANQUÉ;
				}

			__ATTENDRE:
				METTRE_CURSEUR_À_STDVIDE;
				if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, Read(TARGET_DIR "button//confirmation_petite.jpg"), seconds(4)))
				{
					Sleep(0x200);
					KeybdEvent(VK_RETURN);

					int Map;
					switch (FlagDeBoss)
					{
					case ZACUM:
						Map = Zacum1; break;
					case VON_LEON:
						Map = VonLeon2; break;
					case KAUNG:
						Map = Kaung1; break;
					case PAPULATUS:
						Map = Papulatus1; break;
					default:
						throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
					}

				__ATTENDRE2:
					if (WaitUntilMatchingTemplate(
						ClientApi::RECT_CLIENT4,
						GetMapMat(Map),
						seconds(4)))
					{
						return;
					}
					else
					{
						if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "window//waiting_list.png")))
						{
							goto __ATTENDRE2;
						}
						else
						{
							ClientApi::EffaceTousWindows();
							continue;
						}
					}
				}
				else
				{
					if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "window//waiting_list.png")))
					{
						goto __ATTENDRE;
					}
					else
					{
						ClientApi::EffaceTousWindows();
						continue;
					}
				}
			}

			throw PONT_EXCEPTION_CODE::B_MANQUÉ;
		};
		auto TraverserÀR3 = [=](int MillisecondeDeInitialisation)
		{
			auto Marcher = overload_set
			(
				[](initializer_list<BYTE> Keys, seconds MillisecondsKeyDown, int Pas)
				{
					const auto StartTime = system_clock::now();
					KeybdEventDown(VK_RIGHT);
					Sleep(0x40);
					while (duration_cast<seconds>(system_clock::now() - StartTime) < MillisecondsKeyDown)
					{
						KeybdEvent(Keys, Pas);
					}
					KeybdEventUp(VK_RIGHT);
				},
				[](initializer_list<BYTE> Keys, seconds MillisecondsKeyDown, bool EstDroit = true)
				{
					KeybdEventContinued(EstDroit ? VK_RIGHT : VK_LEFT, 0x40);
					const auto StartTime = system_clock::now();
					while (duration_cast<seconds>(system_clock::now() - StartTime) < MillisecondsKeyDown)
					{
						KeybdEvent(Keys, 0x100);
					}
				},
					[](BYTE Key, seconds MillisecondsKeyDown, bool EstDroit = true)
				{
					KeybdEventDown(EstDroit ? VK_RIGHT : VK_LEFT);
					KeybdEventDown(Key);
					this_thread::sleep_for(MillisecondsKeyDown);
					KeybdEventUp(Key);
					KeybdEventUp(EstDroit ? VK_RIGHT : VK_LEFT);
				}
				);

			switch (CharacterInfo.Code)
			{
			default:
				KeybdEventContinued(VK_LEFT, MillisecondeDeInitialisation);
				switch (CharacterInfo.Code)
				{
				default:
					break;

				case USERCONF::CODE_PROPRE::썬콜:
				case USERCONF::CODE_PROPRE::불독:
				case USERCONF::CODE_PROPRE::나로:
				case USERCONF::CODE_PROPRE::키네:
				case USERCONF::CODE_PROPRE::스커:
				case USERCONF::CODE_PROPRE::엔버:
				case USERCONF::CODE_PROPRE::미하일:
				case USERCONF::CODE_PROPRE::윈브:
					KeybdEvent('A', 1200);
					break;

				case USERCONF::CODE_PROPRE::아크:
					KeybdEvent('F', 1200);
					break;

				case USERCONF::CODE_PROPRE::신궁:
				case USERCONF::CODE_PROPRE::카데나:
					KeybdEvent('S', 1200);
					break;

				case USERCONF::CODE_PROPRE::나워:
				case USERCONF::CODE_PROPRE::데벤:
				case USERCONF::CODE_PROPRE::데벤2:
					KeybdEvent('W', 1000);
					break;

				case USERCONF::CODE_PROPRE::배메:
					KeybdEvent('E', 1000);
					break;

				case USERCONF::CODE_PROPRE::루미:
					KeybdEvent('D', 1000);
					break;
				}
				break;

			case USERCONF::CODE_PROPRE::메카닉:
			case USERCONF::CODE_PROPRE::메세:
			case USERCONF::CODE_PROPRE::보마:
				Marcher('Q', seconds(MillisecondeDeInitialisation / 1000), false);
				Sleep(400);
				break;
			}

			switch (CharacterInfo.Code)
			{
			case USERCONF::CODE_PROPRE::닼나:
				Marcher({ 'C' }, seconds(16));
				break;

			case USERCONF::CODE_PROPRE::불독:
			case USERCONF::CODE_PROPRE::썬콜:
				Marcher({ 'Q' }, seconds(32), 1450);
				break;
			case USERCONF::CODE_PROPRE::비숍:
				Marcher({ 'Q' }, seconds(35), 1800);
				break;

			case USERCONF::CODE_PROPRE::신궁:
				Marcher({ 'W' }, seconds(16), 2100);
				KeybdEvent('E');
				KeybdEventContinued(VK_RIGHT, 4000);
				break;

			case USERCONF::CODE_PROPRE::패파:
				Marcher({ 'W' }, seconds(30), 1600);
				break;

			case USERCONF::CODE_PROPRE::섀도어:
				Marcher({ 'C' }, seconds(20));
				break;

			case USERCONF::CODE_PROPRE::나로:
				Marcher({ 'W' }, seconds(30), 1800);
				break;

			case USERCONF::CODE_PROPRE::바이퍼:
				Marcher({ 'C' }, seconds(14));
				break;

			case USERCONF::CODE_PROPRE::소마:
				Marcher({ 'C' }, seconds(12));
				break;

			case USERCONF::CODE_PROPRE::스커:
				Marcher({ 'W' }, seconds(30), 1900);
				break;

			case USERCONF::CODE_PROPRE::팬텀:
				Marcher({ 'C' }, seconds(10), false);
				break;

			case USERCONF::CODE_PROPRE::루미:
				Marcher({ 'A' }, seconds(30), 1800);
				break;

			case USERCONF::CODE_PROPRE::은월:
			case USERCONF::CODE_PROPRE::데슬:
				Marcher({ 'W' }, seconds(30), 1800);
				break;

			case USERCONF::CODE_PROPRE::데벤:
			case USERCONF::CODE_PROPRE::데벤2:
				Marcher({ 'C' }, seconds(10));
				break;

			case USERCONF::CODE_PROPRE::배메:
				Marcher({ 'V' }, seconds(18));
				Sleep(1000);
				break;

			case USERCONF::CODE_PROPRE::아란:
				Marcher({ 'A' }, seconds(30), 1600);
				break;

			case USERCONF::CODE_PROPRE::제논:
				Marcher('W', seconds(20));
				break;

			case USERCONF::CODE_PROPRE::아델:
				Marcher({ 'Q' }, seconds(25), 1800);
				break;
			case USERCONF::CODE_PROPRE::블래:
				Marcher({ 'Q' }, seconds(30), 1500);
				break;

			case USERCONF::CODE_PROPRE::엔버:
				Marcher({ 'W' }, seconds(24), 1800);
				break;

			case USERCONF::CODE_PROPRE::아크:
				Marcher({ 'A' }, seconds(29), 1200);
				break;

			case USERCONF::CODE_PROPRE::호영:
				Marcher({ 'Q' }, seconds(30), 1350);
				break;

			case USERCONF::CODE_PROPRE::제로:
				KeybdEventContinued(VK_RIGHT, 100);
				KeybdEvent({ 'W', 'W', 'E', 'E', 'W', 'W', 'E', 'E', 'W', 'W' }, 1400);
				KeybdEventContinued(VK_RIGHT, 1000);
				break;

			case USERCONF::CODE_PROPRE::키네:
				Marcher({ 'Q' }, seconds(28), 1000);
				break;

			case USERCONF::CODE_PROPRE::카데나:
				Marcher({ 'Q' }, seconds(32), 1800);
				break;

			case USERCONF::CODE_PROPRE::플위:
				KeybdEvent('W', 1000);
				Marcher({ 'Q' }, seconds(28), 1400);
				break;

			case USERCONF::CODE_PROPRE::나워:
				Marcher({ 'A' }, seconds(30), 1800);
				break;

			case USERCONF::CODE_PROPRE::듀블:
				Marcher({ 'A' }, seconds(29), 1400);
				break;

			case USERCONF::CODE_PROPRE::미하일:
				Marcher({ 'C' }, seconds(16));
				break;

			case USERCONF::CODE_PROPRE::윈브:
				Marcher({ 'W' }, seconds(26), 1500);
				break;

			case USERCONF::CODE_PROPRE::보마:
			case USERCONF::CODE_PROPRE::메세:
			case USERCONF::CODE_PROPRE::메카닉:
				Marcher('Q', seconds(22));
				break;
			}
		};
		auto ConfirmerExceptionDeRootAbyss = []()
		{
			if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_deja_accompli.png")))
				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_deja_accompli.png")))
				{
					KeybdEvent(VK_RETURN);
					throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
				}
				else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_non_cle.png")))
				{
					KeybdEvent(VK_RETURN);
					throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
				}
				else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_limite30.png")))
				{
					KeybdEvent(VK_RETURN, VK_RETURN);
					throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
				}
		};
		auto ObtenirCle = []()
		{
			MouseEvent({ 33, 318 }, CLIC_GAUCHE, 1000);

			static const Mat TargetImage1(Read(TARGET_DIR "text//quest_rootabyss.jpg"));
			if (VALLOC MatchInfo;
				MatchTemplate(SourceImageClient4, TargetImage1, &MatchInfo))
			{
				MouseEvent(MatchInfo.Location + Point{ 10, 3 }, CLIC_GAUCHE);
				KeybdEvent({ VK_RETURN, VK_RIGHT, VK_RETURN, VK_RETURN });
			}
			else
			{
				KeybdEvent(VK_ESCAPE);
			}
		};

		int TempsÀAttendre;
		switch (B)
		{
		case Zacum1:
		case Papulatus1:
			TempsÀAttendre = 3000; break;
		default: 
			TempsÀAttendre = 0x300; break;
		}

		if (A == Unknown)
		{
			throw PONT_EXCEPTION_CODE::A_INVALID;
		}

		try
		{
		switch (A)
		{
		case Urus:
			switch (B)
			{
			case Elnas:
			{
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 2800);
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case Elnas:
			switch (B)
			{
			case FreeMarket:
			{
				MouseEvent({ 58, 194, }, CLIC_GAUCHE);
				MouseEvent({ 634, 404 }, CLIC_GAUCHE);
				MouseEvent({ 840, 488 }, CLIC_GAUCHE);
			}
			break;

			case Zacum1:
			{
				TraverserÀBoss(ZACUM);
				ClientApi::OpererParty(CASSER);
			}
			break;

			case Urus:
			case RootAbyss1:
			{
				CrossWithMirror();
			}
			break;

			case VonLeon2:
			{
				TraverserÀBoss(VON_LEON);
				ClientApi::OpererParty(REFAIRE);
			}
			break;

			case Kaung1:
			{
				TempsÀAttendre = 0;
				TraverserÀBoss(KAUNG);
				ClientApi::OpererParty(REFAIRE);
			}
			break;

			case Papulatus1:
			{
				TraverserÀBoss(PAPULATUS);
				ClientApi::OpererParty(REFAIRE);
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case ElnasMarket:
			switch (B)
			{
			case Elnas:
			{
				MinimapRecognizer Recognizer({ 10, 80, 260, 150 });

				Recognizer.BougerÀCritèreRelatif(20);
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
			}
			break;

			case Zacum1:
			{
				TraverserÀBoss(ZACUM);
				ClientApi::OpererParty(CASSER);
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case FreeMarket:
			switch (B)
			{
			case ElnasMarket:
			{
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
				if (!WaitUntilMatchingTemplate(
					RECT_MAP,
					GetMapMat(B),
					seconds(10)))
				{
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 3000);
				}
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case Zacum1:
			switch (B)
			{
			case Zacum2_2:
			{
				MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
				if (Recognizer.OùSuisJe() < 100)
				{
					Recognizer.BougerÀCritèreRelatif(143);
				}

				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1200);
				KeybdEvent(VK_DOWN);
				KeybdEvent(VK_RETURN);
			}
			break;

			case Elnas:
			{
				MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
				if (Recognizer.OùSuisJe() < 100)
				{
					Recognizer.BougerÀCritèreRelatif(143);
				}

				MouseEvent({ 820, 589 }, CLIC_GAUCHE);
				KeybdEvent({ VK_DOWN, VK_DOWN, VK_DOWN, VK_RETURN, VK_RETURN, VK_RETURN });
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case Zacum2_2:
			switch (B)
			{
			case Zacum1:
			{
				TempsÀAttendre = 2000;
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 900);
			}
			break;

			case Zacum3_2:
			{
				int CountDeEssai = 0;
				int CodeOperation = FAIRE;

			__REESSAYER1_Z__:
				ClientApi::OpererParty(CodeOperation);
			__REESSAYER2_Z__:
				if (CountDeEssai++ > 8)
				{
					throw PONT_EXCEPTION_CODE::B_INVALID;
				}

				MouseEvent({ 1000, 570 }, CLIC_GAUCHE);
				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_z1.png")))
				{
					// 파티장만 입장 신청을 할 수 있습니다
					KeybdEvent(VK_ESCAPE);
					CodeOperation = REFAIRE;

					goto __REESSAYER1_Z__;
				}

				for (int p = 1; p <= 2; p++)
				{
					for (int q = 1; q <= p; q++)
					{
						KeybdEvent(VK_RETURN);
						MouseEvent({ 500, 400 }, CLIC_GAUCHE, 100);
					}

					if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_z2.png")))
					{
						// 파티원 중 X님이 오늘 자쿰의 제단에 입장하셔서 들어갈 수 없습니다
						throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
					}
					else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_non_cle.png")))
					{
						// 파티원 중 일부가 같은 맵에 있지 않습니다
						KeybdEvent(VK_ESCAPE);
						CodeOperation = REFAIRE;

						goto __REESSAYER1_Z__;
					}
					else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_plein.png")))
					{
						KeybdEvent(VK_ESCAPE);
						ClientApi::ChangerChaîne(false);

						goto __REESSAYER2_Z__;
					}
				}
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case Zacum3_2:
			switch (B)
			{
			case Zacum2_2:
			{
				ClientApi::OpererParty(CASSER);
			}
			break;
			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case RootAbyss1:
			switch (B)
			{
			case RootAbyss2_1:
			case RootAbyss2_3:
			case RootAbyss2_4:
			{
				KeybdEvent(VK_ESCAPE);
				MouseEvent(POS_VOID, CLIC_GAUCHE);
				ClientApi::OpererParty(FAIRE);
				KeybdEvent(VK_CONTROL);

				ObtenirCle();

				switch (CharacterInfo.Code)
				{
				case USERCONF::CODE_PROPRE::메카닉:
				case USERCONF::CODE_PROPRE::엔버:
					KeybdEvent('W', 1000);
					break;
				case USERCONF::CODE_PROPRE::제로:
					KeybdEvent('X', 1000);
					break;
				}

				switch (B)
				{
				case RootAbyss2_1:
					KeybdEventContinued(VK_LEFT, GET_DURATION(450000, CharacterInfo.Vitesse));
					KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 800);
					break;

				case RootAbyss2_3:
					KeybdEventContinued(VK_RIGHT, GET_DURATION(100000, CharacterInfo.Vitesse));
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 800);
					break;

				case RootAbyss2_4:
					KeybdEventContinued(VK_RIGHT, GET_DURATION(450000, CharacterInfo.Vitesse));
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 800);
					break;
				}
				KeybdEvent(VK_RETURN, 800);

				ConfirmerExceptionDeRootAbyss();
			}
			break;

			case Zacum1:
			{
				KeybdEvent(VK_ESCAPE);
				MouseEvent(POS_VOID, CLIC_GAUCHE);
				TraverserÀBoss(ZACUM);
				ClientApi::OpererParty(CASSER);
			}
			break;

			case Elnas:
			{
				KeybdEventContinued(VK_RIGHT, GET_DURATION(640000, CharacterInfo.Vitesse));
				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 800);
			}
			break;

			default:
				throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case RootAbyss2_1:
		case RootAbyss2_4:
			switch (B)
			{
			case RootAbyss3_1:
			case RootAbyss3_4:
			{
				TraverserÀR3(1800);

				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 3600);
				KeybdEvent({ VK_RIGHT, VK_RETURN });
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case RootAbyss2_3:
			switch (B)
			{
			case RootAbyss3_3:
			{
				TraverserÀR3(3800);

				Sleep(800);
#define GET_DURATION(_d, _ms) (_d / _ms)
				KeybdEventContinued(VK_LEFT, GET_DURATION(90000, CharacterInfo.Vitesse));
				Sleep(800);
				switch (CharacterInfo.Code)
				{
				case USERCONF::CODE_PROPRE::닼나:
				case USERCONF::CODE_PROPRE::섀도어:
				case USERCONF::CODE_PROPRE::소마:
				case USERCONF::CODE_PROPRE::나워:
				case USERCONF::CODE_PROPRE::데벤:
				case USERCONF::CODE_PROPRE::데벤2:
					KeybdEvent('W', 1200);
					break;

				case USERCONF::CODE_PROPRE::패파:
					KeybdEvent('Q', 1000);
					break;

				case USERCONF::CODE_PROPRE::썬콜:
					KeybdEvent('S', 1000);
					break;

				case USERCONF::CODE_PROPRE::불독:
				case USERCONF::CODE_PROPRE::바이퍼:
				case USERCONF::CODE_PROPRE::캐슈:
				case USERCONF::CODE_PROPRE::플위:
				case USERCONF::CODE_PROPRE::윈브:
					KeybdEvent('Z', 1200);
					break;

				default:
					break;
				}
				KeybdEvent(VK_MENU, 1000);
				KeybdEvent({ VK_UP, VK_RIGHT, VK_RETURN });
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case RootAbyss3_1:
		case RootAbyss3_3:
		case RootAbyss3_4:
			switch (B)
			{
			case RootAbyss1:
			{
				ClientApi::OpererParty(CASSER);
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case VonLeon1:
			switch (B)
			{
			case Elnas:
			{
				MinimapRecognizer Recognizer({ 10, 100, 200, 146 });
				Recognizer.BougerÀCritèreRelatif(15);
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
			}
			break;
			}
			break;

		case VonLeon2:
			switch (B)
			{
			case VonLeon1:
			{
				KeybdEventContinued(VK_LEFT, GET_DURATION(500000, CharacterInfo.Vitesse));
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
			}
			break;

			case VonLeon3_1:
			case VonLeon3_2:
			case VonLeon3_3:
			{
				if (B == VonLeon3_3)
				{
					ClientApi::ChangerChaîne(false);
				}

				int t = 0;
				int CodeOperation = NULL;
			__REESSAYER1_V__:
				ClientApi::OpererParty(CodeOperation);

				if (t > 0)
				{
					KeybdEvent(VK_UP);
				}
				else
				{
				__REESSAYER2_V__:
					KeybdEventContinued(VK_RIGHT, GET_DURATION(625000, CharacterInfo.Vitesse));
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1000);
				}

				if (t++ > 8)
				{
					throw PONT_EXCEPTION_CODE::B_MANQUÉ;
				}

				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//non_party_v.png")))
				{
					KeybdEvent(VK_ESCAPE);
					CodeOperation = REFAIRE;

					goto __REESSAYER1_V__;
				}

				KeybdEvent(VK_RETURN);
				for (int p = 0; p < B - VonLeon3_1; p++)
				{
					KeybdEvent(VK_DOWN);
				}

				KeybdEvent(VK_RETURN);
				MouseEvent({ 500, 400 }, CLIC_GAUCHE, 100);
				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_plein.png")))
				{
					KeybdEvent(VK_RETURN);
					ClientApi::ChangerChaîne(false);

					goto __REESSAYER2_V__;
				}
			}
			break;
			}
			break;

		case VonLeon3_1:
		case VonLeon3_2:
		case VonLeon3_3:
			switch (B)
			{
			case VonLeon1:
			{
				ClientApi::OpererParty(CASSER);
			}
			break;
			}
			break;

		case Kaung1:
			switch (B)
			{
			case Elnas:
			{
				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, GET_DURATION(180000, 140));
			}
			break;

			case Kaung2:
			{
				int CountDeEssai = 0;
				int CodeOperation = NULL;

			__REESSAYER1_K__:
				ClientApi::OpererParty(CodeOperation);
				KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.Potion);
			__REESSAYER2_K__:
				if (CountDeEssai++ > 8)
				{
					throw PONT_EXCEPTION_CODE::B_INVALID;
				}

				Sleep(600);
				MouseEvent({ 870, 590 }, CLIC_GAUCHE);
				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_z1.png")))
				{
					// 파티장만 입장 신청을 할 수 있습니다
					KeybdEvent(VK_ESCAPE);
					CodeOperation = REFAIRE;

					goto __REESSAYER1_K__;
				}

				KeybdEvent(VK_RETURN, 200);
				MouseEvent({ 500, 400 }, CLIC_GAUCHE, 100);
				if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_non_cle.png")))
				{
					// 파티원 중 이미 입장한 사람이 있어
					throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
				}
				else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_plein.png")))
				{
					KeybdEvent(VK_ESCAPE);
					ClientApi::ChangerChaîne(false);

					goto __REESSAYER2_K__;
				}

				return;
			}
			break;
			}
			break;

		case Kaung2:
			switch (B)
			{
			case Kaung1:
			{
				ClientApi::OpererParty(CASSER);
			}
			break;
			}
			break;

		case Papulatus1:
			switch (B)
			{
			case Elnas:
			{
				MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
				Recognizer.BougerÀCritèreRelatif(20);

				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
			}
			break;

			case Papulatus2_1:
			case Papulatus2_2:
			{
				int CountDeEssai = 0;
				int CodeOperation = NULL;

			__REESSAYER1_P__:
				ClientApi::OpererParty(CodeOperation);
			__REESSAYER2_P__:
				if (CountDeEssai++ > 8)
				{
					throw PONT_EXCEPTION_CODE::B_INVALID;
				}

				MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
				Recognizer.BougerÀCritèreRelatif(121);
				Sleep(800);

				KeybdEvent({ VK_MENU, VK_MENU, VK_MENU, VK_MENU }, 800);
				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 900);

				for (int i = 0; i < B - Papulatus2_1; i++)
				{
					KeybdEvent(VK_DOWN);
				}
				for (int p = 1; p <= 2; p++)
				{
					for (int q = 1; q <= p; q++)
					{
						KeybdEvent(VK_RETURN);
						MouseEvent({ 500, 400 }, CLIC_GAUCHE, 100);
					}

					if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_z1.png")))
					{
						// 파티장만 입장 신청을 할 수 있습니다
						KeybdEvent(VK_ESCAPE);
						CodeOperation = REFAIRE;

						goto __REESSAYER1_P__;
					}
					else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//r_non_cle.png")))
					{
						// 파티원 중 x님이 오늘 시계탑의 근원에 입장했습니다.
						KeybdEvent(VK_ESCAPE);
						CodeOperation = REFAIRE;

						throw PONT_EXCEPTION_CODE::B_INACCESSIBLE;
					}
					else if (MatchTemplate(SourceImageClient4, Read(TARGET_DIR "text//exception_plein.png")))
					{
						KeybdEvent(VK_ESCAPE);
						ClientApi::ChangerChaîne(false);

						goto __REESSAYER2_P__;
					}
				}
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		case Papulatus2_1:
		case Papulatus2_2:
			switch (B)
			{
			case Papulatus1:
			{
				TempsÀAttendre = 2000;
				ClientApi::OpererParty(CASSER);
			}
			break;

			default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
			}
			break;

		default: throw PONT_EXCEPTION_CODE::INPUT_ERRONÉ;
		}

		if (!WaitUntilMatchingTemplate(
			RECT_MAP,
			GetMapMat(B),
			seconds(10),
			64,
			0.96))
		{
			throw MatchTemplate(Capture(RECT_MAP), GetMapMat(A)) ?
				PONT_EXCEPTION_CODE::B_MANQUÉ : PONT_EXCEPTION_CODE::B_INVALID;
		}

		Sleep(TempsÀAttendre);
		}
		catch (MinimapRecognizer::CharacterNotFoundException&)
		{
			throw PONT_EXCEPTION_CODE::B_INVALID;
		}
		catch (MinimapRecognizer::FinDeTimeException&)
		{
			throw PONT_EXCEPTION_CODE::B_INVALID;
		}
	}
};