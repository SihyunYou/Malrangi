#pragma once
#include "low_util.h"
#include <map>

/****************************************************************************
* Configuration Reader
****************************************************************************/
typedef struct _CONF_INFO
{
	typedef struct _ACCOUNT_INFO
	{
		string Id;
		string Password;
		typedef struct _MAPLEID_INFO
		{
			string Id;
			string SecondPassword;
			typedef struct _CHARACTER_INFO
			{
				BYTE Skill;
				BYTE RequiredBuf1;
				BYTE RequiredBuf2;
			} CHARACTER_INFO;
			vector<string> VecCharacterString;
		}MAPLEID_INFO;
		vector<MAPLEID_INFO> VecMapleId;
	} ACCOUNT_INFO;
	vector<ACCOUNT_INFO> VecAccount;
	map<string, ACCOUNT_INFO::MAPLEID_INFO::CHARACTER_INFO> MapCharacterInfo;

	typedef struct _KEYSET_INFO
	{
		BYTE SpecialTechnology;
		BYTE Picking;
		BYTE Inventory;
		BYTE Potion;
	}KEYSET_INFO;
	KEYSET_INFO VirtualKeyset;

private:
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

				if (!strncmp("var", Line, 3))
				{
					string Lvalue = string(strchr(Line, ' ') + 1, strchr(Line, '=') - 1);
					char* pdq = strchr(Line, '\"');
					string Rvalue = string(pdq + 1, strchr(pdq + 1, '\"'));
					ACCOUNT_INFO::MAPLEID_INFO::CHARACTER_INFO CharacterInfo;
					Stream = istringstream(Rvalue);

					Stream >> SubLine;
					CharacterInfo.Skill = SubLine[0];

					Stream >> SubLine;
					CharacterInfo.RequiredBuf1 = (SubLine == "null" ? NULL : SubLine[0]);

					Stream >> SubLine;
					CharacterInfo.RequiredBuf2 = (SubLine == "null" ? NULL : SubLine[0]);
					
					MapCharacterInfo[Lvalue] = CharacterInfo;
				}
				if (!strncmp("\t\t", Line, 2))
				{
					Stream >> SubLine;
					VecAccount[VecAccount.size() - 1].VecMapleId[VecAccount[VecAccount.size() - 1].VecMapleId.size() - 1].VecCharacterString.push_back(SubLine);
				}
				else if ('\t' == Line[0])
				{
					ACCOUNT_INFO::MAPLEID_INFO MapleIdInfo;

					Stream >> SubLine;
					MapleIdInfo.Id = SubLine;

					Stream >> SubLine;
					MapleIdInfo.SecondPassword = SubLine;

					VecAccount[VecAccount.size() - 1].VecMapleId.push_back(MapleIdInfo);
				}
				else
				{
					ACCOUNT_INFO AccountInfo;

					Stream >> SubLine;
					AccountInfo.Id = SubLine;

					Stream >> SubLine;
					AccountInfo.Password = SubLine;

					VecAccount.push_back(AccountInfo);
				}
			}
			File.close();
		}

		File.open("conf\\key_setting_info.conf");
		if (File.is_open())
		{
			File.getline(Line, sizeof(Line));

			istringstream Stream(Line);
			string SubLine;
			auto GetVirtualKey = [](string Key) -> BYTE
			{
				if ("space" == Key) return VK_SPACE;
				else if ("control" == Key) return VK_CONTROL;
				else if ("shift" == Key) return VK_SHIFT;
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
	~_CONF_INFO()
	{
		;
	}

public:
	static _CONF_INFO* GetInstance(void)
	{
		if (nullptr == Instance)
		{
			Instance = new _CONF_INFO();
		}
		return Instance;
	}
	void Destroy(void)
	{
		if (nullptr != Instance)
		{
			delete Instance;
			Instance = nullptr;
		}
	}
	private:
		static _CONF_INFO* Instance;
}CONF_INFO;
CONF_INFO* CONF_INFO::Instance = nullptr;