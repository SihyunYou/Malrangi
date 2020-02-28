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
				POINT CoorServer;
				struct _CHARACTER_INFO
				{
					string CharacterType;
					BYTE Skill;
					BYTE RequiredBuf1;
					BYTE RequiredBuf2;
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
	}KEYSET_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO MAPLEID_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO SERVER_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO CHARACTER_INFO;
	
	vector<NEXONAC_INFO> VecNexonAccount;
	map<string, SERVER_INFO> MapServerInfo;
	map<string, CHARACTER_INFO> MapCharacterInfo;
	KEYSET_INFO VirtualKeyset;

private:
	USERCONF(void)
	{
		ifstream File;
		File.open(USERCONF_PATH);

		if (File.is_open())
		{
			CHAR Line[0x100];
			const string ServerNames[] = { "스카니아", "베라", "루나", "제니스", "크로아", "유니온", "엘리시움", "이노시스", "레드", "오로라", "아케인", "노바" };
			for (int i = 0; i < sizeof(ServerNames) / sizeof(string); i++)
			{
				MapServerInfo[ServerNames[i]].ServerName = ServerNames[i];
				MapServerInfo[ServerNames[i]].CoorServer = { 745, 68 + i * 32 };
			}

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
				
					Stream = istringstream(Rvalue);
					CHARACTER_INFO CharacterInfo;
					CharacterInfo.CharacterType = Lvalue;

					Stream >> SubLine;
					CharacterInfo.Skill = SubLine[0];

					Stream >> SubLine;
					CharacterInfo.RequiredBuf1 = (SubLine == "null" ? NULL : SubLine[0]);

					Stream >> SubLine;
					CharacterInfo.RequiredBuf2 = (SubLine == "null" ? NULL : SubLine[0]);
					
					MapCharacterInfo[Lvalue] = CharacterInfo;
				}
				else if (!strncmp("vk.", Line, 3))
				{
					auto GetVirtualKey = [](string Key) -> BYTE
					{
						if ("space" == Key) return VK_SPACE;
						else if ("control" == Key) return VK_CONTROL;
						else if ("shift" == Key) return VK_SHIFT;
						else if ("[" == Key) return VK_OEM_4;
						else return Key[0];
					};
					string Lvalue = string(Line + 3, strchr(Line, '=') - 1);
					string Rvalue = string(strchr(Line, '=') + 2);

					if ("inv" == Lvalue)
					{
						VirtualKeyset.Inventory = GetVirtualKey(Rvalue);
					}
					else if ("party" == Lvalue)
					{
						VirtualKeyset.Party = GetVirtualKey(Rvalue);
					}
					else if ("pick" == Lvalue)
					{
						VirtualKeyset.Picking = GetVirtualKey(Rvalue);
					}
					else if ("potion" == Lvalue)
					{
						VirtualKeyset.Potion = GetVirtualKey(Rvalue);
					}
					else if ("tech" == Lvalue)
					{
						VirtualKeyset.SpecialTechnology = GetVirtualKey(Rvalue);
					}
				}
				else if (!strncmp("\t\t\t", Line, 3))
				{
					Stream >> SubLine;
					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						VecCharacter.push_back(MapCharacterInfo[SubLine]);
				}
				else if (!strncmp("\t\t", Line, 2))
				{
					Stream >> SubLine;
					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer.push_back(MapServerInfo[SubLine]);
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