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
			if (FreeMarket == WhereAmI())
			{
				MoveFromAToB(FreeMarket, Unknown);
			}

			switch (int CurrentMap = WhereAmI())
			{
			case MeisterVill:
				MoveFromAToB(MeisterVill, Zacum2_3);
				CurrentMap = Zacum2_3;
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
			case Urus:
				break;

			default: 
				throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
			}
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

		try
		{
			for (; SeqPlay <= 3; SeqPlay++)
			{
				UrusRaid::Play(CharacterInfo);
			}
		}
		catch (int ExceptionCode)
		{
			switch (ExceptionCode)
			{
			case PLAY_SEQUENCE_OVER:
				SeqPlay = 3;
				break;
			case BUTTON_READY_NOT_FOUND:
				break;
			case BATTLE_ENTRY_TIMEOUT:
			case BATTLE_TIMEOUT:
			case EXIT_TO_U3_FAILED:
			case NPC_MASHUR_NOT_FOUND:
			default:
				++SeqPlay;
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
private:
	bool IsRetryAvailable(BRIDGE_EXCEPTION_CODE ec, int CharacterType)
	{
		switch (ec)
		{
		case BRIDGE_EXCEPTION_CODE::A_FAIL:
		case BRIDGE_EXCEPTION_CODE::B_FAIL:
			return CharacterType ? true : false;
			break;
		case BRIDGE_EXCEPTION_CODE::A_ERRORINPUT:
		case BRIDGE_EXCEPTION_CODE::B_ERRORINPUT:
		default:
			return false;
		}
	};

public:
	void Play(USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ROOTABYSS)
		{
			try
			{
				if (FreeMarket == WhereAmI())
				{
					MoveFromAToB(FreeMarket, Unknown);
				}

				switch (int CurrentMap = WhereAmI())
				{
				case MeisterVill:
					MoveFromAToB(MeisterVill, Zacum2_3);
					CurrentMap = Zacum2_3;
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
				case RootAbyss1:
					UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ROOTABYSS);
					MoveFromAToB(RootAbyss1, RootAbyss2_1);
					MoveFromAToB(RootAbyss2_1, RootAbyss3_1);
					break;

				default: throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
				}
			}
			catch (BRIDGE_EXCEPTION_CODE ExceptionCode)
			{
				throw AppException("MoveAToBFailedException", IsRetryAvailable(ExceptionCode, CharacterInfo.Type));
			}

			try
			{
				RootAbyssRaid::Play(CharacterInfo);
			}
			catch (int)
			{
				throw AppException("RootAbyssRaidFailedException", ARE_FLAGS_ON(CharacterInfo.Type));
			}
		}

		if (CharacterInfo.Type & USERCONF::CHARACTER_INFO::ZACUM)
		{
			try
			{
				if (FreeMarket == WhereAmI())
				{
					MoveFromAToB(FreeMarket, Unknown);
				}

				bool IsReady = false;
				switch (int CurrentMap = WhereAmI())
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
					IsReady = true;
				case Zacum2_2:
					if (!IsReady)
					{
						MoveFromAToB(Zacum2_2, Zacum1);
						MoveFromAToB(Zacum1, Zacum2_2);
					}
					UNFLAG(CharacterInfo.Type, USERCONF::CHARACTER_INFO::ZACUM);
					MoveFromAToB(Zacum2_2, Zacum3_2);
					break;

				case MeisterVill:
					MoveFromAToB(MeisterVill, Zacum2_3);
				case Zacum2_3:
					MoveFromAToB(Zacum2_3, Zacum1);
					goto __Zacum1_Z;

				default:
					throw AppException(INVALID_MAP_EXCEPTION(CurrentMap), false);
				}
			}
			catch (BRIDGE_EXCEPTION_CODE ExceptionCode)
			{
				throw AppException("MoveAToBFailedException", IsRetryAvailable(ExceptionCode, CharacterInfo.Type));
			}

			try
			{
				ZacumRaid::Play(CharacterInfo);
			}
			catch (int)
			{
				throw AppException("RootAbyssRaidFailedException", ARE_FLAGS_ON(CharacterInfo.Type));
			}
		}
	}
};
class CalcPlay : private Calc, private Bridge
{
public:
	void Play(const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		try
		{
			if (FreeMarket == WhereAmI())
			{
				MoveFromAToB(FreeMarket, Unknown);
			}

			switch (WhereAmI())
			{
			case Zacum2_2:
				MoveFromAToB(Zacum2_2, Zacum1);
			case Zacum1:
				MoveFromAToB(Zacum1, Zacum2_3);
			case Zacum2_3:
				MoveFromAToB(Zacum2_3, MeisterVill);
				goto __MeisterVill__;

			case Urus:
				MoveFromAToB(Urus, Elnas);
				goto __Elnas__;
			case RootAbyss1:
				MoveFromAToB(RootAbyss1, Elnas);
				goto __Elnas__;
			case ElnasMarket:
				MoveFromAToB(ElnasMarket, Elnas);
				goto __Elnas__;

			__Elnas__:
			case Elnas:
				MoveFromAToB(Elnas, FreeMarket);
				break;

			__MeisterVill__:
			case MeisterVill:
				MoveFromAToB(MeisterVill, FreeMarket);
			case FreeMarket:
				break;
			}
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

		try
		{
			Calc::Play(MapleIdInfo);
		}
		catch (int ErrorCode)
		{
			throw AppException("CalcFailedException", true);
		}
	}
};