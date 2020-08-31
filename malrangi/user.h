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
#if defined(BUILD_URUS)
				bool IsCompleted;
				unsigned int NombreDeCharacter;

				_SERVER_INFO() : IsCompleted(false), NombreDeCharacter(0) {}
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
				struct _CHARACTER_INFO
				{
					enum class _CODE_PROPRE
					{
						�����,
						�ȶ�,
						����,
						�ҵ�,
						����,
						���,
						����,
						�ű�,
						����,
						����,
						������,
						���,
						������,
						ĸƾ,
						ĳ��,
						�Ҹ�,
						������,
						����,
						����,
						����,
						��Ŀ,
						�ƶ�,
						����,
						���,
						�޼�,
						����,
						����,
						����,
						����,
						��,
						���,
						����,
						����,
						��ī��,
						ī����,
						����,
						ī����,
						�Ƶ�,
						�ϸ���,
						��ũ,
						ȣ��,
						����,
						Ű��
					};
					string ClassName;
					_CODE_PROPRE Code;
					unsigned int Speed;

					enum
					{
						ZACUM = 0b001,
						ROOTABYSS_BLOODYQUEEN = 0b010,
						ROOTABYSS_VONBAN = 0b100
					};
					int Flag;
					bool IsCompleted;

					_CHARACTER_INFO() : IsCompleted(false) {}
				};
				vector<_CHARACTER_INFO> VecCharacter;
#endif
			};
			vector<_SERVER_INFO> VecServer;
		};
		vector<_MAPLEID_INFO> VecMapleId;
	} NEXONAC_INFO;
	typedef struct _KEYSET_INFO
	{
		BYTE SpecialTechnology;
		BYTE Picking;
		BYTE Inventory;
		BYTE Potion;
		BYTE Party;
		BYTE Attack;
		BYTE Talk;
	}KEYSET_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO MAPLEID_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO SERVER_INFO;
#if defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO CHARACTER_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO::_CODE_PROPRE CODE_PROPRE;
	map<string, pair<CODE_PROPRE, unsigned int>> MapCharacter;
#endif
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
#if defined(BUILD_URUS)
					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						NombreDeCharacter += 1;
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
					CHARACTER_INFO CharacterInfo;

					MapCharacter =
					{
			#define FAIRE_MAP(_n, _s) {#_n, pair{CODE_PROPRE::_n, _s}}
						FAIRE_MAP(�����, 140),
						FAIRE_MAP(�ȶ�, 140),
						FAIRE_MAP(����, 150),
						FAIRE_MAP(�ҵ�, 140),
						FAIRE_MAP(����, 140),
						FAIRE_MAP(���, 128),
						FAIRE_MAP(����, 160),
						FAIRE_MAP(�ű�, 160),
						FAIRE_MAP(����, 143),
						FAIRE_MAP(����, 138),
						FAIRE_MAP(������, 140),
						FAIRE_MAP(���, 139),
						FAIRE_MAP(������, 160),
						FAIRE_MAP(ĸƾ, 140),
						FAIRE_MAP(ĳ��, 140),
						FAIRE_MAP(�Ҹ�, 160),
						FAIRE_MAP(������, 140),
						FAIRE_MAP(����, 140),
						FAIRE_MAP(����, 160),
						FAIRE_MAP(����, 137),
						FAIRE_MAP(��Ŀ, 160),
						FAIRE_MAP(�ƶ�, 160),
						FAIRE_MAP(����, 140),
						FAIRE_MAP(���, 140),
						FAIRE_MAP(�޼�, 165),
						FAIRE_MAP(����, 160),
						FAIRE_MAP(����, 140),
						FAIRE_MAP(����, 140),
						FAIRE_MAP(����, 160),
						FAIRE_MAP(��, 160),
						FAIRE_MAP(���, 170),
						FAIRE_MAP(����, 170),
						FAIRE_MAP(����, 160),
						FAIRE_MAP(��ī��, 140),
						FAIRE_MAP(ī����, 140),
						FAIRE_MAP(����, 165),
						FAIRE_MAP(ī����, 140),
						FAIRE_MAP(�Ƶ�, 165),
						FAIRE_MAP(�ϸ���, 128),
						FAIRE_MAP(��ũ, 165),
						FAIRE_MAP(ȣ��, 160),
						FAIRE_MAP(����, 129),
						FAIRE_MAP(Ű��, 160)
			#undef FAIRE_MAP
					};

					Stream >> SubLine;
					CharacterInfo.ClassName = SubLine;
					CharacterInfo.Code = MapCharacter[SubLine].first;
					CharacterInfo.Speed = MapCharacter[SubLine].second;

					Stream >> SubLine;
					CharacterInfo.Flag = stoi(SubLine);

					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						VecCharacter.push_back(CharacterInfo);
#endif
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
			VirtualKeyset.Talk = VK_OEM_COMMA;
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