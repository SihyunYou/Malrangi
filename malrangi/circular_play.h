#pragma once
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

#if defined(BUILD_URUS)
#include "urus.h"

class UrusPlay : private UrusRaid, private Bridge
{
public:
	void Play(
		const bool IsQuickCompletionMode)
	{
		try
		{
			bool EstGauche = false;
			switch (int CurrentMap = WhereAmI())
			{
			case Zacum2_2:
				BougerDeAAB(Zacum2_2, Zacum1);
			case Zacum1:
				BougerDeAAB(Zacum1, Elnas);
				goto __Elnas_U;

			case RootAbyss1:
				BougerDeAAB(RootAbyss1, Elnas);
				goto __Elnas_U;
			case FreeMarket:
				BougerDeAAB(FreeMarket, ElnasMarket);
			case ElnasMarket:
				BougerDeAAB(ElnasMarket, Elnas);
				goto __Elnas_U;

			__Elnas_U:
			case Elnas:
				EstGauche = true;
				BougerDeAAB(Elnas, Urus);
			case Urus:
				break;

			default:
				throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
			}

			for (; SeqPlay <= 3; SeqPlay++)
			{
				UrusRaid::Play(IsQuickCompletionMode, EstGauche);
			}
		}
		catch (BRIDGE_EXCEPTION_CODE & ExceptionCode)
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
		catch (URUSRAID_EXCEPTION_CODE & ExceptionCode)
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
#endif
#if defined(BUILD_DAILYBOSS)
#include "dailyboss.h"

class DailyBossPlay : private ZacumRaid, private RootAbyssBloodyQueenRaid, private RootAbyssVonBanRaid, private Bridge
{
public:
	void Play(USERCONF::CHARACTER_INFO& CharacterInfo)
	{
	__CIRCULAR_REPLAY__:
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
				Priority = USERCONF::CHARACTER_INFO::ROOTABYSS_BLOODYQUEEN;
				break;
			}

			if (USERCONF::CHARACTER_INFO::ZACUM == Priority)
			{
				goto __BEGIN_ZACUM;
			}

			while (CharacterInfo.Flag)
			{
				if (CharacterInfo.Flag & USERCONF::CHARACTER_INFO::ROOTABYSS_BLOODYQUEEN)
				{
					switch (int CurrentMap = WhereAmI())
					{
					case Zacum3_2:
						BougerDeAAB(Zacum3_2, Zacum2_2);
					case Zacum2_2:
						BougerDeAAB(Zacum2_2, Zacum1);
					case Zacum1:
						BougerDeAAB(Zacum1, Elnas);
						goto __Elnas;

					case Urus:
						BougerDeAAB(Urus, Elnas);
						goto __Elnas;

					case FreeMarket:
						BougerDeAAB(FreeMarket, ElnasMarket);
					case ElnasMarket:
						BougerDeAAB(ElnasMarket, Elnas);
						goto __Elnas;

					__Elnas:
					case Elnas:
						BougerDeAAB(Elnas, RootAbyss1);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, USERCONF::CHARACTER_INFO::ROOTABYSS_BLOODYQUEEN);
						BougerDeAAB(RootAbyss1, RootAbyss2_1, &CharacterInfo);
						BougerDeAAB(RootAbyss2_1, RootAbyss3_1, &CharacterInfo);
						break;

					default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
					}

					RootAbyssBloodyQueenRaid::Play(CharacterInfo);
				}

				if (CharacterInfo.Flag & USERCONF::CHARACTER_INFO::ROOTABYSS_VONBAN)
				{
					switch (int CurrentMap = WhereAmI())
					{
					case Zacum3_2:
						BougerDeAAB(Zacum3_2, Zacum2_2);
					case Zacum2_2:
						BougerDeAAB(Zacum2_2, Zacum1);
					case Zacum1:
						BougerDeAAB(Zacum1, Elnas);
						goto __Elnas_;

					case Urus:
						BougerDeAAB(Urus, Elnas);
						goto __Elnas_;

					case FreeMarket:
						BougerDeAAB(FreeMarket, ElnasMarket);
					case ElnasMarket:
						BougerDeAAB(ElnasMarket, Elnas);
						goto __Elnas_;

					case RootAbyss3_1:
						BougerDeAAB(RootAbyss3_1, RootAbyss1);
						goto __RootAbyss1;

					__Elnas_:
					case Elnas:
						BougerDeAAB(Elnas, RootAbyss1);
					__RootAbyss1:
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, USERCONF::CHARACTER_INFO::ROOTABYSS_VONBAN);
						BougerDeAAB(RootAbyss1, RootAbyss2_3, &CharacterInfo);
						BougerDeAAB(RootAbyss2_3, RootAbyss3_3, &CharacterInfo);
						break;

					default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
					}

					RootAbyssVonBanRaid::Play(CharacterInfo);
				}

			__BEGIN_ZACUM:
				if (CharacterInfo.Flag & USERCONF::CHARACTER_INFO::ZACUM)
				{
					switch (int CurrentMap = WhereAmI())
					{
					case RootAbyss3_1:
					case RootAbyss3_3:
						BougerDeAAB(CurrentMap, RootAbyss1);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, USERCONF::CHARACTER_INFO::ZACUM);
						BougerDeAAB(RootAbyss1, Zacum1);
						goto __Zacum1_Z;
					case Urus:
						BougerDeAAB(Urus, Elnas);
						goto __Elnas_Z;

					case FreeMarket:
						BougerDeAAB(FreeMarket, ElnasMarket);
					case ElnasMarket:
						BougerDeAAB(ElnasMarket, Zacum1);
						goto __Zacum1_Z;

					__Elnas_Z:
					case Elnas:
						UNFLAG(CharacterInfo.Flag, USERCONF::CHARACTER_INFO::ZACUM);
						BougerDeAAB(Elnas, Zacum1);
					__Zacum1_Z:
					case Zacum1:
						BougerDeAAB(Zacum1, Zacum2_2);
					case Zacum2_2:
						UNFLAG(CharacterInfo.Flag, USERCONF::CHARACTER_INFO::ZACUM);
						BougerDeAAB(Zacum2_2, Zacum3_2);
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
				IsRetryAvailable = CharacterInfo.Flag ? true : false;
				break;
			case BRIDGE_EXCEPTION_CODE::DEJA_ACCOMPLI:
				goto __CIRCULAR_REPLAY__;
			case BRIDGE_EXCEPTION_CODE::A_ERRORINPUT:
			case BRIDGE_EXCEPTION_CODE::B_ERRORINPUT:
			case BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE:
			default:
				IsRetryAvailable = false;
				break;
			}

			throw AppException("MoveAToBFailedException", IsRetryAvailable);
		}
		catch (BOSSRAID_EXCEPTION_CODE & ExceptionCode)
		{
			throw AppException("BossRaidException", ARE_FLAGS_ON(CharacterInfo.Flag));
		}
		catch (ZACUMRAID_EXCEPTION_CODE & ExceptionCode)
		{
			throw AppException("ZacumRaidException", ARE_FLAGS_ON(CharacterInfo.Flag));
		}
	}
};
#endif 
#if defined(BUILD_CALC)
#include "calc.h"

class CalcPlay : private Calc, private Bridge
{
public:
	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		try
		{
			switch (int CurrentMap = WhereAmI())
			{
			case Zacum3_2:
				BougerDeAAB(Zacum3_2, Zacum2_2);
			case Zacum2_2:
				BougerDeAAB(Zacum2_2, Zacum1);
			case Zacum1:
				BougerDeAAB(Zacum1, Elnas);
				goto __Elnas__;

			case Urus:
				BougerDeAAB(Urus, Elnas);
				goto __Elnas__;

			case RootAbyss3_1:
			case RootAbyss3_3:
				BougerDeAAB(CurrentMap, RootAbyss1);
			case RootAbyss1:
				BougerDeAAB(RootAbyss1, Elnas);
				goto __Elnas__;
			case ElnasMarket:
				BougerDeAAB(ElnasMarket, Elnas);
				goto __Elnas__;

			__Elnas__:
			case Elnas:
				BougerDeAAB(Elnas, FreeMarket);
				break;

			case FreeMarket:
				break;

			default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
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
			case BRIDGE_EXCEPTION_CODE::NULL_PARAMETRE:
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
#endif
