#pragma once
#include "raid.h"
#include "calc.h"
#include "map.h"

class UrusPlay : private UrusRaid, private Bridge
{
public:
	void Play(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		if (AmIIn(Urus))
		{
			for (int i = 0; i < 3; i++)
			{
				UrusRaid::Play(CharacterInfo, i);
			}
		}
		else
		{
			throw AppException("NotInUrus");
		}
	}
};
#define INVALID_MAP_EXCEPTION(NumberOfMap) "InvalidMapException(MapName = " + MapFileName[NumberOfMap] + ")" 
class DailyBossPlay : private ZacumRaid, private RootAbyssRaid, private Bridge
{
public:
	void Play(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		switch (CharacterInfo.Type)
		{
		case USERCONF::CHARACTER_INFO::ZACUM:
			switch (int CurrentMap = WhereAmI())
			{
			case Urus:
				MoveFromAToB(Urus, Elnas);
				MoveFromAToB(Elnas, ElnasOffice);
				MoveFromAToB(ElnasOffice, Zacum1);
				MoveFromAToB(Zacum1, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum3_2);
				break;

			case Zacum2_2:
			case Zacum2_3:
				MoveFromAToB(CurrentMap, Zacum1);
				MoveFromAToB(Zacum1, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum3_2);
				break;
			default:
				throw AppException(INVALID_MAP_EXCEPTION(CurrentMap));
			}
			ZacumRaid::Play(CharacterInfo);
			break;

		case USERCONF::CHARACTER_INFO::ZACUM_ROOTABYSS:
			switch (int CurrentMap = WhereAmI())
			{
			case Zacum2_2:
			case Zacum2_3:
				MoveFromAToB(CurrentMap, Zacum1);
				MoveFromAToB(Zacum1, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum3_2);
				ZacumRaid::Play(CharacterInfo);
				MoveFromAToB(Zacum3_2, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum1);
				MoveFromAToB(Zacum1, Elnas);
				MoveFromAToB(Elnas, RootAbyss1);
				MoveFromAToB(RootAbyss1, RootAbyss2_1);
				MoveFromAToB(RootAbyss2_1, RootAbyss3_1);
				RootAbyssRaid::Play(CharacterInfo);
				break;

			case Urus:
				MoveFromAToB(Urus, Elnas);
				MoveFromAToB(Elnas, RootAbyss1);
				MoveFromAToB(RootAbyss1, RootAbyss2_1);
				MoveFromAToB(RootAbyss2_1, RootAbyss3_1);
				RootAbyssRaid::Play(CharacterInfo);
				MoveFromAToB(RootAbyss3_1, RootAbyss1);
				MoveFromAToB(RootAbyss1, Elnas);
				MoveFromAToB(Elnas, ElnasOffice);
				MoveFromAToB(ElnasOffice, Zacum1);
				MoveFromAToB(Zacum1, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum3_2);
				ZacumRaid::Play(CharacterInfo);
				MoveFromAToB(Zacum3_2, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum1);
				MoveFromAToB(Zacum1, Elnas);
				MoveFromAToB(Elnas, Urus);
				break;

			case RootAbyss1:
				MoveFromAToB(RootAbyss1, RootAbyss2_1);
				MoveFromAToB(RootAbyss2_1, RootAbyss3_1);
				RootAbyssRaid::Play(CharacterInfo);
				MoveFromAToB(RootAbyss3_1, RootAbyss1);
				MoveFromAToB(RootAbyss1, Elnas);
				MoveFromAToB(Elnas, ElnasOffice);
				MoveFromAToB(ElnasOffice, Zacum1);
				MoveFromAToB(Zacum1, Zacum2_2);
				MoveFromAToB(Zacum2_2, Zacum3_2);
				ZacumRaid::Play(CharacterInfo);
				break;

			default:
				throw AppException(INVALID_MAP_EXCEPTION(CurrentMap));
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