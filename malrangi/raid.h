#pragma once
#include "boss.h"

class UrusRaid
{
private:
	VALLOC MatchInfo;

public:
	UrusRaid() : SeqPlay(1) {}

protected:
	int SeqPlay;

	enum
	{
		BUTTON_READY_NOT_FOUND,
		PLAY_SEQUENCE_OVER,
		BATTLE_ENTRY_TIMEOUT,
		BATTLE_TIMEOUT,
		EXIT_TO_U3_FAILED,
		NPC_MASHUR_NOT_FOUND
	};
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageButtonReady(Read(TARGET_DIR "button//ready.jpg"));
		static const Mat TargetImageButtonEnter(Read(TARGET_DIR "button//enter.jpg"));
		static const Mat TargetImagePicUrus(Read(TARGET_DIR "pic//urus.jpg"));
		static const Mat TargetImageNpcMashur(Read(TARGET_DIR "npc//mashur.jpg"));
		static const Mat TargetImageMapU3 = Read(TARGET_DIR "map//u3.jpg");
		static const Mat TargetImageWindowUrusCompleted = Read(TARGET_DIR "window//urus_completed.jpg");

		/*** 전투 전 ***/
		if (1 == SeqPlay)
		{
			ClientApi::BreakParty();
		}
		
		KeybdEventContinued(VK_LEFT, 4000);
		MouseEvent({ 1140, 630 }, LEFT_CLICK);
		MouseEvent({ 440, 710 }, LEFT_CLICK);
		MouseEvent({ 487, 771 }, LEFT_CLICK);
		MouseEvent({ 820, 780 }, LEFT_CLICK);

		Mat SourceImage = SourceImageClient4;
		if (MatchTemplate(SourceImage, TargetImageButtonReady, &MatchInfo))
		{
			MouseEvent(MatchInfo.Location, LEFT_CLICK);
		}
		else
		{
			if (MatchTemplate(SourceImage, TargetImageWindowUrusCompleted, &MatchInfo))
			{
				throw PLAY_SEQUENCE_OVER;
			}
			else
			{
				throw BUTTON_READY_NOT_FOUND;
			}
		}

#define Operate(d)		KeybdEventContinued(d, 0x40);	\
							KeybdEvent('E', 0x20);
		RECT NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.left += 100;
		NewRectClient.right -= 700;
		NewRectClient.bottom -= 500;

		if (!DoUntilMatchingTemplate(
			NewRectClient,
			TargetImagePicUrus,
			[]()
			{
				MouseEvent({ 640, 455 }, LEFT_CLICK, 0x200);
			},
			seconds(1800),
				0))
		{
			throw BATTLE_ENTRY_TIMEOUT;
		}


		/*** 전투 중 ***/
		const auto StartTime = system_clock::now();
		BYTE CurrentDir, LastDir = NULL;
		int CountNullOfCurrentDir, CountInitialOperation = 0;
		NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.left += 900;
		NewRectClient.top += 530;

		MouseEvent({ 897, 159 }, LEFT_CLICK, 0);
		if (DoUntilMatchingTemplate(
			NewRectClient,
			TargetImageButtonEnter,
			[&](void)
			{
				if (duration_cast<seconds>(system_clock::now() - StartTime) > seconds(240))
				{
					Sleep(0x100);
					return;
				}

				int Score = 0;
				Mat MaskImage;
				RECT NewRect = ClientApi::RECT_CLIENT4;
				NewRect.top += 156;
				NewRect.bottom -= 100;

				InRange(Capture(NewRect, 1), Scalar(242, 30, 195), Scalar(255, 60, 205), MaskImage);

				for (int y = 0; y < MaskImage.rows; ++y)
				{
					for (int x = 0; x < MaskImage.cols; ++x)
					{
						if (MaskImage.at<BYTE>(y, x) == 255)
						{
							(x < 692) ? --Score : ++Score;
						}
					}
				}

				if (Score >= -2 && Score <= 2)
				{
					CurrentDir = NULL;
				}
				else
				{
					LastDir = CurrentDir = (Score < 0) ? VK_LEFT : VK_RIGHT;
				}

				if (CountNullOfCurrentDir > 120)
				{
					LastDir = (LastDir == VK_LEFT) ? VK_RIGHT : VK_LEFT;
					CountNullOfCurrentDir = 0;
				}

				if ((NULL == LastDir && NULL == CurrentDir) || CountInitialOperation < 24)
				{
					Operate(VK_LEFT);
					Operate(VK_RIGHT);
					++CountInitialOperation;
				}
				else if (NULL != LastDir && NULL == CurrentDir)
				{
					Operate(LastDir);
					++CountNullOfCurrentDir;
				}
				else // NULL != CurrentDir
				{
					Operate(CurrentDir);
					CountNullOfCurrentDir = 0;
				}
			},
			seconds(1000),
				0))
		{
			Sleep(2000);
		}
		else
		{
			throw BATTLE_TIMEOUT;
		}
#undef Operate


		/*** 전투 후 ***/
		static int SeqSuccess = 0;
		Write(SNAP_DIR "urus-raid//" + to_string(++SeqSuccess) + ".jpg", SourceImageClient4Colored);
		MouseEvent({ 1259, 694 }, LEFT_CLICK);

		if (WaitUntilMatchingTemplate(
			ClientApi::RECT_CLIENT4,
			TargetImageMapU3,
			seconds(60)))
		{
			Sleep(1000);
		}
		else
		{
			throw EXIT_TO_U3_FAILED;
		}

		if (SeqPlay <= 2)
		{
			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 4000);
			if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMashur, seconds(60)))
			{
				Sleep(1000);
			}
			else
			{
				throw NPC_MASHUR_NOT_FOUND;
			}
		}
	}
};

class ZacumRaid : protected BossRaid
{
private:
	VALLOC MatchInfo;

protected:
	enum
	{
		THROW_EYEOFFIRE_FAILED
	};
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageEyeOfFire = Read(TARGET_DIR "item//eye_of_fire1.jpg");
		static const Mat TargetImageInventoryBar = Read(TARGET_DIR "window//inventory_bar.jpg");
		static const Mat TargetImageButtonExpandingInventory = Read(TARGET_DIR "button//expanding_inventory.jpg");

		static map<string, vector<SKILL>> MapVecSkills;
		MapVecSkills["닼나"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["불독"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["썬콜"] =
		{
			{'Q', SKILL::UNITARY },
			{'W', SKILL::ASSIST, seconds(5)}
		};
		MapVecSkills["비숍"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["보마"] =
		{
			{'Q', SKILL::RAPID },
			{'O', SKILL::ONOFF }
		};
		MapVecSkills["신궁"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["패파"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["나로"] =
		{
			{'Q', SKILL::UNITARY},
			{'2', SKILL::BUF }
		};
		MapVecSkills["섀도어"] =
		{
			{'Q', SKILL::UNITARY },
			{'1', SKILL::BUF }
		};
		MapVecSkills["듀블"] =
		{
			{'Q', SKILL::UNITARY },
			{'2', SKILL::BUF }
		};
		MapVecSkills["바이퍼"] =
		{
			{'Q', SKILL::UNITARY },
			{'3', SKILL::BUF }
		};
		MapVecSkills["캐슈"] =
		{
			{'Q', SKILL::UNITARY },
			{'1', SKILL::BUF }
		};
		MapVecSkills["미하일"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["소마"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["플위"] =
		{
			{'Q', SKILL::UNITARY },
			{'D', SKILL::BUF },
			{'W', SKILL::ASSIST, seconds(10)}
		};
		MapVecSkills["윈브"] =
		{
			{'Q', SKILL::RAPID },
			{'2', SKILL::BUF }
		};
		MapVecSkills["나워"] =
		{
			{'Q', SKILL::UNITARY },
			{'1', SKILL::BUF },
			{'W', SKILL::ASSIST, seconds(15)}
		};
		MapVecSkills["스커"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["아란"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["에반"] =
		{
			{'Q', SKILL::UNITARY },
			{'A', SKILL::ASSIST, seconds(15)}
		};
		MapVecSkills["메세"] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills["팬텀"] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills["은월"] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills["데슬"] =
		{
			{'W', SKILL::UNITARY },
			{'2', SKILL::BUF }
		};
		MapVecSkills["데벤"] =
		{
			{'W', SKILL::UNITARY},
			{'E', SKILL::ASSIST, seconds(6)}
		};
		MapVecSkills["블래"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["배메"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["와헌"] =
		{
			{'Q', SKILL::RAPID },
			{'A', SKILL::ASSIST, seconds(30)},
			{'O', SKILL::ONOFF}
		};
		MapVecSkills["제논"] =
		{
			{'Q', SKILL::UNITARY },
			{'3', SKILL::BUF}
		};
		MapVecSkills["메카닉"] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills["엔버"] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills["제로"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["키네"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["일리움"] =
		{
			{'Q', SKILL::UNITARY },
			{'R', SKILL::ASSIST},
			{'2', SKILL::ONOFF}
		};
		MapVecSkills["카데나"] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills["아크"] =
		{
			{'A', SKILL::UNITARY }
		};
		MapVecSkills["호영"] =
		{
			{'Q', SKILL::UNITARY }
		};
		SetSkills(&MapVecSkills[CharacterInfo.ClassName]);

		try
		{
			RaidCallBoss(
				[this]()
				{
					KeybdEventContinued(VK_RIGHT, 1200);
					KeybdEvent(KeysetInfo.Inventory);

					if (MatchTemplate(SourceImageClient4, TargetImageEyeOfFire, &MatchInfo))
					{
						MouseEvent(MatchInfo.Location + Point{ 10, 10 }, LEFT_CLICK, 600);
					}
					else
					{
						Mat SourceImage = SourceImageClient4;
						MatchTemplate(SourceImage, TargetImageInventoryBar, &MatchInfo);
						MouseEvent(MatchInfo.Location + Point{ 40 * 2, 40 }, LEFT_CLICK, 600);
						MatchTemplate(SourceImage, TargetImageButtonExpandingInventory, &MatchInfo);
						MouseEvent(MatchInfo.Location + Point{ 6, 6 }, LEFT_CLICK, 600);

						if (MatchTemplate(SourceImageClient4, TargetImageEyeOfFire, &MatchInfo))
						{
							MouseEvent(MatchInfo.Location + Point{ 10, 10 }, LEFT_CLICK, 600);
						}
						else
						{
							throw THROW_EYEOFFIRE_FAILED;
						}
					}

					MouseEvent({ 25, 76 }, LEFT_CLICK);
					KeybdEvent(KeysetInfo.Inventory, 3600);
				});

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageItemPlusCoin, seconds(120));
				},
				true);

			RaidCompleteRequest(-2000, 5000);
		}
		catch (int)
		{
			Write(SNAP_DIR "zacum-raid//" + to_string(++SeqFail) + ".jpg", SourceImageClient4Colored);
			throw;
		}
	}
};


class RootAbyssRaid : protected BossRaid
{
private:
	VALLOC MatchInfo;

public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageRootAbyssBox = Read(TARGET_DIR "pic//rootabyss_box.jpg");
		static map<string, vector<SKILL>> MapVecSkills =
		{
			{
				"데벤",
				{
					{'1', SKILL::BUF, seconds(180)},
					{'2', SKILL::BUF, seconds(180)},
					{'3', SKILL::BUF, seconds(60)},
					{'E', SKILL::ASSIST, seconds(6)},
					{'Q', SKILL::UNITARY}
				}
			}
		};
		SetSkills(&MapVecSkills[CharacterInfo.ClassName]);

		try
		{
			RaidCallBoss(
				[]()
				{
					for (int i = 0; i < 9; i++)
					{
						MouseEvent({ 982, 561 }, LEFT_CLICK, 400);
					}
				});

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageRootAbyssBox, seconds(120));
				},
				true);

			RaidCompleteRequest(
				2500,
				-5000,
				[this]()
				{
					Sleep(1000);
					KeybdEvent('C', 1500);
					KeybdEvent(KeysetInfo.Attack, 3500);
				});
		}
		catch (int)
		{
			Write(SNAP_DIR "rootabyss-raid//" + to_string(++SeqFail) + ".jpg", SourceImageClient4Colored);
			throw;
		}
	}
};