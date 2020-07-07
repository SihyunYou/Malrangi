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
		B_ERRORINPUT
	};
	
	void MoveFromAToB(const int A, const int B, const USERCONF::CHARACTER_INFO& CharacterInfo, bool IsRetry = false)
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
		

		switch (A)
		{
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

		case Elnas:
			switch (B)
			{
			case Zacum1:
				CrossToZacum();
				return;

			case Urus:
			case RootAbyss1:
				Cross([&CrossWithMirror, B]()
					{
						CrossWithMirror();
					});
				return;

			case FreeMarket:
				Cross([]()
					{
						MouseEvent({ 58, 194, }, LEFT_CLICK);
						MouseEvent({ 634, 404 }, LEFT_CLICK);
						MouseEvent({ 840, 488 }, LEFT_CLICK);
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case Zacum1:
			switch (B)
			{
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
						KeybdEvent(VK_DOWN, 600);
					}
					KeybdEvent(VK_RETURN);
				});
				return;

			case Elnas:
				Cross([]()
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
						if (Recognizer.WhereAmI() < 100)
						{
							Recognizer.MoveToRelativeCriteria(143);
						}

						MouseEvent({ 820, 589 }, LEFT_CLICK);
						KeybdEvent(VK_DOWN, 400);
						KeybdEvent(VK_DOWN, 400);
						KeybdEvent(VK_DOWN, 400);
						KeybdEvent(VK_RETURN, 400);
						KeybdEvent(VK_RETURN, 400);
						KeybdEvent(VK_RETURN, 400);
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

			case Zacum3_2:
				Cross(
					[]() {
						ClientApi::MakeParty();

						MouseEvent({ 1000, 570 }, LEFT_CLICK, 600);
						KeybdEvent({ VK_RETURN, VK_RETURN, VK_RETURN }, 600);
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

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
			case Elnas:
				Cross(
					[](){
						ClientApi::MinimapRecognizer Recognizer({ 10, 80, 260, 150 });
						Recognizer.MoveToRelativeCriteria(20);

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
					});
				return;

			case Zacum1:
				CrossToZacum();
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case RootAbyss1:
			switch (B)
			{
			case RootAbyss2_1:
				Cross([]()
					{
						static const Mat TargetImageQuestRootAbyss(Read(TARGET_DIR "text//quest_rootabyss.jpg"));

						ClientApi::MakeParty();

						KeybdEvent(VK_CONTROL);
						// 고목나무 열쇠 받기
						MouseEvent({ 33, 318 }, LEFT_CLICK, 1500);
						if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, TargetImageQuestRootAbyss, &MatchInfo))
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

			case Elnas:
				Cross([]()
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 126, 200, 146 });
						Recognizer.MoveToRelativeCriteria(43);
						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
					});
				return;

			case Zacum1:
				CrossToZacum();
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case RootAbyss2_1:
			switch (B)
			{
			case RootAbyss3_1:
				Cross([&CharacterInfo]()
					{
						if("데벤" == CharacterInfo.ClassName)
						{
							KeybdEvent('W', 1000);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
						}
						else if ("썬콜" == CharacterInfo.ClassName || "불독" == CharacterInfo.ClassName || "비숍" == CharacterInfo.ClassName)
						{
							KeybdEvent('A', 1000);
							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('Q');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("신궁" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 100);
							KeybdEvent('W');
							KeybdEventContinued(VK_LEFT, 800);
							KeybdEventContinued(VK_RIGHT, 100);

							for (int q = 0; q < 12; q++)
							{
								KeybdEvent('W');
								KeybdEventContinued(VK_RIGHT, 1500);
							}
						}
						else if ("팬텀" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 200);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
						}
						else if ("소마" == CharacterInfo.ClassName)
						{
							KeybdEvent('3');
							KeybdEvent('W', 1100);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C'}, 1000);
						}
						else if ("닼나" == CharacterInfo.ClassName)
						{
							KeybdEvent('A', 1200);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
						}
						else if ("스커" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 400);
							KeybdEvent({ 'X', 'C' }, 400);

							KeybdEventContinued(VK_RIGHT, 400);
							for (int q = 0; q < 11; q++)
							{
								KeybdEvent({ 'X', 'C' }, 400);
							}
						}
						else if ("아크" == CharacterInfo.ClassName)
						{
							KeybdEvent('F', 1000);
							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('A');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("제로" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 800);
							KeybdEventContinued(VK_RIGHT, 100);
							KeybdEvent({ 'W', 'W', 'E', 'E', 'W', 'W', 'E', 'E', 'W', 'W', 'E'}, 1500);
						}
						else if ("메카닉" == CharacterInfo.ClassName)
						{
							KeybdEvent({ 'E', 'E' }, 1200);
							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('A');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("은월" == CharacterInfo.ClassName)
						{
							for (int q = 0; q < 18; q++)
							{
								KeybdEvent('W');
								KeybdEventContinued(VK_RIGHT, 900);
							}
						}
						else if ("배메" == CharacterInfo.ClassName)
						{
							KeybdEvent('O', 1000);
							KeybdEvent('E');
							KeybdEventDown(VK_RIGHT);
							for (int q = 0; q < 15; q++)
							{
								KeybdEvent('C', 400);
								KeybdEvent('Q');
							}
							KeybdEventUp(VK_RIGHT);
						}
						else if ("키네" == CharacterInfo.ClassName)
						{
							KeybdEvent('S', 1000);
							for (int q = 0; q < 14; q++)
							{
								KeybdEvent({ 'Q', 'Q' }, 1000);
								KeybdEventContinued(VK_RIGHT, 1200);
							}
						}
						else if ("블래" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 100);
							KeybdEvent('Q');
							KeybdEventContinued(VK_LEFT, 800);
							KeybdEventContinued(VK_RIGHT, 100);

							for (int q = 0; q < 20; q++)
							{
								KeybdEvent('Q');
								KeybdEventContinued(VK_RIGHT, 800);
							}
						}
						else if ("엔버" == CharacterInfo.ClassName)
						{
							KeybdEvent({ 'A', 'A' }, 1000);
							for (int q = 0; q < 13; q++)
							{
								KeybdEvent('W');
								KeybdEventContinued(VK_RIGHT, 1200);
							}
						}
						else if ("데슬" == CharacterInfo.ClassName)
						{
							KeybdEvent('E', 1000);
							for (int q = 0; q < 14; q++)
							{
								KeybdEvent('W');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("아란" == CharacterInfo.ClassName)
						{
							KeybdEvent('A', 1000);
							for (int q = 0; q < 13; q++)
							{
								KeybdEvent('W', 1000);
								KeybdEventContinued(VK_RIGHT, 1200);
							}
						}
						else if ("바이퍼" == CharacterInfo.ClassName)
						{
							KeybdEvent('E', 1000);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C'}, 1000);
						}
						else if ("플위" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 100);
							KeybdEvent('W');
							KeybdEventContinued(VK_LEFT, 800);
							KeybdEventContinued(VK_RIGHT, 100);

							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('Q');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("나워" == CharacterInfo.ClassName)
						{
						KeybdEvent('W');
						for (int q = 0; q < 20; q++)
						{
							KeybdEvent('Q');
							KeybdEventContinued(VK_RIGHT, 800);
						}
						}
						else if ("메세" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 100);
							KeybdEvent('F');
							KeybdEventContinued(VK_LEFT, 800);

							KeybdEventDown(VK_RIGHT);
							KeybdEventDown('Q');
							Sleep(20000);
							KeybdEventUp('Q');
							KeybdEventUp(VK_RIGHT);
						}
						else if ("보마" == CharacterInfo.ClassName)
						{
							KeybdEventContinued(VK_LEFT, 100);
							KeybdEvent('W');
							KeybdEventContinued(VK_LEFT, 800);

							KeybdEventDown(VK_RIGHT);
							KeybdEventDown('Q');
							Sleep(20000);
							KeybdEventUp('Q');
							KeybdEventUp(VK_RIGHT);
						}
						else if ("제논" == CharacterInfo.ClassName)
						{
							KeybdEventContinued('W', 1000);
							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('D');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("듀블" == CharacterInfo.ClassName)
						{
							KeybdEvent('E', 1000);
							for (int q = 0; q < 13; q++)
							{
								KeybdEvent({ 'F', 'W' }, 1000);
							}
						}
						else if ("미하일" == CharacterInfo.ClassName)
						{
							KeybdEvent('F', 1000);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);
						}
						else if("나로" == CharacterInfo.ClassName)
						{
							KeybdEvent('R', 1000);
							for (int q = 0; q < 16; q++)
							{
								KeybdEvent('W');
								KeybdEventContinued(VK_RIGHT, 1000);
							}
						}
						else if ("윈브" == CharacterInfo.ClassName)
						{
							KeybdEvent('E');
							for (int q = 0; q < 13; q++)
							{
								KeybdEvent({ 'W', 'C' }, 1000);
							}
						}
						else
						{
							throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
						}

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 4000);
						KeybdEvent({ VK_RIGHT, VK_RETURN });
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case RootAbyss3_1:
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

		default: throw BRIDGE_EXCEPTION_CODE::A_ERRORINPUT;
		}
	}
};