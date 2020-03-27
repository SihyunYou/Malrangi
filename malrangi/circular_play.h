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

class UrusPlay : private UrusRaid, private Bridge
{
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		if (!AmIIn(Urus))
		{
			try
			{
				switch (int CurrentMap = WhereAmI())
				{
				case Zacum2_2:
				case Zacum2_3:
					MoveFromAToB(CurrentMap, Zacum1);
				case Zacum1:
					MoveFromAToB(Zacum1, Elnas);
					goto __Elnas_U;

				case RootAbyss1:
					MoveFromAToB(RootAbyss1, Elnas);
					goto __Elnas_U;
				case ElnasMarket:
					MoveFromAToB(ElnasMarket, Elnas);
					goto __Elnas_U;

				__Elnas_U:
				case Elnas:
					MoveFromAToB(Elnas, Urus);
					break;
				}
			}
			catch (int ErrorCode)
			{
				bool IsRetryAvailable;
				switch (ErrorCode)
				{
				case A_FAIL:
				case B_FAIL:
					IsRetryAvailable = true;
					break;
				case A_ERRORINPUT:
				case B_ERRORINPUT:
				default:
					IsRetryAvailable = false;
				}

				throw AppException("MoveAToBFailedException", IsRetryAvailable);
			}
		}

		bool IsTryFirst = true;
		for (; SeqPlay <= 3; SeqPlay++)
		{
			try
			{
				UrusRaid::Play(CharacterInfo, IsTryFirst);
				IsTryFirst = false;
			}
			catch (int ErrorCode)
			{
				string Message;
				bool IsCounted = true;

				switch (ErrorCode)
				{
				case BUTTON_READY_NOT_FOUND:
					Message = "ButtonReadyNotFoundException";
					IsCounted = false;
					break;
				case BATTLE_ENTRY_TIMEOUT:
					Message = "BattleEntryTimeoutException";
					break;
				case BATTLE_TIMEOUT:
					Message = "BattleTimeoutException";
					break;
				case EXIT_TO_U3_FAILED:
					Message = "ExitToU3FailedException";
					break;
				case NPC_MASHUR_NOT_FOUND:
					Message = "NpcMashurNotFoundException";
					break;

				default:
					Message = "UnknownUrusException";
				}

				if (IsCounted)
				{
					++SeqPlay;
				}
				throw AppException(Message, SeqPlay <= 2);
			}
		}

		SeqPlay = 1;
	}
};
#define INVALID_MAP_EXCEPTION(NumberOfMap) "InvalidMapException(MapName = " + MapFileName[NumberOfMap] + ")" 
class DailyBossPlay : private ZacumRaid, private RootAbyssRaid, private Bridge
{
public:
	void Play(USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		int CurrentMap;
		if (MeisterVill == CurrentMap)
		{
			;
		}
		else if (FreeMarket == CurrentMap)
		{
			;
		}

	
		auto IsRetryAvailable = [](EXCEPTION_CODE ec, int CharacterType)
		{
			switch (ec)
			{
			case EXCEPTION_CODE::A_FAIL:
			case EXCEPTION_CODE::B_FAIL:
				return CharacterType ? true : false;
				break;
			case EXCEPTION_CODE::A_ERRORINPUT:
			case EXCEPTION_CODE::B_ERRORINPUT:
			default:
				return false;
			}
		};

		if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ROOTABYSS)
		{
			bool IsExceptionSafe = true;
			try
			{
				switch (CurrentMap = WhereAmI())
				{
				case Zacum3_2:
					MoveFromAToB(Zacum3_2, Zacum2_2);
					CurrentMap = Zacum2_2;
				case Zacum2_2:
				case Zacum2_3:
					MoveFromAToB(CurrentMap, Zacum1);
				case Zacum1:
					MoveFromAToB(Zacum1, Elnas);
					goto __Elnas;

				case Urus:
					MoveFromAToB(Urus, Elnas);
					goto __Elnas;

				case ElnasMarket:
					MoveFromAToB(ElnasMarket, Elnas);
					goto __Elnas;

				__Elnas:
				case Elnas:
					MoveFromAToB(Elnas, RootAbyss1);
					IsExceptionSafe = false;
				case RootAbyss1:
					MoveFromAToB(RootAbyss1, RootAbyss2_1);
					MoveFromAToB(RootAbyss2_1, RootAbyss3_1);
					break;

				default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
				}
			}
			catch (EXCEPTION_CODE ExceptionCode)
			{
				if (!IsExceptionSafe)
				{
					CharacterInfo.Type -= USERCONF::CHARACTER_INFO::ROOTABYSS;
				}
				throw AppException("MoveAToBFailedException", IsRetryAvailable(ExceptionCode, CharacterInfo.Type));
			}

			try
			{
				CharacterInfo.Type -= USERCONF::CHARACTER_INFO::ROOTABYSS;
				RootAbyssRaid::Play(CharacterInfo);
			}
			catch (int ErrorCode)
			{
				throw AppException("RootAbyssRaidFailedException", CharacterInfo.Type ? true : false);
			}
		}

		if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ZACUM)
		{
			bool IsExceptionSafe = true;
			try
			{
				switch (CurrentMap = WhereAmI())
				{
				case RootAbyss3_1:
					MoveFromAToB(RootAbyss3_1, RootAbyss1);
				case RootAbyss1:
					MoveFromAToB(RootAbyss1, Elnas);
					goto __Elnas_Z;

				case Urus:
					MoveFromAToB(Urus, Elnas);
					goto __Elnas_Z;

				__Elnas_Z:
				case Elnas:
					MoveFromAToB(Elnas, ElnasOffice);
				case ElnasOffice:
					MoveFromAToB(ElnasOffice, Zacum1);
				__Zacum1_Z:
				case Zacum1:
					MoveFromAToB(Zacum1, Zacum2_2);
				case Zacum2_2:
					IsExceptionSafe = false;
					MoveFromAToB(Zacum2_2, Zacum3_2);
					break;

				case Zacum2_3:
					MoveFromAToB(Zacum2_3, Zacum1);
					goto __Zacum1_Z;

				default:
					throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
				}
			}
			catch (EXCEPTION_CODE ExceptionCode)
			{
				if (!IsExceptionSafe)
				{
					CharacterInfo.Type -= USERCONF::CHARACTER_INFO::ZACUM;
				}
				throw AppException("MoveAToBFailedException", IsRetryAvailable(ExceptionCode, CharacterInfo.Type));
			}
			
			try
			{
				CharacterInfo.Type -= USERCONF::CHARACTER_INFO::ZACUM;
				ZacumRaid::Play(CharacterInfo);
			}
			catch (int ErrorCode)
			{
				throw AppException("ZacumRaidFailedException", CharacterInfo.Type ? true : false);
			}
		}
	}
};
class CalcPlay : private Calc, private Bridge
{
public:
	void Play(const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		switch (WhereAmI())
		{
		case Urus:
			MoveFromAToB(Urus, Elnas);
			MoveFromAToB(Elnas, FreeMarket);
			Calc::Play(MapleIdInfo);
			MoveFromAToB(FreeMarket, ElnasMarket);
			MoveFromAToB(ElnasMarket, Elnas);
			MoveFromAToB(Elnas, Urus);
			break;

		case Zacum2_2:
			MoveFromAToB(Zacum2_2, Zacum1);
			MoveFromAToB(Zacum1, Zacum2_3);
		case Zacum2_3:
			MoveFromAToB(Zacum2_3, MeisterVill);
			MoveFromAToB(MeisterVill, FreeMarket);
			Calc::Play(MapleIdInfo);
			MoveFromAToB(FreeMarket, MeisterVill);
			MoveFromAToB(MeisterVill, Zacum2_3);
			break;
		}
	}
};