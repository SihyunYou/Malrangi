#pragma once
#include "boss.h"

class UrusRaid
{
public:
	UrusRaid() : SeqPlay(1) {}

protected:
	int SeqPlay;

	enum class URUSRAID_EXCEPTION_CODE
	{
		PARTY_UNBREAKABLE,
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
		static const Mat TargetImageWindowUrusPartyNotBroken = Read(TARGET_DIR "window//urus_party_not_broken.jpg");
		VALLOC MatchInfo;

		/*** 전투 전 ***/
		if (1 == SeqPlay)
		{
			ClientApi::BreakParty();
		}
		else if (3 < SeqPlay)
		{
			return;
		}

		KeybdEventContinued(VK_LEFT, 4000);

		bool IsRetry = false;
	__RETRY__:
		MouseEvent({ 1140, 630 }, LEFT_CLICK);
		MouseEvent({ 440, 710 }, LEFT_CLICK);
		MouseEvent({ 487, 771 }, LEFT_CLICK);
		MouseEvent({ 820, 780 }, LEFT_CLICK);
		MouseEvent({ 820, 780 }, LEFT_CLICK);

		Mat SourceImage = SourceImageClient4;
		if (MatchTemplate(SourceImage, TargetImageButtonReady, &MatchInfo))
		{
			MouseEvent(MatchInfo.Location, LEFT_CLICK);
		}
		else
		{
			if (MatchTemplate(SourceImage, TargetImageWindowUrusPartyNotBroken, &MatchInfo))
			{
				if (IsRetry)
				{
					throw URUSRAID_EXCEPTION_CODE::PARTY_UNBREAKABLE;
				}
				else
				{
					KeybdEvent(VK_ESCAPE);
					ClientApi::BreakParty();

					IsRetry = true;
					goto __RETRY__;
				}
			}

			if (MatchTemplate(SourceImage, TargetImageWindowUrusCompleted, &MatchInfo))
			{
				throw URUSRAID_EXCEPTION_CODE::PLAY_SEQUENCE_OVER;
			}
			else
			{
				throw URUSRAID_EXCEPTION_CODE::BUTTON_READY_NOT_FOUND;
			}
		}

#define Operate(d)		KeybdEventContinued(d, 0x40);	\
							KeybdEvent('E', 0x20);
		RECT NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.left += 100;
		NewRectClient.right -= 700;
		NewRectClient.bottom -= 500;
		int p = 0;

		if (!DoUntilMatchingTemplate(
			NewRectClient,
			TargetImagePicUrus,
			[&p]()
			{
				if ((++p % 0x20) == 0)
				{
					KeybdEvent(VK_RETURN, 0);
				}
			},
			seconds(600),
			1))
		{
			throw URUSRAID_EXCEPTION_CODE::BATTLE_ENTRY_TIMEOUT;
		}


		/*** 전투 중 ***/
		const auto StartTime = system_clock::now();
		BYTE CurrentDir, LastDir = NULL;
		int CountNullOfCurrentDir, CountInitialOperation = 0;
		int CountMaximumNullOfCurrentDir = 24;
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

				if (CountNullOfCurrentDir > CountMaximumNullOfCurrentDir)
				{
					LastDir = (LastDir == VK_LEFT) ? VK_RIGHT : VK_LEFT;
					CountNullOfCurrentDir = 0;
					CountMaximumNullOfCurrentDir += 12;
				}

				if ((NULL == LastDir && NULL == CurrentDir) || CountInitialOperation < 24)
				{
					Operate(VK_LEFT);
					Operate(VK_RIGHT);
					++CountNullOfCurrentDir;
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
			throw URUSRAID_EXCEPTION_CODE::BATTLE_TIMEOUT;
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
			throw URUSRAID_EXCEPTION_CODE::EXIT_TO_U3_FAILED;
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
				throw URUSRAID_EXCEPTION_CODE::NPC_MASHUR_NOT_FOUND;
			}
		}
	}
};

class ZacumRaid : protected BossRaid
{
protected:
	enum class ZACUMRAID_EXCEPTION_CODE
	{
		THROW_EYEOFFIRE_FAILED
	};
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageEyeOfFire = Read(TARGET_DIR "item//eye_of_fire1.jpg");

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
			{'Q', SKILL::RAPID },
			{'1', SKILL::BUF}
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
					Sleep(500);
					KeybdEventContinued(VK_RIGHT, 1200);
					
					if (-1 == ClientApi::ThrowItem(TargetImageEyeOfFire))
					{
						throw ZACUMRAID_EXCEPTION_CODE::THROW_EYEOFFIRE_FAILED;
					}
					Sleep(3000);
				});

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBossTimer, seconds(120));
				},
				200,
				true);

			RaidCompleteRequest(-3000, 5000);
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
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageNpcPetrick = Read(TARGET_DIR "npc//petrick.jpg");
		static const Mat ArrTargetImageItem1[] =
		{
			Read(TARGET_DIR "item//mastery_book20.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//red_potion.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//blue_potion.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//reinforce.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//mineral_pocket.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//hub_pocket.jpg", IMREAD_COLOR)
		};
		static const Mat ArrTargetImageItem2[] =
		{
			Read(TARGET_DIR "item//crystal2.jpg", IMREAD_COLOR),
			Read(TARGET_DIR "item//crystal3.jpg", IMREAD_COLOR)
		};
		static map<string, unsigned int> MapCharacterSpeed =
		{
			{"닼나", 150},
			{"불독", 140},
			{"썬콜", 140},
			{"비숍", 128},
			{"보마", 160},
			{"신궁", 160},
			{"나로", 138},
			{"듀블", 139},
			{"바이퍼", 160},
			{"소마", 160},
			{"플위", 140},
			{"윈브", 160},
			{"나워", 137},
			{"스커", 160},
			{"미하일", 140},
			{"아란", 160},
			{"메세", 165},
			{"팬텀", 160},
			{"은월", 140},
			{"데슬", 140},
			{"데벤", 160},
			{"블래", 160},
			{"배메", 170},
			{"제논", 160},
			{"메카닉", 140},
			{"엔버", 165},
			{"제로", 129},
			{"키네", 160},
			{"아크", 165},
		};
		static map<string, vector<SKILL>> MapVecSkills =
		{
			{
				"닼나",
				{
					{'E', SKILL::ASSIST, seconds(10)},
					{'W', SKILL::UNITARY}
				}
			},
			{
				"불독",
				{
					{'1', SKILL::BUF},
					{'2', SKILL::BUF},
					{'3', SKILL::BUF},
					{VK_F1, SKILL::BUF},
					{'R', SKILL::ASSIST, seconds(50)},
					{'W', SKILL::ASSIST, seconds(10)},
					{'O', SKILL::ONOFF},
					{'L', SKILL::ONOFF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"썬콜",
				{
					{VK_F1, SKILL::BUF},
					{'T', SKILL::ASSIST, seconds(180)},
					{'W', SKILL::ASSIST, seconds(6)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"비숍",
				{
					{'R', SKILL::ASSIST, seconds(180)},
					{'1', SKILL::BUF},
					{'2', SKILL::BUF},
					{VK_F2, SKILL::BUF},
					{'D', SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"보마",
				{
					{'O', SKILL::ONOFF},
					{'2', SKILL::BUF},
					{'3', SKILL::BUF},
					{VK_F2, SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"신궁",
				{
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"듀블",
				{
					{'2', SKILL::BUF},
					{'Z', SKILL::ASSIST, seconds(180)},
					{'S', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"소마",
				{
					{'R', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"윈브",
				{
					{'2', SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"나워",
				{
					{'P', SKILL::ONOFF},
					{'1', SKILL::BUF},
					{'2', SKILL::BUF},
					{VK_F1, SKILL::BUF},
					{VK_F2, SKILL::ASSIST, seconds(180)},
					{'W', SKILL::ASSIST, seconds(9)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"스커",
				{
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"팬텀",
				{
					{'A', SKILL::UNITARY}
				}
			},
			{
				"제로",
				{
					{VK_DELETE, SKILL::ASSIST, seconds(180)},
				//	{'T', SKILL::ASSIST, seconds(180)},
					{'Z', SKILL::UNITARY}
				}
			},
			{
				"메카닉",
				{
					{'E', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"은월",
				{
					{'T', SKILL::ASSIST, seconds(180)},
					{'W', SKILL::UNITARY}
				}
			},
			{
				"배메",
				{
					{'1', SKILL::BUF},
					{VK_F2, SKILL::BUF},
					{VK_F3, SKILL::BUF},
					{'F', SKILL::BUF},
					{'D', SKILL::BUF},
					{'R', SKILL::BUF},
					{'L', SKILL::ONOFF},
					{'Q', SKILL::UNITARY}
				}
			},
				{
				"키네",
				{
					{'W', SKILL::ASSIST, seconds(180)},
					{'T', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
				{
				"블래",
				{
					{'R', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"엔버",
				{
					{'4', SKILL::BUF},
					{'D', SKILL::UNITARY}
				}
			},
			{
				"데슬",
				{
					{'2', SKILL::BUF},
					{'3', SKILL::BUF},
					{'T', SKILL::ASSIST, seconds(180)},
					{'W', SKILL::UNITARY}
				}
			},
			{
				"아란",
				{
					{VK_F2, SKILL::BUF},
					{'R', SKILL::ASSIST, seconds(20)},
					{'Z', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"바이퍼",
				{
				{'L', SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"플위",
				{
					{'W', SKILL::ASSIST, seconds(6)},
					{'Q', SKILL::UNITARY}
				}
			},
				{
				"메세",
				{
					{'2', SKILL::BUF},
					{VK_F1, SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
				},
				{
				"제논",
				{
					{'2', SKILL::BUF},
					{VK_F2, SKILL::BUF},
					{'4', SKILL::BUF},
					{VK_F1, SKILL::BUF},
					{'T', SKILL::ASSIST, seconds(180)},
					{'Q', SKILL::UNITARY}
				}
				},
				{
				"미하일",
				{
					{'L', SKILL::ONOFF},
					{'Q', SKILL::UNITARY}
				}
			},	
			{
				"나로",
				{
					{'P', SKILL::ONOFF},		
					{'2', SKILL::BUF},
					{'R', SKILL::BUF},
					{VK_F3, SKILL::BUF},
					{'Q', SKILL::UNITARY}
				}
			},
			{
				"아크",
				{
				//	{'T', SKILL::ASSIST, seconds(180)},
					{'R', SKILL::ASSIST, seconds(180)},
					{'A', SKILL::UNITARY}
				}
			},
			{
				"데벤",
				{
					{'1', SKILL::BUF},
					{'2', SKILL::BUF},
					{'3', SKILL::BUF},
					{'4', SKILL::BUF},
					{'T', SKILL::ASSIST, seconds(180)},
					{'E', SKILL::ASSIST, seconds(6)},
					{'Q', SKILL::UNITARY}
				}
			},
		};
		SetSkills(&MapVecSkills[CharacterInfo.ClassName]);

#define GET_DURATION(_d, _s) (_d / _s)
		try
		{
			RaidCallBoss(
				[this, &CharacterInfo]()
				{
					KeybdEventContinued(VK_RIGHT, GET_DURATION(214000, MapCharacterSpeed[CharacterInfo.ClassName]));
					Sleep(800);

					for (int i = 0; i < 9; i++)
					{
						MouseEvent({ 870, 561 }, LEFT_CLICK, 280);
					}
				});

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBossTimer, seconds(120));
				},
				7000,
				true);

			RaidCompleteRequest(
				2500,
				-4500,
				[this, &CharacterInfo]()
				{
					Sleep(1000);

					KeybdEventContinued(VK_LEFT, 8000);
					KeybdEventContinued(VK_RIGHT, GET_DURATION(672000, MapCharacterSpeed[CharacterInfo.ClassName]));
					Sleep(1000);
					
					if (VALLOC MatchInfo;
						MatchTemplate(SourceImageClient4, TargetImageNpcPetrick, &MatchInfo), 0.8)
					{
						MouseEvent(MatchInfo.Location, LEFT_CLICK);

						for (const auto& TargetImage : ArrTargetImageItem1)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DLEFT_CLICK);
								KeybdEvent({ VK_RETURN, VK_RETURN }, 400);
							}
						}
						
						for (const auto& TargetImage : ArrTargetImageItem2)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DLEFT_CLICK);
								KeybdEvent({ '3', '0', VK_RETURN, VK_RETURN }, 400);
							}
						}
						
						KeybdEvent(VK_ESCAPE);
					}

					if ("데슬" == CharacterInfo.ClassName || "아란" == CharacterInfo.ClassName)
					{
						KeybdEvent('Q', 3000);
					}
					else if ("엔버" == CharacterInfo.ClassName)
					{
						KeybdEventContinued(KeysetInfo.Attack, 3000);
					}
					else
					{
						KeybdEvent(KeysetInfo.Attack, 3000);
					}
				});
		}
		catch (int)
		{
			Write(SNAP_DIR "rootabyss-raid//" + to_string(++SeqFail) + ".jpg", SourceImageClient4Colored);
			throw;
		}
	}
};