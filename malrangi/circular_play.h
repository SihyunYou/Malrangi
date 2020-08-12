#pragma once
#include "raid.h"
#include "calc.h"
#include "map.h"

class AppException : public MalrangiException
{
public:
	bool IsRetryAvailable;

	AppException(string Description, bool IsRetryAvailable) :
		MalrangiException(__CLASSNAME__ + "! " + Description),
		IsRetryAvailable(IsRetryAvailable) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};
#define INVALID_MAP_EXCEPTION(_n) "InvalidMapException(MapName = " + to_string(_n) + ")" 

class UrusPlay : private UrusRaid, private Bridge
{
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		try
		{
			switch (int CurrentMap = WhereAmI())
			{
			case Zacum2_2:
				MoveFromAToB(Zacum2_2, Zacum1, CharacterInfo);
			case Zacum1:
				MoveFromAToB(Zacum1, Elnas, CharacterInfo);
				goto __Elnas_U;

			case RootAbyss1:
				MoveFromAToB(RootAbyss1, Elnas, CharacterInfo);
				goto __Elnas_U;
			case FreeMarket:
				MoveFromAToB(FreeMarket, ElnasMarket, CharacterInfo);
			case ElnasMarket:
				MoveFromAToB(ElnasMarket, Elnas, CharacterInfo);
				goto __Elnas_U;

			__Elnas_U:
			case Elnas:
				MoveFromAToB(Elnas, Urus, CharacterInfo);
			case Urus:
				break;

			default: 
				throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
			}

			for (; SeqPlay <= 3; SeqPlay++)
			{
				UrusRaid::Play(CharacterInfo);
			}
		}
		catch (BRIDGE_EXCEPTION_CODE& ExceptionCode)
		{
			bool IsRetryAvailable;
			switch (ExceptionCode)
			{
			case BRIDGE_EXCEPTION_CODE::A_FAIL:
			case BRIDGE_EXCEPTION_CODE::B_FAIL:
				IsRetryAvailable = true;
				break;
			case BRIDGE_EXCEPTION_CODE::A_ERRORINPUT:
			case BRIDGE_EXCEPTION_CODE::B_ERRORINPUT:
				IsRetryAvailable = false;
				break;
			}

			throw AppException("MoveAToBFailedException", IsRetryAvailable);
		}
		catch (URUSRAID_EXCEPTION_CODE& ExceptionCode)
		{
			switch (ExceptionCode)
			{
			case URUSRAID_EXCEPTION_CODE::BUTTON_READY_NOT_FOUND:
			case URUSRAID_EXCEPTION_CODE::BATTLE_ENTRY_TIMEOUT:
			case URUSRAID_EXCEPTION_CODE::BATTLE_TIMEOUT:
			case URUSRAID_EXCEPTION_CODE::EXIT_TO_U3_FAILED:
			case URUSRAID_EXCEPTION_CODE::NPC_MASHUR_NOT_FOUND:
				++SeqPlay;
				break;

			case URUSRAID_EXCEPTION_CODE::PARTY_UNBREAKABLE:
			case URUSRAID_EXCEPTION_CODE::PLAY_SEQUENCE_OVER:
			default:
				SeqPlay = 3;
			}

			bool IsRetryAvailable = SeqPlay <= 2;
			if (!IsRetryAvailable)
			{
				SeqPlay = 1;
			}

			throw AppException("UrusRaidException", IsRetryAvailable);
		}

		SeqPlay = 1;
	}
};
class DailyBossPlay : private ZacumRaid, private RootAbyssRaid, private Bridge
{
public:
	void Play(USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		try
		{
			int Priority; // ÀÚÄñ / ºí·¯µðÄý
			switch (WhereAmI())
			{
			case Zacum1:
			case Zacum2_2:
			case Zacum3_2:
				Priority = USERCONF::CHARACTER_INFO::ZACUM;
				break;

			case RootAbyss1:
			case RootAbyss2_1:
			case RootAbyss3_1:
			case Urus:
			case Elnas:
			case ElnasMarket:
			case FreeMarket:
			default:
				Priority = USERCONF::CHARACTER_INFO::ROOTABYSS;
				break;
			}

			if (USERCONF::CHARACTER_INFO::ZACUM == Priority)
			{
				goto __BEGIN_ZACUM;
			}

			while (CharacterInfo.Type)
			{
				if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ROOTABYSS)
				{
					switch (int CurrentMap = WhereAmI())
					{
					case Zacum3_2:
						MoveFromAToB(Zacum3_2, Zacum2_2, CharacterInfo);
					case Zacum2_2:
						MoveFromAToB(Zacum2_2, Zacum1, CharacterInfo);
					__Zacum1:
					case Zacum1:
						MoveFromAToB(Zacum1, Elnas, CharacterInfo);
						goto __Elnas;

					case Urus:
						MoveFromAToB(Urus, Elnas, CharacterInfo);
						goto __Elnas;

					case FreeMarket:
						MoveFromAToB(FreeMarket, ElnasMarket, CharacterInfo);
					case ElnasMarket:
						MoveFromAToB(ElnasMarket, Elnas, CharacterInfo);
						goto __Elnas;

					__Elnas:
					case Elnas:
						MoveFromAToB(Elnas, RootAbyss1, CharacterInfo);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ROOTABYSS);
						MoveFromAToB(RootAbyss1, RootAbyss2_1, CharacterInfo);
						MoveFromAToB(RootAbyss2_1, RootAbyss3_1, CharacterInfo);
						break;

					default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
					}

					RootAbyssRaid::Play(CharacterInfo);
				}

			__BEGIN_ZACUM:
				if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ZACUM)
				{
					switch (int CurrentMap = WhereAmI())
					{
					case RootAbyss3_1:
						MoveFromAToB(RootAbyss3_1, RootAbyss1, CharacterInfo);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ZACUM);
						MoveFromAToB(RootAbyss1, Zacum1, CharacterInfo);
						goto __Zacum1_Z;
					case Urus:
						MoveFromAToB(Urus, Elnas, CharacterInfo);
						goto __Elnas_Z;

					case FreeMarket:
						MoveFromAToB(FreeMarket, ElnasMarket, CharacterInfo);
					case ElnasMarket:
						MoveFromAToB(ElnasMarket, Zacum1, CharacterInfo);
						goto __Zacum1_Z;

					__Elnas_Z:
					case Elnas:
						UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ZACUM);
						MoveFromAToB(Elnas, Zacum1, CharacterInfo);
					__Zacum1_Z:
					case Zacum1:
						MoveFromAToB(Zacum1, Zacum2_2, CharacterInfo);
					case Zacum2_2:
						UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ZACUM);
						MoveFromAToB(Zacum2_2, Zacum3_2, CharacterInfo);
						break;

					default:
						throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
					}

					ZacumRaid::Play(CharacterInfo);
				}
			}
		}
		catch (BRIDGE_EXCEPTION_CODE ExceptionCode)
		{
			bool IsRetryAvailable;
			switch (ExceptionCode)
			{
			case BRIDGE_EXCEPTION_CODE::A_FAIL:
			case BRIDGE_EXCEPTION_CODE::B_FAIL:
				IsRetryAvailable = CharacterInfo.Type ? true : false;
				break;
			case BRIDGE_EXCEPTION_CODE::A_ERRORINPUT:
			case BRIDGE_EXCEPTION_CODE::B_ERRORINPUT:
			default:
				IsRetryAvailable = false;
			}

			throw AppException("MoveAToBFailedException", IsRetryAvailable);
		}
		catch (BOSSRAID_EXCEPTION_CODE & ExceptionCode)
		{
			throw AppException("BossRaidException", ARE_FLAGS_ON(CharacterInfo.Type));
		}
		catch (ZACUMRAID_EXCEPTION_CODE & ExceptionCode)
		{
			throw AppException("ZacumRaidException", ARE_FLAGS_ON(CharacterInfo.Type));
		}
	}
};
class CalcPlay : private Calc, private Bridge
{
public:
	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo,
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		try
		{
			switch (int CurrentMap = WhereAmI())
			{
			case Zacum2_2:
				MoveFromAToB(Zacum2_2, Zacum1, CharacterInfo);
			case Zacum1:
				MoveFromAToB(Zacum1, Elnas, CharacterInfo);
				goto __Elnas__;

			case Urus:
				MoveFromAToB(Urus, Elnas, CharacterInfo);
				goto __Elnas__;
			case RootAbyss1:
				MoveFromAToB(RootAbyss1, Elnas, CharacterInfo);
				goto __Elnas__;
			case ElnasMarket:
				MoveFromAToB(ElnasMarket, Elnas, CharacterInfo);
				goto __Elnas__;

			__Elnas__:
			case Elnas:
				MoveFromAToB(Elnas, FreeMarket, CharacterInfo);
				break;

			case FreeMarket:
				break;
			}

			Calc::Play(MapleIdInfo);
		}
		catch (BRIDGE_EXCEPTION_CODE ExceptionCode)
		{
			bool IsRetryAvailable;
			switch (ExceptionCode)
			{
			case BRIDGE_EXCEPTION_CODE::A_FAIL:
			case BRIDGE_EXCEPTION_CODE::B_FAIL:
				IsRetryAvailable = true;
				break;
			case BRIDGE_EXCEPTION_CODE::A_ERRORINPUT:
			case BRIDGE_EXCEPTION_CODE::B_ERRORINPUT:
				IsRetryAvailable = false;
				break;
			}

			throw AppException("MoveAToBFailedException", IsRetryAvailable);
		}
		catch (CALC_EXCEPTION_CODE ExceptionCode)
		{
			throw AppException("CalcFailedException", true);
		}
	}
};