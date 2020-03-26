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
		ElnasOffice,
		Zacum1,
		Zacum2_2,
		Zacum2_3,
		Zacum3_2,
		RootAbyss1,
		RootAbyss2_1,
		RootAbyss3_1,
		MeisterVill,
		FreeMarket,
		MAX_MAPS
	};
	map<int, string> MapFileName;
	Bridge()
	{
		MapFileName[Urus] = "urus";
		MapFileName[Elnas] = "elnas";
		MapFileName[ElnasMarket] = "elnas_market";
		MapFileName[ElnasOffice] = "elnas_office";
		MapFileName[Zacum1] = "z1";
		MapFileName[Zacum2_2] = "z2_2";
		MapFileName[Zacum2_3] = "z2_3";
		MapFileName[Zacum3_2] = "z3_2";
		MapFileName[RootAbyss1] = "r1";
		MapFileName[RootAbyss2_1] = "r2_1";
		MapFileName[RootAbyss3_1] = "r3_1";
		MapFileName[MeisterVill] = "meistervill";
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
		return Value > 0.96 ? Seq : Unknown;
	}
	bool AmIIn(int NumberOfMap)
	{
		return MatchTemplate(Capture(RECT_MAP), GetMapMat(NumberOfMap), nullptr, 0.96);
	}
	void MoveFromAToB(int A, int B)
	{
		enum
		{
			A_FAIL,
			A_ERRORINPUT,
			B_FAIL,
			B_ERRORINPUT,
			SUCCESS
		};

		bool IsRetry = false;
		bool IsRetryAvailable;
		auto CrossFromAToB = [this, &IsRetryAvailable, &IsRetry](int A, int B) -> int
		{
			auto Cross =
				overload(
					[this, A, B, &IsRetryAvailable](auto CrossRoutine)
					{
						ClientApi::SET_CLIENT_STDPOS();
						IsRetryAvailable = false;
						if (AmIIn(A))
						{
							CrossRoutine();

							if (WaitUntilMatchingTemplate(
								RECT_MAP,
								GetMapMat(B),
								seconds(32)))
							{
								Sleep(0x400);
								return SUCCESS;
							}
							else
							{
								return B_FAIL;
							}
						}
						else
						{
							return A_FAIL;
						}
					},
					[this, A, B, &IsRetryAvailable, &IsRetry](auto CrossRoutine, auto RecrossRoutine)
					{
						ClientApi::SET_CLIENT_STDPOS();
						IsRetryAvailable = true;
						if (AmIIn(A))
						{
							!IsRetry ? CrossRoutine() : RecrossRoutine();

							if (WaitUntilMatchingTemplate(
								RECT_MAP,
								GetMapMat(B),
								seconds(32)))
							{
								Sleep(0x400);
								return SUCCESS;
							}
							else
							{
								return B_FAIL;
							}
						}
						else
						{
							return A_FAIL;
						}
					}
					);
			auto CrossOrRecross = [this, A, B, &IsRetryAvailable, &IsRetry](auto CrossAndRecrossRoutine)
			{
				ClientApi::SET_CLIENT_STDPOS();
				IsRetryAvailable = true;
				if (AmIIn(A))
				{
					CrossAndRecrossRoutine();

					if (WaitUntilMatchingTemplate(
						RECT_MAP,
						GetMapMat(B),
						seconds(32)))
					{
						Sleep(0x400);
						return SUCCESS;
					}
					else
					{
						return B_FAIL;
					}
				}
				else
				{
					return A_FAIL;
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
				else
				{
					throw AppException("DimensionalMirrorNotFound");
				}
			};

			switch (A)
			{
			case Urus:
				switch (B)
				{
				case Elnas:
					return CrossOrRecross([]()
						{
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 4000);
						});

				default: return B_ERRORINPUT;
				}

			case Elnas:
				switch (B)
				{
				case ElnasOffice:
					return CrossOrRecross([]()
						{
							KeybdEventContinued(VK_RIGHT, 10);
							KeybdEvent({ 'C', 'C', 'X', 'X','X','X','X','X','X' }, 1300);
							ClientApi::Jump(ClientApi::JUMP_T::DEMON);
							ClientApi::DownJump(1000);
							KeybdEvent(VK_UP);
						});

				case Urus:
				case RootAbyss1:
					return CrossOrRecross([&CrossWithMirror, B]()
						{
							CrossWithMirror();
						});

				case FreeMarket:
					return CrossOrRecross([]()
						{
							MouseEvent({ 58, 194, }, LEFT_CLICK);
							MouseEvent({ 634, 404 }, LEFT_CLICK);
							MouseEvent({ 840, 488 }, LEFT_CLICK);
						});

				default: return B_ERRORINPUT;
				}

			case ElnasOffice:
				switch (B)
				{
				case Zacum1:
					return CrossOrRecross([]()
						{
							MouseEvent({ 482, 592 }, LEFT_CLICK);
							KeybdEvent(VK_RETURN);
							KeybdEvent(VK_RETURN);
						});

				default: return B_ERRORINPUT;
				}

			case Zacum1:
				switch (B)
				{
				case Zacum2_2:
				case Zacum2_3:
					return CrossOrRecross([Level = Zacum2_2 == B ? 1 : 2]()
					{
						ClientApi::MinimapRecognizer Recognizer({ 10, 54, 190, 120 });
						if (Recognizer.WhereAmI() < 100)
						{
							Recognizer.MoveToRelativeCriteria(143);
						}

						MouseEvent({ 820, 589 }, LEFT_CLICK);
						KeybdEvent({ VK_DOWN, VK_DOWN, VK_RETURN, VK_DOWN, VK_RETURN, VK_RETURN, VK_RETURN, VK_RETURN }, 400);
						KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1200);
						for (int i = 0; i < (int)Level; i++)
						{
							KeybdEvent(VK_DOWN, 600);
						}
						KeybdEvent(VK_RETURN);
					});

				case Elnas:
					return CrossOrRecross([]()
						{
							ClientApi::MinimapRecognizer Recognizer({ 10, 54, 190, 120 });
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

				default: return B_ERRORINPUT;
				}

			case Zacum2_2:
			case Zacum2_3:
				switch (B)
				{
				case Zacum1:
					return CrossOrRecross([]()
						{
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
						});

				case Zacum3_2:
					return CrossOrRecross([]()
						{
							ClientApi::MakeParty();
							MouseEvent({ 1000, 570 }, LEFT_CLICK, 600);
							KeybdEvent(VK_RETURN);
						});

				case MeisterVill:
				{
					int ReturnValue = CrossOrRecross([]()
						{
							static const Mat TargetImage = Read(TARGET_DIR "button//meistervill.jpg");

							KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
							if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, LEFT_CLICK);
								KeybdEvent(VK_RETURN);
							}
							else
							{
								throw AppException("ButtonMeisterVillNotFoundException");
							}
						});
					if (SUCCESS == ReturnValue)
					{
						KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
					}

					return ReturnValue;
				}

				default: return B_ERRORINPUT;
				}

			case Zacum3_2:
				switch (B)
				{
				case Zacum2_2:
				case Zacum2_3:
					return Cross(
						[]()
						{
							ClientApi::BreakParty();
						},
						[]()
						{
							;
						});

				default: return B_ERRORINPUT;
				}

			case MeisterVill:
				switch (B)
				{
				case FreeMarket:
					return CrossOrRecross([]()
						{
							ClientApi::DownJump(1500);
							ClientApi::DownJump(1500);
							KeybdEventContinued(VK_LEFT, 1000);
							Sleep(0x100);
							KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2600);
						});

				case Zacum2_3:
					return Cross(
						[this]()
						{
							static bool IsForward = true;

							ClientApi::MoveServer(IsForward = (IsForward == true) ? false : true);
							if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, GetMapMat(MeisterVill), seconds(60)))
							{
								KeybdEventContinued(VK_UP, 1000);
							}
						},
						[]()
						{
							KeybdEventContinued(VK_UP, 1000);
						});

				default: return B_ERRORINPUT;
				}

			case FreeMarket:
				switch (B)
				{
				case MeisterVill:
				case ElnasMarket:
					return CrossOrRecross([]()
						{
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
						});

				default: return B_ERRORINPUT;
				}

			case ElnasMarket:
				switch (B)
				{
				case Elnas: // 재조정필요
					return CrossOrRecross([]()
						{
							KeybdEventContinued(VK_LEFT, 3800);
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1400);
						});

				default: return B_ERRORINPUT;
				}

			case RootAbyss1:
				switch (B)
				{
				case RootAbyss2_1:
					return CrossOrRecross([]()
						{
							static const Mat TargetImageQuestRootAbyss(Read(TARGET_DIR "text//quest_rootabyss.jpg"));

							ClientApi::MakeParty();

							// 고목나무 열쇠 받기
							MouseEvent({ 33, 318 }, LEFT_CLICK, 1500);
							if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, TargetImageQuestRootAbyss, &MatchInfo, 0.96))
							{
								MouseEvent(MatchInfo.Location + Point{ 10, 3 }, LEFT_CLICK);
								KeybdEvent({ VK_RETURN, VK_RIGHT, VK_RETURN });
							}
							else
							{
								KeybdEvent(VK_RETURN);
							}

							ClientApi::MinimapRecognizer Recognizer({ 10, 100, 200, 120 });
							Recognizer.MoveToRelativeCriteria(62);
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
							KeybdEvent(VK_RETURN);
						});
						
				case Elnas:
					return CrossOrRecross([]()
						{
							ClientApi::MinimapRecognizer Recognizer({ 10, 100, 200, 120 });
							Recognizer.MoveToRelativeCriteria(43);
							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1000);
						});

				default: return B_ERRORINPUT;
				}

			case RootAbyss2_1:
				switch (B)
				{
				case RootAbyss3_1:
					return Cross([]()
						{
							KeybdEvent('W', 1100);
							KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);

							KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 3500);
							KeybdEvent({ VK_RIGHT, VK_RETURN });
						});

				default: return B_ERRORINPUT;
				}

			case RootAbyss3_1:
				switch (B)
				{
				case RootAbyss1:
					return Cross(
						[]()
						{
							ClientApi::BreakParty();
						},
						[]()
						{
							;
						});

				default: return B_ERRORINPUT;
				}

			default: return A_ERRORINPUT;
			}
		};

		string Message;

	__RECROSS:
		switch (CrossFromAToB(A, B))
		{
		case SUCCESS: return;

		case A_FAIL:
			Message = "NotIn" + MapFileName[A] + "(A_FAIL)"; break;
		case B_FAIL:
			Message = "MoveTo" + MapFileName[A] + "(B_FAIL)";
			break;
			/*if (IsRetryAvailable && IsRetryFirst)
			{
				IsRetryFirst = false;

				try
				{
					ClientApi::RemoveAllIngameWindows();
				ClientApi::ExitGame();

				}
				goto __RECROSS;
			}
			else
			{
				Message = "MoveTo" + MapFileName[A] + "(B_FAIL)";
				break;
			}*/
		case A_ERRORINPUT:
			Message = MapFileName[A] + "(A_ERRORINPUT)"; break;
		case B_ERRORINPUT:
			Message = MapFileName[B] + "(B_ERRORINPUT)"; break;
		}
		throw AppException(Message);
	}
};