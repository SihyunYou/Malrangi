#pragma once
#include "client.h"

class Bridge
{
	template <class F1, class F2>
	struct overload_set : F1, F2
	{
		overload_set(F1 f1, F2 f2)
			: F1(f1), F2(f2)
		{}

		using F1::operator();
		using F2::operator();
	};

	template <class F1, class F2>
	overload_set<F1, F2> overload(F1 f1, F2 f2)
	{
		return overload_set<F1, F2>(f1, f2);
	}

public:
	enum
	{
		Unknown = -1,
		Urus,
		Elnas,
		ElnasMarket,
		Zacum1,
		Zacum2_2,
		Zacum3_2,
		RootAbyss1,
		RootAbyss2_1,
		RootAbyss3_1,
		RootAbyss2_3,
		RootAbyss3_3,
		FreeMarket,
		MAX_MAPS
	};
	map<int, string> MapFileName;
	Bridge()
	{
		MapFileName[Urus] = "urus";
		MapFileName[Elnas] = "elnas";
		MapFileName[ElnasMarket] = "elnas_market";
		MapFileName[Zacum1] = "z1";
		MapFileName[Zacum2_2] = "z2_2";
		MapFileName[Zacum3_2] = "z3_2";
		MapFileName[RootAbyss1] = "r1";
		MapFileName[RootAbyss2_1] = "r2_1";
		MapFileName[RootAbyss3_1] = "r3_1";
		MapFileName[RootAbyss2_3] = "r2_3";
		MapFileName[RootAbyss3_3] = "r3_3";
		MapFileName[FreeMarket] = "freemarket";
	}
	Mat GetMapMat(int NumberOfMap)
	{
		return Read(TARGET_DIR "map//" + MapFileName[NumberOfMap] + ".jpg");
	}
#define RECT_MAP { 0, 0, 300, 300 }
	int WhereAmI()
	{
		static array<Mat, MAX_MAPS> ArrTargetImageMap;
		for (int i = 0; i < MAX_MAPS; i++)
		{
			ArrTargetImageMap[i] = GetMapMat(i);
		}

		auto [Seq, Value, Location] = GetHighestMatchedTemplate(Capture(RECT_MAP), ArrTargetImageMap);
		return Value > 0.9 ? Seq : Unknown;
	}
	bool AmIIn(int NumberOfMap)
	{
		return MatchTemplate(Capture(RECT_MAP), GetMapMat(NumberOfMap), nullptr, 0.96);
	}

	enum class BRIDGE_EXCEPTION_CODE
	{
		A_FAIL,
		A_ERRORINPUT,
		B_FAIL,
		B_ERRORINPUT,
		DEJA_ACCOMPLI,
		NULL_PARAMETRE
	};
	
	void BougerDeAAB(
		const int A, const int B, const LPVOID Reserved = nullptr)
	{
		auto Cross = [this, A, B](auto CrossRoutine)
		{
			ClientApi::SET_CLIENT_STDPOS();
			if (AmIIn(A))
			{
				CrossRoutine();
				
				if (WaitUntilMatchingTemplate(
					RECT_MAP,
					GetMapMat(B),
					seconds(32)))
				{
					Sleep(0x400);
				}
				else
				{
					throw BRIDGE_EXCEPTION_CODE::B_FAIL;
				}
			}
			else
			{
				throw BRIDGE_EXCEPTION_CODE::A_FAIL;
			}
		};
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

			MouseEvent({ 58, 194 }, LEFT_CLICK);
			MouseEvent({ 566, 402 }, LEFT_CLICK);
			if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, Read(FilePath), &MatchInfo))
			{
				MouseEvent(MatchInfo.Location, LEFT_CLICK);
				MouseEvent({ 690, 622 }, LEFT_CLICK);
			}
		};
#if defined(BUILD_DAILYBOSS)
		auto CharacterInfo = static_cast<const USERCONF::CHARACTER_INFO*>(Reserved);
		auto CrossToZacum = [&, this](void)
		{
			static const Mat TargetImageButtonBoss = Read(TARGET_DIR "button//boss.jpg");
			static const Mat TargetImageWindowEnterRefuse = Read(TARGET_DIR "window//enter_refuse.jpg");
			static const Mat TargetImageButtonSmallEnter = Read(TARGET_DIR "button//small_enter.jpg");
			static const Mat TargetImageWindowWaitingListZacum = Read(TARGET_DIR "window\\waiting_list_zacum.jpg");
			bool IsRetry = false;

		__RETRY__MATCH:
			KeybdEvent(VK_F7);
			MouseEvent(POS_VOID, CURSOR_MOVE);
			if (VALLOC MatchInfo;
				MatchTemplate(SourceImageClient4, TargetImageButtonBoss, &MatchInfo))
			{
				MouseEvent(MatchInfo.Location + Point{ -5, 50 }, LEFT_CLICK);
				MouseEvent(MatchInfo.Location + Point{ 110, 310 }, LEFT_CLICK);

				if (MatchTemplate(SourceImageClient4, TargetImageWindowEnterRefuse, &MatchInfo))
				{
					throw BRIDGE_EXCEPTION_CODE::B_FAIL;
				}
				else
				{
					KeybdEvent(VK_RETURN);
				}
			}

			MouseEvent(POS_VOID, CURSOR_MOVE);
			if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageButtonSmallEnter, seconds(150)))
			{
				Sleep(400);
				KeybdEvent(VK_RETURN);

				if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, GetMapMat(Zacum1), seconds(36)))
				{
					ClientApi::MoveServer(true);
					if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, GetMapMat(Zacum1), seconds(36)))
					{
						Sleep(4000);
						ClientApi::BreakParty();

						return;
					}
				}
				else
				{
					if (!MatchTemplate(SourceImageClient4, TargetImageWindowWaitingListZacum))
					{
						if (!IsRetry)
						{
							IsRetry = true;
							goto __RETRY__MATCH;
						}
					}
				}
			}

			throw BRIDGE_EXCEPTION_CODE::B_FAIL;
		};
		auto CrossToR3 = [&, this](void)
		{
			KeybdEventContinued(VK_LEFT, 4000);

			switch (CharacterInfo->Code)
			{
			default:
				KeybdEventContinued(VK_RIGHT, 88);
				break;
			case USERCONF::CODE_PROPRE::팬텀:
			case USERCONF::CODE_PROPRE::제논:
			case USERCONF::CODE_PROPRE::보마:
			case USERCONF::CODE_PROPRE::메세:
			case USERCONF::CODE_PROPRE::메카닉:
				break;
			}

			switch (CharacterInfo->Code)
			{
			case USERCONF::CODE_PROPRE::닼나:
				for (int q = 0; q < 13; q++)
				{
					KeybdEvent('C', 960);
				}
				break;

			case USERCONF::CODE_PROPRE::불독:
			case USERCONF::CODE_PROPRE::썬콜:
				KeybdEvent('3', 1000);
				for (int q = 0; q < 15; q++)
				{
					KeybdEvent('S', 800);
					KeybdEventContinued(VK_RIGHT, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::비숍:
				for (int q = 0; q < 15; q++)
				{
					KeybdEvent('Q', 800);
					KeybdEventContinued(VK_RIGHT, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::신궁:
				for (int q = 0; q < 16; q++)
				{
					KeybdEvent('W', 800);
					KeybdEventContinued(VK_RIGHT, 900);
				}
				break;

			case USERCONF::CODE_PROPRE::나로:
				for (int q = 0; q < 17; q++)
				{
					KeybdEvent('W', 800);
					KeybdEventContinued(VK_RIGHT, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::바이퍼:
				KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
				break;

			case USERCONF::CODE_PROPRE::소마:
				KeybdEvent('3');
				KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'W' }, 1000);
				break;

			case USERCONF::CODE_PROPRE::스커:
				for (int q = 0; q < 13; q++)
				{
					KeybdEvent({ 'X', 'C' }, 400);
				}
				break;

			case USERCONF::CODE_PROPRE::팬텀:
				KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
				break;

			case USERCONF::CODE_PROPRE::은월:
				for (int q = 0; q < 18; q++)
				{
					KeybdEvent('W', 800);
					KeybdEventContinued(VK_RIGHT, 900);
				}
				break;

			case USERCONF::CODE_PROPRE::데슬:
				for (int q = 0; q < 17; q++)
				{
					KeybdEvent('W', 800);
					KeybdEventContinued(VK_RIGHT, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::데벤:
				KeybdEvent({ 'W', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'W' }, 920);
				break;

			case USERCONF::CODE_PROPRE::배메:
				for (int q = 0; q < 9; q++)
				{
					KeybdEvent({ 'X', 'Z' }, 900);
				}
				break;

			case USERCONF::CODE_PROPRE::아란:
				for (int q = 0; q < 16; q++)
				{
					KeybdEvent('A', 800);
					KeybdEventContinued(VK_RIGHT, 900);
				}
				break;

			case USERCONF::CODE_PROPRE::제논:
				KeybdEventDown(VK_RIGHT);
				KeybdEventDown('W');
				Sleep(20000);
				KeybdEventUp('W');
				KeybdEventUp(VK_RIGHT);
				break;

			case USERCONF::CODE_PROPRE::보마:
			case USERCONF::CODE_PROPRE::메세:
			case USERCONF::CODE_PROPRE::메카닉:
				KeybdEventDown(VK_RIGHT);
				KeybdEventDown('Q');
				Sleep(20000);
				KeybdEventUp('Q');
				KeybdEventUp(VK_RIGHT);
				break;

			case USERCONF::CODE_PROPRE::블래:
				for (int q = 0; q < 20; q++)
				{
					KeybdEvent('Q', 800);
					KeybdEventContinued(VK_RIGHT, 800);
				}
				break;

			case USERCONF::CODE_PROPRE::엔버:
				for (int q = 0; q < 13; q++)
				{
					KeybdEvent('W', 800);
					KeybdEventContinued(VK_RIGHT, 1200);
				}
				break;

			case USERCONF::CODE_PROPRE::아크:
				for (int q = 0; q < 20; q++)
				{
					KeybdEvent('A', 600);
					KeybdEventContinued(VK_RIGHT, 900);
				}
				break;

			case USERCONF::CODE_PROPRE::제로:
				KeybdEvent({ 'W', 'W', 'E', 'E', 'W', 'W', 'E', 'E', 'W', 'W', 'Z' }, 1400);
				KeybdEventContinued(VK_RIGHT, 1000);
				break;

			case USERCONF::CODE_PROPRE::키네:
				for (int q = 0; q < 15; q++)
				{
					KeybdEvent('C', 1000);
					KeybdEventContinued(VK_RIGHT, 1100);
				}
				break;

			case USERCONF::CODE_PROPRE::플위:
				for (int q = 0; q < 16; q++)
				{
					KeybdEvent('Q', 720);
					KeybdEventContinued(VK_RIGHT, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::나워:
				for (int q = 0; q < 20; q++)
				{
					KeybdEvent('A', 800);
					KeybdEventContinued(VK_RIGHT, 800);
				}
				break;

			case USERCONF::CODE_PROPRE::듀블:
				for (int q = 0; q < 13; q++)
				{
					KeybdEvent({ 'F', 'W' }, 1000);
				}
				break;

			case USERCONF::CODE_PROPRE::미하일:
				KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
				break;

			case USERCONF::CODE_PROPRE::윈브:
				for (int q = 0; q < 13; q++)
				{
					KeybdEvent({ 'W', 'C' }, 1000);
				}
				break;

			default:
				throw BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE;
			}
		};
#endif

		switch (A)
		{
#if defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
		case Urus:
			switch (B)
			{
			case Elnas:
				Cross(
					[](){
						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 4000);
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
#endif

		case Elnas:
			switch (B)
			{
#if defined(BUILD_DAILYBOSS)
			case Zacum1:
				CrossToZacum();
				return;
#endif
#if defined(BUILD_URUS)
			case Urus:
#endif
#if defined(BUILD_DAILYBOSS)
			case RootAbyss1:
#endif
				Cross([&CrossWithMirror, B]()
					{
						CrossWithMirror();
					});
				return;

#if defined(BUILD_CALC)
			case FreeMarket:
				Cross([]()
					{
						MouseEvent({ 58, 194, }, LEFT_CLICK);
						MouseEvent({ 634, 404 }, LEFT_CLICK);
						MouseEvent({ 840, 488 }, LEFT_CLICK);
					});
				return;
#endif
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case Zacum1:
			switch (B)
			{
#if defined(BUILD_DAILYBOSS)
			case Zacum2_2:
				Cross([Level = Zacum2_2 == B ? 1 : 2]()
				{
					try
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
						if (Recognizer.WhereAmI() < 100)
						{
							Recognizer.MoveToRelativeCriteria(143);
						}
					}
					catch (ClientApi::MinimapRecognizer::CharacterNotFoundException&)
					{
						throw BRIDGE_EXCEPTION_CODE::B_FAIL;
					}

					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1200);
					for (int i = 0; i < (int)Level; i++)
					{
						KeybdEvent(VK_DOWN);
					}
					KeybdEvent(VK_RETURN);
				});
				return;
#endif
			case Elnas:
				Cross([]()
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
						if (Recognizer.WhereAmI() < 100)
						{
							Recognizer.MoveToRelativeCriteria(143);
						}

						MouseEvent({ 820, 589 }, LEFT_CLICK);
						KeybdEvent({ VK_DOWN, VK_DOWN, VK_DOWN, VK_RETURN, VK_RETURN, VK_RETURN }, 400);
					});
				return;
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case Zacum2_2:
			switch (B)
			{
			case Zacum1:
				Cross([]()
					{
						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
					});
				return;

#if defined(BUILD_DAILYBOSS)
			case Zacum3_2:
				Cross(
					[]() {
						ClientApi::MakeParty();

						MouseEvent({ 1000, 570 }, LEFT_CLICK, 600);
						KeybdEvent({ VK_RETURN, VK_RETURN, VK_RETURN }, 600);
					});
				return;
#endif
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

#if defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
		case Zacum3_2:
			switch (B)
			{
			case Zacum2_2:
				Cross(
					[](){
						ClientApi::BreakParty();
					});
				return;
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
#endif

		case FreeMarket:
			switch (B)
			{
			case ElnasMarket:
				if (AmIIn(A))
				{
					KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);

					if (WaitUntilMatchingTemplate(
						RECT_MAP,
						GetMapMat(B),
						seconds(32)))
					{
						Sleep(0x400);
					}
					else
					{
						KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 3000);

						if (WaitUntilMatchingTemplate(
							RECT_MAP,
							GetMapMat(B),
							seconds(32)))
						{
							Sleep(0x400);
						}
						else
						{
							throw BRIDGE_EXCEPTION_CODE::B_FAIL;
						}
					}
				}
				else
				{
					throw BRIDGE_EXCEPTION_CODE::A_FAIL;
				}
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case ElnasMarket:
			switch (B)
			{
#if defined(BUILD_URUS) || defined(BUILD_DAILYBOSS)
			case Elnas:
				Cross(
					[](){
						ClientApi::MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
						Recognizer.MoveToRelativeCriteria(20);

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
					});
				return;
#endif
#if defined(BUILD_DAILYBOSS)
			case Zacum1:
				CrossToZacum();
				return;
#endif
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case RootAbyss1:
			switch (B)
			{
#if defined(BUILD_DAILYBOSS)
			case RootAbyss2_1:
				if (nullptr == CharacterInfo)
				{
					throw BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE;
				}
				Cross([CharacterInfo]()
					{
						ClientApi::MakeParty();

						KeybdEvent(VK_CONTROL);
						switch (CharacterInfo->Code)
						{
						default:
							break;
						case USERCONF::CODE_PROPRE::메카닉:
						case USERCONF::CODE_PROPRE::엔버:
							KeybdEvent('W', 1000);
						}

						// 고목나무 열쇠 받기
						MouseEvent({ 33, 318 }, LEFT_CLICK, 1000);

						static const Mat TargetImage1(Read(TARGET_DIR "text//quest_rootabyss.jpg"));
						if (VALLOC MatchInfo; 
							MatchTemplate(SourceImageClient4, TargetImage1, &MatchInfo))
						{
							MouseEvent(MatchInfo.Location + Point{ 10, 3 }, LEFT_CLICK);
							KeybdEvent({ VK_RETURN, VK_RIGHT, VK_RETURN, VK_RETURN });
						}
						else
						{
							KeybdEvent(VK_ESCAPE);
						}

						try
						{
							ClientApi::MinimapRecognizer Recognizer({ 10, 126, 200, 146 });
							Recognizer.MoveToRelativeCriteria(62);
						}
						catch (ClientApi::MinimapRecognizer::CharacterNotFoundException&)
						{
							throw BRIDGE_EXCEPTION_CODE::B_FAIL;
						}

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
						KeybdEvent(VK_RETURN);
					});
				return;

			case RootAbyss2_3:
				if (nullptr == CharacterInfo)
				{
					throw BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE;
				}
				Cross([CharacterInfo]()
					{
						KeybdEvent(VK_ESCAPE);
						MouseEvent(POS_VOID, LEFT_CLICK);
						ClientApi::MakeParty();

						switch (CharacterInfo->Code)
						{
						default:
							break;
						case USERCONF::CODE_PROPRE::메카닉:
						case USERCONF::CODE_PROPRE::엔버:
							KeybdEvent('W', 1000);
						}

						KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1600);
						KeybdEvent(VK_RETURN);
						MouseEvent({ 700, 400 }, LEFT_CLICK);
					});
				return;

			case Zacum1:
				KeybdEvent(VK_ESCAPE);
				MouseEvent(POS_VOID, LEFT_CLICK);
				CrossToZacum();
				return;
#endif
#if defined(BUILD_URUS) || defined(BUILD_CALC)
			case Elnas:
				Cross([]()
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 126, 200, 146 });
						Recognizer.MoveToRelativeCriteria(43);
						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
					});
				return;
#endif
			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

#if defined(BUILD_DAILYBOSS)
		case RootAbyss2_1:
			switch (B)
			{
			case RootAbyss3_1:
				Cross([&CrossToR3]()
					{
						CrossToR3();

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 3600);
						KeybdEvent({ VK_RIGHT, VK_RETURN });
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
#endif
#if defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
		case RootAbyss3_1:
		case RootAbyss3_3:
			switch (B)
			{
			case RootAbyss1:
				Cross(
					[](){
						ClientApi::BreakParty();
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
#endif

#if defined(BUILD_DAILYBOSS)
		case RootAbyss2_3:
			switch (B)
			{
			case RootAbyss3_3:
				if (nullptr == CharacterInfo)
				{
					throw BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE;
				}
				Cross([&CrossToR3, &CharacterInfo]()
					{
						CrossToR3();

						Sleep(800);
#define GET_DURATION(_d, _ms) (_d / _ms)
						KeybdEventContinued(VK_LEFT, GET_DURATION(92000, CharacterInfo->Speed));
						Sleep(800);
						switch (CharacterInfo->Code)
						{
						case USERCONF::CODE_PROPRE::닼나:
						case USERCONF::CODE_PROPRE::보마:
						case USERCONF::CODE_PROPRE::패파:
						case USERCONF::CODE_PROPRE::섀도어:
						case USERCONF::CODE_PROPRE::소마:
						case USERCONF::CODE_PROPRE::나워:
						case USERCONF::CODE_PROPRE::데벤:
							KeybdEvent('W', 1200);
							break;
						case USERCONF::CODE_PROPRE::불독:
						case USERCONF::CODE_PROPRE::썬콜:

						case USERCONF::CODE_PROPRE::바이퍼:
						case USERCONF::CODE_PROPRE::캐슈:
						case USERCONF::CODE_PROPRE::플위:
						case USERCONF::CODE_PROPRE::윈브:
							KeybdEvent('Z', 1200);
							break;
						case USERCONF::CODE_PROPRE::신궁:
							KeybdEvent('A', 1200);
							break;
						case USERCONF::CODE_PROPRE::메세:
							KeybdEvent('F', 1200);
							break;
						
						default:
							break;
						}
						KeybdEvent(VK_MENU, 1000);
						KeybdEvent({VK_UP, VK_RIGHT, VK_RETURN });
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
#endif
		default: throw BRIDGE_EXCEPTION_CODE::A_ERRORINPUT;
		}
	}
};