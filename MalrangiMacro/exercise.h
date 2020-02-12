/****************************************************************************
*
* exercise.h -- Macro Applications Optimized to Maple
*
****************************************************************************/

#pragma once
#include "cvwrap.h"

typedef struct _CLIENT_ELEM
{
#define HWND_MAPLESTORY (::FindWindow(NULL, TEXT("MapleStory")))
	static void SetClientStandardPosition(void);
	static CONST RECT RECT_CLIENT1;
	static CONST RECT RECT_CLIENT4;
	static CONST Mat TARGETIMAGE_E1;
	static CONST Mat TARGETIMAGE_E2;
	static CONST Mat TARGETIMAGE_E3;
	static CONST Mat TARGETIMAGE_I1;
	static CONST RECT RECT_E1;
	static CONST RECT RECT_E2;
	static CONST RECT RECT_E3;
	static CONST RECT RECT_I1;
}CLIENT_ELEM;
void CLIENT_ELEM::SetClientStandardPosition(void)
{
	SetWindowPos(GetConsoleWindow(),
		0,
		1362, 0,
		0, 0,
		SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(HWND_MAPLESTORY,
		HWND_TOP,
		-3, -26,
		0, 0,
		SWP_NOSIZE);
}
CONST RECT CLIENT_ELEM::RECT_CLIENT1 = { 0, 0, 800, 600 };
CONST RECT CLIENT_ELEM::RECT_CLIENT4 = { 0, 0, 1366, 768 };
CONST Mat CLIENT_ELEM::TARGETIMAGE_E1 = Cvw::Read("res\\initial_logo.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_E2 = Cvw::Read("res\\world-channel_select.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_E3 = Cvw::Read("res\\character_select.jpg");
CONST Mat CLIENT_ELEM::TARGETIMAGE_I1 = Cvw::Read("res\\button_mileage.jpg");
CONST RECT CLIENT_ELEM::RECT_E1 = { 700, 0, 850, 100 };
CONST RECT CLIENT_ELEM::RECT_E2 = { 30, 0, 190, 50 };
CONST RECT CLIENT_ELEM::RECT_E3 = { 0, 0, 200, 70 };
CONST RECT CLIENT_ELEM::RECT_I1 = { 0, 280, 100, 500 };


typedef struct _CONF_INFO
{
	typedef struct _ACCOUNT_INFO
	{
		enum class ID_T
		{
			NEXON,
			MAPLE,
			NAVER
		};
		ID_T IdType;
		string Id;
		string Password;
		string SecondPassword;
		DWORD Sequence;
		DWORD MainServer;
		typedef struct _CHARACTER_SPECIALTY
		{
			BYTE Skill;
			BYTE RequiredBuf1;
			BYTE RequiredBuf2; // used on Zacum's being created
		} CHARACTER_SPECIALTY;
		vector<CHARACTER_SPECIALTY> VecCharacter;
	} ACCOUNT_INFO;
	array<ACCOUNT_INFO, 20> ArrAccount;
	INT CountAccount = 0;
	INT CountAllCharacters = 0;

	typedef struct _KEYSET_INFO
	{
		BYTE SpecialTechnology;
		BYTE Picking;
		BYTE Inventory;
		BYTE Potion;
	}KEYSET_INFO;
	KEYSET_INFO VirtualKeyset;

	_CONF_INFO(void)
	{
		ifstream File;
		CHAR Line[0x100];

		File.open("conf\\account_info.conf");
		if (File.is_open())
		{
			while (File.getline(Line, sizeof(Line)))
			{
				if (strlen(Line) == 0)
				{
					continue;
				}

				istringstream Stream(Line);
				string SubLine;
				_CONF_INFO::ACCOUNT_INFO::CHARACTER_SPECIALTY CharacterInfo;

				Stream >> SubLine;
				if ("//" == SubLine.substr(0, 2))
				{
					continue;
				}
				else if ("ACL" == SubLine)
				{
					Stream >> SubLine;
					if ("NEXON" == SubLine)
					{
						ArrAccount[CountAccount].IdType = _CONF_INFO::ACCOUNT_INFO::ID_T::NEXON;
					}
					else if ("NAVER" == SubLine)
					{
						ArrAccount[CountAccount].IdType = _CONF_INFO::ACCOUNT_INFO::ID_T::NAVER;
					}
					else 
					{
						ArrAccount[CountAccount].IdType = _CONF_INFO::ACCOUNT_INFO::ID_T::MAPLE;
					}

					Stream >> SubLine;
					ArrAccount[CountAccount].Id = SubLine;

					Stream >> SubLine;
					ArrAccount[CountAccount].Password = SubLine;

					Stream >> SubLine;
					ArrAccount[CountAccount].SecondPassword = SubLine;

					Stream >> SubLine;
					stringstream(SubLine) >> ArrAccount[CountAccount].Sequence;

					++CountAccount;
				}
				else
				{
					CharacterInfo.Skill = SubLine[0];
					cout << CharacterInfo.Skill << endl;
					Stream >> SubLine;
					CharacterInfo.RequiredBuf1 = (SubLine == "null" ? NULL : SubLine[0]);
					
					Stream >> SubLine;
					CharacterInfo.RequiredBuf2 = (SubLine == "null" ? NULL : SubLine[0]);

					ArrAccount[CountAccount - 1].VecCharacter.push_back(CharacterInfo);
				}
			}
			File.close();
		}

		_CONF_INFO::ACCOUNT_INFO tmp;
		for (int i = 0; i < CountAccount; i++)
		{
			for (int j = i + 1; j < CountAccount; j++)
			{
				if (ArrAccount[i].Id == ArrAccount[j].Id)
				{
					if (ArrAccount[i].Sequence > ArrAccount[j].Sequence)
					{
						tmp = ArrAccount[i];
						ArrAccount[i] = ArrAccount[j];
						ArrAccount[j] = tmp;
					}
				}
			}
		}

		for each (auto & AccountInfo in ArrAccount)
		{
			CountAllCharacters += AccountInfo.VecCharacter.size();
		}


		File.open("conf\\key_setting_info.conf");
		if (File.is_open())
		{
			File.getline(Line, sizeof(Line));

			istringstream Stream(Line);
			string SubLine;
			auto GetVirtualKey = [](string Key) -> BYTE
			{
				if ("SPACE" == Key) return VK_SPACE;
				else if ("CONTROL" == Key) return VK_CONTROL;
				else if ("SHIFT" == Key) return VK_SHIFT;
				else return Key[0];
			};

			Stream >> SubLine;
			VirtualKeyset.Inventory = GetVirtualKey(SubLine);

			Stream >> SubLine;
			VirtualKeyset.SpecialTechnology = GetVirtualKey(SubLine);

			Stream >> SubLine;
			VirtualKeyset.Picking = GetVirtualKey(SubLine);

			Stream >> SubLine;
			VirtualKeyset.Potion = GetVirtualKey(SubLine);

			File.close();
		}
	}
}CONF_INFO;
static CONST CONF_INFO Conf;








