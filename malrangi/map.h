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
	
	void MoveFromAToB(const int A, const int B, bool IsRetry = false)
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
			case ElnasOffice:
				Cross(
					[]()
					{
						KeybdEventContinued(VK_RIGHT, 10);
						KeybdEvent({ 'C', 'C', 'X', 'X','X','X','X','X','X' }, 1300);
						ClientApi::Jump(ClientApi::JUMP_T::DEMON);
						ClientApi::DownJump(1000);
						KeybdEvent(VK_UP);
					});
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

		case ElnasOffice:
			switch (B)
			{
			case Zacum1:
				Cross([]()
					{
						MouseEvent({ 482, 592 }, LEFT_CLICK);
						KeybdEvent(VK_RETURN);
						KeybdEvent(VK_RETURN);
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case Zacum1:
			switch (B)
			{
			case Zacum2_2:
			case Zacum2_3:
				Cross([Level = Zacum2_2 == B ? 1 : 2]()
				{
					ClientApi::MinimapRecognizer Recognizer({ 10, 80, 190, 146 });
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
		case Zacum2_3:
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
						KeybdEvent(VK_RETURN);
					});
				return;

			case MeisterVill:
				Cross(
					[]() {
						static const Mat TargetImage = Read(TARGET_DIR "button//meistervill.jpg");

						KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
						if (VALLOC MatchInfo; 
							MatchTemplate(SourceImageClient4, TargetImage, &MatchInfo))
						{
							MouseEvent(MatchInfo.Location, LEFT_CLICK);
							KeybdEvent(VK_RETURN);
						}
					});
				KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case Zacum3_2:
			switch (B)
			{
			case Zacum2_2:
			case Zacum2_3:
				Cross(
					[](){
						ClientApi::BreakParty();
					});
				return;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case MeisterVill:
			switch (B)
			{
			case FreeMarket:
				Cross([]()
					{
						ClientApi::DownJump(1500);
						ClientApi::DownJump(1500);
						KeybdEventContinued(VK_LEFT, 1000);
						Sleep(0x100);
						KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2600);
					});
				return;

			case Zacum2_3:
				if (AmIIn(A))
				{
					static bool IsForward = true;
					ClientApi::MoveServer(IsForward = (IsForward == true) ? false : true);

					if (DoUntilMatchingTemplate(
						ClientApi::RECT_CLIENT4, 
						GetMapMat(Zacum2_3), 
						[]()
						{
							KeybdEvent(VK_UP, 0x200);
						},
						seconds(60)))
					{
						Sleep(0x400);
						return;
					}
					throw BRIDGE_EXCEPTION_CODE::B_FAIL;
				}
				throw BRIDGE_EXCEPTION_CODE::A_FAIL;

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}
				
		case FreeMarket:
			switch (B)
			{
			case MeisterVill:
			case ElnasMarket:
			case Unknown:
				if (AmIIn(FreeMarket))
				{
					KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
					if (WaitUntilMatchingTemplate(
						ClientApi::RECT_CLIENT4,
						array<Mat, 2>{ GetMapMat(MeisterVill), GetMapMat(ElnasMarket) },
						seconds(60)))
					{
						Sleep(0x400);
						return;
					}
					throw BRIDGE_EXCEPTION_CODE::B_FAIL;
				}
				throw BRIDGE_EXCEPTION_CODE::A_FAIL;

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

						// 고목나무 열쇠 받기
						MouseEvent({ 33, 318 }, LEFT_CLICK, 1500);
						if (VALLOC MatchInfo; MatchTemplate(SourceImageClient4, TargetImageQuestRootAbyss, &MatchInfo))
						{
							MouseEvent(MatchInfo.Location + Point{ 10, 3 }, LEFT_CLICK);
							KeybdEvent({ VK_RETURN, VK_RIGHT, VK_RETURN });
						}
						else
						{
							KeybdEvent(VK_ESCAPE);
						}

						ClientApi::MinimapRecognizer Recognizer({ 10, 126, 200, 146 });
						Recognizer.MoveToRelativeCriteria(62);
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

			default: throw BRIDGE_EXCEPTION_CODE::B_ERRORINPUT;
			}

		case RootAbyss2_1:
			switch (B)
			{
			case RootAbyss3_1:
				Cross([]()
					{
						KeybdEvent('W', 1100);
						KeybdEvent({ 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C', 'C' }, 1000);

						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 3500);
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