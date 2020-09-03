#pragma once
#include "client.h"

#pragma comment(lib,"winmm.lib")

class MobDetect
{
public:
	MobDetect()
	{

	}

public:
	DWORD GetRelativeDirection(cv::InputArray LowerScalar, cv::InputArray UpperScalar)
	{
		Mat Mask;

		if (InRange(Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR), LowerScalar, UpperScalar, Mask))
		{
			DWORD EvaluatedDirection = NULL;
			int ScoreX = 0;

			for (int y = 0; y < Mask.rows; ++y)
			{
				for (int x = 0; x < Mask.cols; ++x)
				{
					if (Mask.at<BYTE>(y, x) == 255)
					{
						if (x < 683)
						{
							--ScoreX;
						}
						else
						{
							++ScoreX;
						}
					}
				}
			}

			if (ScoreX < 0)
			{
				EvaluatedDirection = VK_LEFT;
			}
			else if (ScoreX > 0)
			{
				EvaluatedDirection = VK_RIGHT;
			}
		}

		return NULL;
	}
};

class BossRaid
{
protected:
	VALLOC MatchInfo;

public:
	struct SKILL
	{
		BYTE Key;
		enum
		{
			UNITARY = 0b00000001,
			RAPID = 0b00000010,
			BUF = 0b00000100,
			ASSIST = 0b00001000,
			ONOFF = 0b00010000,
		};
		DWORD Flag;
		seconds Cooltime;
		milliseconds Delay;
		seconds Reserved1; // Charging time
		system_clock::time_point LastRenewedTime;

		SKILL(BYTE Key, DWORD Flag,
			milliseconds Delay = milliseconds(1000), seconds Cooltime = seconds(180), seconds Reserved1 = seconds(-1)) :
			Key(Key),
			Flag(Flag),
			Delay(Delay),
			Cooltime(Cooltime),
			Reserved1(Reserved1)
		{
			switch (Flag)
			{
			case UNITARY:
				Delay = milliseconds(800); break;
			case RAPID:
				Delay = milliseconds(200); break;
			case BUF:
			case ASSIST:
			case ONOFF:
			default:
				Delay = milliseconds(1000); break;
			}
		}
	};

public:
	int SeqSuccess, SeqFail;

public:
	vector<SKILL>* VecSkills = nullptr;
	SKILL LastSkill;
	const USERCONF::KEYSET_INFO& KeysetInfo;

	inline static const Mat TargetImageItemPlusCoin = Read(TARGET_DIR "item//pluscoin.jpg");
	inline static const Mat TargetImageBossTimer = Read(TARGET_DIR "window//boss_timer.jpg");

	enum class BOSSRAID_EXCEPTION_CODE
	{
		BATTLE_TIMEOUT = 100,
		RAID_COMPLETE_FAILED
	};

public:
	BossRaid() :
		SeqSuccess(0), SeqFail(0),
		VecSkills(nullptr),
		KeysetInfo(USERCONF::GetInstance()->VirtualKeyset),
		LastSkill(NULL, NULL) {}

public:
	void SupprimerBufs(int p = 1)
	{
		for (int q = 0; q < p; q++)
		{
			MouseEvent({ 1350, 45 }, CLIC_DROIT);
		}
	}
	void SetSkills(vector<SKILL>* _VecSkills)
	{
		VecSkills = _VecSkills;
	}
	void UseSkills(vector<SKILL>& VecSkills, int Flags)
	{
		auto UseSkill = [this](SKILL& CurrentSkill)
		{
			auto IsCoolOn = [](const SKILL& Skill)
			{
				return duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime) > Skill.Cooltime;
			};
			auto RenewTime = [](SKILL& Skill)
			{
				Skill.LastRenewedTime += duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime);
			};

			if (SKILL::ASSIST == CurrentSkill.Flag || SKILL::BUF == CurrentSkill.Flag)
			{
				if (IsCoolOn(CurrentSkill))
				{
					RenewTime(CurrentSkill);
				}
				else
				{
					return;
				}
			}

			if (LastSkill.Key != NULL)
			{
				if ((SKILL::UNITARY != CurrentSkill.Flag && SKILL::RAPID != CurrentSkill.Flag))
				{
					KeybdEventUp(LastSkill.Key);
					this_thread::sleep_for(LastSkill.Delay);
				}
			}

			if (seconds(0) == CurrentSkill.Reserved1)
			{
				KeybdEvent({ CurrentSkill.Key, CurrentSkill.Key }, 1200);
			}
			else if (seconds(-1) != CurrentSkill.Reserved1)
			{
				KeybdEventContinued(CurrentSkill.Key, CurrentSkill.Reserved1);
			}
			else
			{
				if (SKILL::UNITARY == CurrentSkill.Flag || SKILL::RAPID == CurrentSkill.Flag)
				{
					KeybdEventDown(CurrentSkill.Key);
				}
				else
				{
					KeybdEvent(CurrentSkill.Key, 0);
				}
			}

			LastSkill = CurrentSkill;
		};

		for (auto& Skill : VecSkills)
		{
			if (IS_FLAG_ON(Flags, Skill.Flag))
			{
				UseSkill(Skill);
			}
			Sleep(0x10);
		}
	}

	template <class T_LAMBDA>
	void RaidCallBoss(T_LAMBDA CallBossRoutine, bool IsForward)
	{
		if (IsForward)
		{
			UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
			CallBossRoutine();
		}
		else
		{
			CallBossRoutine();
			UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
		}
	}
	template <class T_LAMBDA>
	void RaidDoBattle(T_LAMBDA PendingUntilMatchingTemplateRoutine, int ResurectionTimeCycle, bool IsFixed)
	{
		bool IsThreadJoinable = false;
		auto ThreadResurrection = thread(
			[&IsThreadJoinable, ResurectionTimeCycle]()
			{
				while (!IsThreadJoinable)
				{
					MouseEvent({ 800, 320 }, CLIC_GAUCHE, ResurectionTimeCycle);
				}
			}
		);
		auto ThreadBattle = thread(
			[&IsThreadJoinable, this]()
			{
				do
				{
					UseSkills(*VecSkills, SKILL::UNITARY | SKILL::RAPID | SKILL::ASSIST);
				} while (!IsThreadJoinable);
			}
		);

		bool IsRaidCompleteSuccessfully = PendingUntilMatchingTemplateRoutine();
		IsThreadJoinable = true;

		if (ThreadBattle.joinable())
		{
			ThreadBattle.join();
		}
		if (ThreadResurrection.joinable())
		{
			ThreadResurrection.join();
		}

		for each (const auto & Skill in *VecSkills)
		{
			KeybdEventUp(Skill.Key);
		}

		if (!IsRaidCompleteSuccessfully)
		{
			throw BOSSRAID_EXCEPTION_CODE::BATTLE_TIMEOUT;
		}
	}

protected:
#define ArrowKeyFromInteger(_x) (_x < 0 ? VK_LEFT : VK_RIGHT)
	void RaidCompleteRequest(int PickingMilliseconds1, int PickingMilliseconds2)
	{
		KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds1), KeysetInfo.Picking, std::labs(PickingMilliseconds1));
		KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds2), KeysetInfo.Picking, std::labs(PickingMilliseconds2));
	}
	template <class T_LAMBDA>
	void RaidCompleteRequest(int PickingMilliseconds1, int PickingMilliseconds2, T_LAMBDA BeforePickingRoutine)
	{
		BeforePickingRoutine();

		if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageItemPlusCoin, seconds(10)))
		{
			KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds1), KeysetInfo.Picking, std::labs(PickingMilliseconds1));
			KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds2), KeysetInfo.Picking, std::labs(PickingMilliseconds2));
		}
		else
		{
			throw BOSSRAID_EXCEPTION_CODE::RAID_COMPLETE_FAILED;
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

		static map<USERCONF::CODE_PROPRE, vector<SKILL>> MapVecSkills;
		MapVecSkills[USERCONF::CODE_PROPRE::닼나] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::불독] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::썬콜] =
		{
			{'Q', SKILL::UNITARY },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::비숍] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::보마] =
		{
			{'Q', SKILL::RAPID },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::신궁] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::패파] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나로] =
		{
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::섀도어] =
		{
			{'1', SKILL::BUF },
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::듀블] =
		{
			{'Q', SKILL::UNITARY },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::바이퍼] =
		{
			{'Q', SKILL::UNITARY },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캐슈] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::미하일] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::소마] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::플위] =
		{
			{'Q', SKILL::UNITARY },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::윈브] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나워] =
		{
			{'Q', SKILL::UNITARY },
		};
		MapVecSkills[USERCONF::CODE_PROPRE::스커] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아란] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::에반] =
		{
			{'A', SKILL::ASSIST},
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메세] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::팬텀] =
		{
			{'1', SKILL::BUF},
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::은월] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데슬] =
		{
			{'2', SKILL::BUF },
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데벤] =
		{
			{'E', SKILL::ASSIST, seconds(6)},
			{'W', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::블래] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::배메] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::와헌] =
		{
			{'O', SKILL::ONOFF},
			{'A', SKILL::ASSIST, seconds(30)},
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제논] =
		{
			{'3', SKILL::BUF},
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메카닉] =
		{
			{'Q', SKILL::RAPID }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::엔버] =
		{
			{'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제로] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::키네] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::일리움] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::카데나] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아크] =
		{
			{'A', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::호영] =
		{
			{'Q', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아델] =
		{
			{'Q', SKILL::UNITARY }
		};
		SetSkills(&MapVecSkills[CharacterInfo.Code]);

		try
		{
			KeybdEventContinued(VK_RIGHT, 1200);
			UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
			Sleep(800);

			if (-1 == ClientApi::ThrowItem(TargetImageEyeOfFire))
			{
				throw ZACUMRAID_EXCEPTION_CODE::THROW_EYEOFFIRE_FAILED;
			}
			Sleep(3000);

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

class RootAbyssBloodyQueenRaid : protected BossRaid
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
		
		static map<USERCONF::CODE_PROPRE, vector<SKILL>> MapVecSkills;
		MapVecSkills[USERCONF::CODE_PROPRE::닼나] =
		{
			{'1', SKILL::BUF},
			{ 'E', SKILL::ASSIST, milliseconds(800), seconds(10)},
			{ 'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::불독] =
		{
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(50)},
			{'W', SKILL::ASSIST, milliseconds(800), seconds(10)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::썬콜] =
		{
			{'E', SKILL::ASSIST, milliseconds(1000), seconds(75), seconds(8)},
			{'W', SKILL::ASSIST, milliseconds(800), seconds(5)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::비숍] =
		{
			{VK_F2, SKILL::BUF},
			{'D', SKILL::BUF},
			{'R', SKILL::ASSIST},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::보마] =
		{
			{'O', SKILL::ONOFF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::신궁] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::패파] =
		{

		};
		MapVecSkills[USERCONF::CODE_PROPRE::나로] =
		{
			{'2', SKILL::BUF},
			{'R', SKILL::BUF},
			{'Q', SKILL::UNITARY}																	
		};
		MapVecSkills[USERCONF::CODE_PROPRE::섀도어] =
		{
			{'Q', SKILL::UNITARY },
			{'1', SKILL::BUF }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::듀블] =
		{
			{'2', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{'R', SKILL::ASSIST, milliseconds(800), seconds(60), seconds(10)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::바이퍼] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'L', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캡틴] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캐슈] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::소마] =
		{
			{'4', BossRaid::SKILL::BUF},
			{VK_F2, BossRaid::SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::플위] =
		{
			{'1', SKILL::BUF, milliseconds(1000), seconds(180), seconds(3)},
			{'D', SKILL::BUF},
			{'Q', SKILL::ASSIST, milliseconds(800), seconds(5)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::윈브] =
		{
			{'2', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나워] =
		{
			{'P', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'W', SKILL::ASSIST, milliseconds(800), seconds(8)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::스커] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아란] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(20)},
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::에반] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메세] =
		{
			{'2', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::팬텀] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'W', SKILL::ASSIST, milliseconds(1000), seconds(15)},
			{'A', SKILL::UNITARY}
		},
		MapVecSkills[USERCONF::CODE_PROPRE::루미] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::은월] =
		{
			{'W', BossRaid::SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데벤] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF},
			{'E', SKILL::ASSIST, milliseconds(800), seconds(7)},
			{'Q', SKILL::UNITARY, milliseconds(600)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데슬] =
		{
			{'2', SKILL::BUF},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::블래] =
		{
			{'R', SKILL::ASSIST, seconds(180)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::배메] =
		{
			{'L', SKILL::ONOFF},
			{'2', SKILL::ONOFF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::와헌] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제논] =
		{
			{'2', SKILL::BUF},
			{'4', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메카닉] =
		{
			{'E', SKILL::ASSIST, seconds(180)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::카이저] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::엔버] =
		{
			{'4', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{'E', SKILL::BUF},
			{'F', SKILL::ASSIST, milliseconds(800), seconds(60), seconds(8)},
			{'D', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제로] =
		{
			{VK_DELETE, SKILL::ASSIST},
			{'T', SKILL::ASSIST},
			{'Z', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::키네] =
		{
			{'W', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::호영] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::일리움] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아크] =
		{
			{'W', SKILL::ASSIST},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아델] =
		{
			{'E', SKILL::ASSIST, milliseconds(1000), seconds(60)},
			{'S', SKILL::ASSIST, milliseconds(1000), seconds(30)},
			{'Q', SKILL::UNITARY}
		};
		SetSkills(&MapVecSkills[CharacterInfo.Code]);

#define GET_DURATION(_d, _s) (_d / _s)
		try
		{
			KeybdEventContinued(VK_RIGHT, GET_DURATION(214000, CharacterInfo.Speed));
			UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
			Sleep(500);
			for (int i = 0; i < 9; i++)
			{
				MouseEvent({ 870, 561 }, CLIC_GAUCHE, 280);
			}

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBossTimer, seconds(120));
				},
				7000,
				true);

			RaidCompleteRequest
			(
				2500,
				-4500,
				[this, &CharacterInfo]()
				{
					Sleep(600);
					if (USERCONF::CODE_PROPRE::아크 == CharacterInfo.Code)
					{
						KeybdEvent('Z');
					}
					
					switch (CharacterInfo.Code)
					{
					case USERCONF::CODE_PROPRE::데벤:
						break;

					default:
						KeybdEventContinued(VK_LEFT, 8000);
						KeybdEventContinued(VK_RIGHT, GET_DURATION(668000, CharacterInfo.Speed));
						break;
					}
					Sleep(600);

					if (VALLOC MatchInfo;
						MatchTemplate(SourceImageClient4, TargetImageNpcPetrick, &MatchInfo))
					{
						MouseEvent(MatchInfo.Location, CLIC_GAUCHE);

						for (const auto& TargetImage : ArrTargetImageItem1)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
								KeybdEvent({ VK_RETURN, VK_RETURN }, 400);
							}
						}

						for (const auto& TargetImage : ArrTargetImageItem2)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
								KeybdEvent({ '3', '0', VK_RETURN, VK_RETURN }, 400);
							}
						}

						MouseEvent({ 660, 220 }, CLIC_GAUCHE);
						MouseEvent(POS_VOID, CLIC_GAUCHE);
					}

					if (USERCONF::CODE_PROPRE::데슬 == CharacterInfo.Code || USERCONF::CODE_PROPRE::아란== CharacterInfo.Code)
					{
						KeybdEvent('Q', 3000);
					}
					else if (USERCONF::CODE_PROPRE::엔버 == CharacterInfo.Code)
					{
						KeybdEventContinued(KeysetInfo.Attack, 3000);
					}
					else
					{
						KeybdEvent(KeysetInfo.Attack, 3000);
					}
				}
			);
		}
		catch (int)
		{
			Write(SNAP_DIR "rootabyss-raid//" + to_string(++SeqFail) + ".jpg", SourceImageClient4Colored);
			throw;
		}
	}
};

class RootAbyssVonBanRaid : protected BossRaid
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
		static map<USERCONF::CODE_PROPRE, vector<SKILL>> MapVecSkills;
		MapVecSkills[USERCONF::CODE_PROPRE::닼나] =
		{
			{'1', SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{ 'R', SKILL::ASSIST, milliseconds(1200) },
			{ 'E', SKILL::ASSIST, milliseconds(1000), seconds(10)},
			{ 'W', SKILL::UNITARY }
		};
		MapVecSkills[USERCONF::CODE_PROPRE::불독] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::썬콜] =
		{ 
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{'T', SKILL::ASSIST, milliseconds(1000), seconds(180), seconds(1)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::비숍] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::보마] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::신궁] =
		{
			{'O', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{VK_F4, SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::패파] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나로] =
		{
			{'P', SKILL::ONOFF},
			{'2', SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{VK_F4, SKILL::BUF},
			{'R', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::섀도어] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::듀블] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::바이퍼] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캡틴] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::캐슈] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::소마] =
		{
			{'T', SKILL::ASSIST},
			{'R', SKILL::ASSIST},
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::플위] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::윈브] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::나워] =
		{
			{'P', SKILL::ONOFF},
			{'1', SKILL::BUF},
			{'3', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{'T', SKILL::ASSIST, milliseconds(800), seconds(180), seconds(1)},
			{'W', SKILL::ASSIST, milliseconds(800), seconds(8)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::스커] =
		{
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아란] =
		{
			{VK_F1, SKILL::BUF},
			{VK_F2, SKILL::BUF},
			{'L', SKILL::BUF},
			{'Z', SKILL::ASSIST, milliseconds(1000), seconds(150)},
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(20)},
			{'Q', SKILL::UNITARY},
		};
		MapVecSkills[USERCONF::CODE_PROPRE::에반] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메세] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::팬텀] =
		{
			{'1', SKILL::BUF},
			{'2', SKILL::BUF},
			{'S', SKILL::BUF},
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(150), seconds(5)},
			{'W', SKILL::ASSIST, milliseconds(1000), seconds(15)},
			{'A', SKILL::UNITARY}
		},
		MapVecSkills[USERCONF::CODE_PROPRE::루미] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::은월] =
		{
			{'4', SKILL::BUF},
			{'T', SKILL::ASSIST},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데슬] =
		{
			{'2', SKILL::BUF, milliseconds(1300)},
			{VK_F2, SKILL::BUF},
			{VK_F4, SKILL::BUF},
			{'T', SKILL::ASSIST},
			{'W', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::데벤] =
		{
			{'3', BossRaid::SKILL::BUF},
			{VK_F1, BossRaid::SKILL::BUF},
			{VK_F2, BossRaid::SKILL::BUF},
			{VK_F3, BossRaid::SKILL::BUF},
			{'T', BossRaid::SKILL::ASSIST, milliseconds(1200)},
			{'E', BossRaid::SKILL::ASSIST, milliseconds(800), seconds(7)},
			{'Q', BossRaid::SKILL::UNITARY, milliseconds(600)}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::블래] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::배메] =
		{
			{'L', SKILL::ONOFF},
			{'P', SKILL::ONOFF},
			{VK_F2, SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{'D', SKILL::BUF},
			{'R', SKILL::BUF},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::와헌] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제논] =
		{
			{'2', SKILL::BUF},
			{VK_F1, SKILL::BUF},
			{'3', SKILL::BUF},
			{'4', SKILL::BUF},
			{'T', SKILL::ASSIST},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::메카닉] =
		{
			{VK_F2, SKILL::BUF},
			{VK_F3, SKILL::BUF},
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(200), seconds(8)},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::카이저] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::엔버] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::제로] =
		{
			{'R', SKILL::ASSIST, milliseconds(1000), seconds(180), seconds(3)},
			{'Z', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::키네] =
		{
			{'F', SKILL::BUF},
			{'W', SKILL::BUF},
			{'T', SKILL::ASSIST},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::호영] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::일리움] =
		{
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아크] =
		{
			{'T', SKILL::ASSIST},
			{'R', SKILL::ASSIST},
			{'Q', SKILL::UNITARY}
		};
		MapVecSkills[USERCONF::CODE_PROPRE::아델] =
		{
			{'R', SKILL::ASSIST},
			{'E', SKILL::ASSIST, milliseconds(1000), seconds(60)},
			{'S', SKILL::ASSIST, milliseconds(1000), seconds(30)},
			{'Q', SKILL::UNITARY}
		};
		SetSkills(&MapVecSkills[CharacterInfo.Code]);

		try
		{
			switch (CharacterInfo.Code)
			{
			default:
				break;
			case USERCONF::CODE_PROPRE::배메:
				ClientApi::SupprimerBuf(1);
			}
			UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
			MouseEvent({ 995, 570 }, CLIC_GAUCHE, 2500);

			RaidDoBattle(
				[]() -> bool
				{
					return WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBossTimer, seconds(120));
				},
				7000,
					true);

			RaidCompleteRequest(
				-3600,
				8000,
				[this, &CharacterInfo]()
				{
					Sleep(1000);
					if (USERCONF::CODE_PROPRE::아크 == CharacterInfo.Code)
					{
						KeybdEvent('Z');
					}

					if (VALLOC MatchInfo;
						MatchTemplate(SourceImageClient4, TargetImageNpcPetrick, &MatchInfo))
					{
						MouseEvent(MatchInfo.Location, CLIC_GAUCHE);

						for (const auto& TargetImage : ArrTargetImageItem1)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
								KeybdEvent({ VK_RETURN, VK_RETURN }, 400);
							}
						}

						for (const auto& TargetImage : ArrTargetImageItem2)
						{
							if (MatchTemplate(Capture({ 0, 0, 600, 750 }, IMREAD_COLOR), TargetImage, &MatchInfo))
							{
								MouseEvent(MatchInfo.Location, DOUBLECLIC_GAUCHE);
								KeybdEvent({ '3', '0', VK_RETURN, VK_RETURN }, 400);
							}
						}

						MouseEvent({ 660, 220 }, CLIC_GAUCHE);
						MouseEvent(POS_VOID, CLIC_GAUCHE);
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