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
			CHARGING = 0b00100000
		};
		DWORD Flag;
		seconds Cooltime;
		system_clock::time_point LastRenewedTime;

		SKILL() = default;
		SKILL(BYTE Key, DWORD Flag, seconds Cooltime = seconds(180)) :
			Key(Key),
			Flag(Flag),
			Cooltime(Cooltime) { }
	};

public:
	int SeqSuccess, SeqFail;

public:
	vector<SKILL>* VecSkills = nullptr;
	SKILL MainSkill;
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
		KeysetInfo(USERCONF::GetInstance()->VirtualKeyset) {}

public:
	void SetSkills(vector<SKILL>* _VecSkills)
	{
		VecSkills = _VecSkills;

		for (const auto& Skill : *VecSkills)
		{
			if (IS_FLAG_ON(SKILL::UNITARY | SKILL::RAPID, Skill.Flag))
			{
				MainSkill = Skill;
				return;
			}
		}
	}
	void UseSkills(vector<SKILL>& VecSkills, int Flags)
	{
		auto UseSkill = [this](SKILL& Skill)
		{
			auto IsCoolOn = [](const SKILL& Skill)
			{
				return duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime) > Skill.Cooltime;
			};
			auto RenewTime = [](SKILL& Skill)
			{
				Skill.LastRenewedTime += duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime);
			};

			switch (Skill.Flag)
			{
			case SKILL::ONOFF:
				KeybdEventUp(MainSkill.Key);
				KeybdEvent(Skill.Key, 1000);
				break;

			case SKILL::CHARGING:
				if (IsCoolOn(Skill))
				{
					KeybdEventContinued(Skill.Key, 4000);
				}
				KeybdEventUp(Skill.Key);
				break;

			case SKILL::ASSIST:
			case SKILL::BUF:
				if (IsCoolOn(Skill))
				{
					KeybdEventUp(MainSkill.Key);
					Sleep((SKILL::UNITARY == MainSkill.Flag) ? 800 : 200);
					RenewTime(Skill);
					KeybdEvent(Skill.Key, 1200);
				}
				break;

			case SKILL::UNITARY:
			case SKILL::RAPID:
				KeybdEventDown(Skill.Key);
				break;
			}
		};

		for (auto& Skill : VecSkills)
		{
			if (IS_FLAG_ON(Flags, Skill.Flag))
			{
				UseSkill(Skill);
			}
			Sleep(0x20);
		}
	}

	template <class T_LAMBDA>
	void RaidCallBoss(T_LAMBDA CallBossRoutine)
	{
		UseSkills(*VecSkills, SKILL::BUF | SKILL::ONOFF);
		CallBossRoutine();
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
					MouseEvent({ 800, 320 }, LEFT_CLICK, ResurectionTimeCycle);
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



class WonkiDetect
{
public:
	WonkiDetect()
	{
		vector<Mat> VecTargetImage;
		VecTargetImage.push_back(Read(TARGET_DIR "pic//violetta.png"));

		while (true)
		{
			Mat SourceImage = Capture(ClientApi::RECT_CLIENT4);
			for each (const auto & TargetImage in VecTargetImage)
			{
				if (MatchTemplate(SourceImage, TargetImage))
				{
					PlaySoundA("gibbon-monkey-daniel_simon.wav", NULL, SND_ASYNC);
					Sleep(0x1600);
				}
			}
			Sleep(0x100);
		}
	}
};