#pragma once
#include <map>

/****************************************************************************
* Configuration Reader
****************************************************************************/
class USERCONF
{
public:
	typedef struct _NEXONAC_INFO
	{
		string Id;
		string Password;
		struct _MAPLEID_INFO
		{
			string Id;
			string SecondPassword;
			struct _SERVER_INFO
			{
				string ServerName;
				struct _CHARACTER_INFO
				{
					string ClassName;
					enum
					{
						ZACUM,
						ZACUM_ROOTABYSS
					};
					int Type;
				};
				vector<struct _CHARACTER_INFO> VecCharacter;
			};
			vector<struct _SERVER_INFO> VecServer;
		};
		vector<struct _MAPLEID_INFO> VecMapleId;
	} NEXONAC_INFO;
	typedef struct _KEYSET_INFO
	{
		BYTE SpecialTechnology;
		BYTE Picking;
		BYTE Inventory;
		BYTE Potion;
		BYTE Party;
		BYTE Attack;
	}KEYSET_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO MAPLEID_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO SERVER_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO CHARACTER_INFO;
	vector<NEXONAC_INFO> VecNexonAccount;
	KEYSET_INFO VirtualKeyset;

private:
	USERCONF(void)
	{
		ifstream File;
		File.open(USERCONF_PATH);

		if (File.is_open())
		{
			CHAR Line[0x100];
			while (File.getline(Line, sizeof(Line)))
			{
				if (strlen(Line) == 0)
				{
					continue;
				}
				istringstream Stream(Line);
				string SubLine;

				if (!strncmp("\t\t\t", Line, 3))
				{
					CHARACTER_INFO CharacterInfo;

					Stream >> SubLine;
					if (SubLine[SubLine.length() - 1] == '*')
					{
						CharacterInfo.ClassName = SubLine.substr(0, SubLine.length() - 1);
						CharacterInfo.Type = CHARACTER_INFO::ZACUM_ROOTABYSS;
					}
					else
					{
						CharacterInfo.ClassName = SubLine;
						CharacterInfo.Type = CHARACTER_INFO::ZACUM;
					}

					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						VecCharacter.push_back(CharacterInfo);
				}
				else if (!strncmp("\t\t", Line, 2))
				{
					SERVER_INFO ServerInfo;

					Stream >> SubLine;
					ServerInfo.ServerName = SubLine;

					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer.push_back(ServerInfo);
				}
				else if ('\t' == Line[0])
				{
					MAPLEID_INFO MapleIdInfo;

					Stream >> SubLine;
					MapleIdInfo.Id = SubLine;

					Stream >> SubLine;
					MapleIdInfo.SecondPassword = SubLine;

					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.push_back(MapleIdInfo);
				}
				else
				{
					NEXONAC_INFO NexonAccountInfo;

					Stream >> SubLine;
					NexonAccountInfo.Id = SubLine;

					Stream >> SubLine;
					NexonAccountInfo.Password = SubLine;

					VecNexonAccount.push_back(NexonAccountInfo);
				}
			}
			File.close();

			VirtualKeyset.Inventory = 'I';
			VirtualKeyset.Party = VK_OEM_4;
			VirtualKeyset.Picking = VK_SPACE;
			VirtualKeyset.Potion = VK_CONTROL;
			VirtualKeyset.SpecialTechnology = 'Y';
			VirtualKeyset.Attack = VK_OEM_PERIOD;
		}
	}
	~USERCONF()
	{
		;
	}


public:
	static USERCONF* GetInstance(void)
	{
		if (nullptr == Instance)
		{
			Instance = new USERCONF();
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
	static USERCONF* Instance;
};
USERCONF* USERCONF::Instance = nullptr;