#pragma once
#include "client.h"

#pragma comment(lib,"winmm.lib")

class AppException : public MalrangiException
{
public:
	AppException(void) :
		MalrangiException(__CLASSNAME__) {}
	AppException(string Description) :
		MalrangiException(__CLASSNAME__ + "! " + Description) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};

class BossRaid
{
protected:
	struct SKILL
	{
		BYTE Key;
		enum
		{
			UNITARY,
			RAPID,
			BUF,
			ASSIST,
			ONOFF
		};
		DWORD Type;
		seconds Cooltime;
		system_clock::time_point LastRenewedTime;

		SKILL() = default;
		SKILL(BYTE Key, DWORD Type, seconds Cooltime = seconds(180)) :
			Key(Key),
			Type(Type),
			Cooltime(Cooltime),
			LastRenewedTime(system_clock::now())
		{

		}
	};

public:
	int SeqSuccess, SeqFail;

protected:
	vector<SKILL>* VecSkills = nullptr;
	SKILL MainSkill;
	const USERCONF::KEYSET_INFO& KeysetInfo;
	bool IsFirstLoop;

	inline static const Mat TargetImageItemPlusCoin = Read(TARGET_DIR "item//pluscoin.jpg");

public:
	BossRaid() :
		SeqSuccess(0), SeqFail(0),
		VecSkills(nullptr),
		KeysetInfo(USERCONF::GetInstance()->VirtualKeyset),
		IsFirstLoop(true) { }

protected:
	void SetSkills(vector<SKILL>* _VecSkills)
	{
		VecSkills = _VecSkills;

		for (const auto& Skill : *VecSkills)
		{
			if (SKILL::UNITARY == Skill.Type || SKILL::RAPID == Skill.Type)
			{
				MainSkill = Skill;
				return;
			}
		}
	}
	template <class T_LAMBDA>
	void RaidCallBoss(T_LAMBDA CallBossRoutine)
	{
		for (const auto& Skill : *VecSkills)
		{
			if (SKILL::BUF == Skill.Type || SKILL::ONOFF == Skill.Type)
			{
				KeybdEvent(Skill.Key, 1200);
			}
		}

		CallBossRoutine();
	}
	template <class T_LAMBDA>
	void RaidDoBattle(T_LAMBDA PendingUntilMatchingTemplateRoutine, bool IsFixed)
	{
		auto IsCoolOn = [](const SKILL& Skill) -> bool
		{
			return duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime) > Skill.Cooltime;
		};
		auto UseBufAssist =
			[MainKey = MainSkill.Key, Delay = (SKILL::UNITARY == MainSkill.Type) ? 800 : 100]
		(SKILL& Skill) -> void
		{
			KeybdEventUp(MainKey);
			Sleep(Delay);
			Skill.LastRenewedTime += duration_cast<seconds>(system_clock::now() - Skill.LastRenewedTime);
			KeybdEvent(Skill.Key, 800);
		};

		bool IsAllowedToBeComplete = false;
		auto ThreadResurrection = thread(
			[&, this]()
			{
				while (!IsAllowedToBeComplete)
				{
					MouseEvent({ 800, 320 }, LEFT_CLICK, 0x80);
				}
			}
		);
		auto ThreadBattle = thread(
			[&, this]()
			{
				do
				{
					for (auto& Skill : *VecSkills)
					{
						switch (Skill.Type)
						{
						case SKILL::ASSIST:
							if (IsFirstLoop || IsCoolOn(Skill))
							{
								IsFirstLoop = false;
								UseBufAssist(Skill);
							}
							break;
						case SKILL::BUF:
							if (IsCoolOn(Skill))
							{
								UseBufAssist(Skill);
							}
							break;

						case SKILL::UNITARY:
						case SKILL::RAPID:
							KeybdEventDown(Skill.Key);
							break;
						}

						Sleep(0x40);
					}
				} while (!IsAllowedToBeComplete);
			}
		);

		IsAllowedToBeComplete = PendingUntilMatchingTemplateRoutine();

		if (ThreadBattle.joinable())
		{
			ThreadBattle.join();
		}
		if (ThreadResurrection.joinable())
		{
			ThreadResurrection.join();
		}

		if (!IsAllowedToBeComplete)
		{
			throw AppException(__FEWHAT__);
		}
	}

private:
	void CompleteRaid()
	{
		IsFirstLoop = true;
		for each (const auto & Skill in *VecSkills)
		{
			KeybdEventUp(Skill.Key);
		}
	}
protected: 
#define ArrowKeyFromInteger(_x) (_x < 0 ? VK_LEFT : VK_RIGHT)
	void RaidCompleteRequest(int PickingMilliseconds1, int PickingMilliseconds2)
	{
		CompleteRaid();

		KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds1), KeysetInfo.Picking, std::labs(PickingMilliseconds1));
		KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds2), KeysetInfo.Picking, std::labs(PickingMilliseconds2));
	}
	template <class T_LAMBDA>
	void RaidCompleteRequest(int PickingMilliseconds1, int PickingMilliseconds2, T_LAMBDA BeforePickingRoutine)
	{
		CompleteRaid();
		BeforePickingRoutine();
	
		if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageItemPlusCoin, seconds(10)))
		{
			KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds1), KeysetInfo.Picking, std::labs(PickingMilliseconds1));
			KeybdEventContinuedWithSubKey(ArrowKeyFromInteger(PickingMilliseconds2), KeysetInfo.Picking, std::labs(PickingMilliseconds2));
		}
		else
		{
			throw AppException("BeforePickingRoutineError");
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