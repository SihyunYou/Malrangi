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
						히어로,
						팔라,
						닼나,
						불독,
						썬콜,
						비숍,
						보마,
						신궁,
						패파,
						나로,
						섀도어,
						듀블,
						바이퍼,
						캡틴,
						캐슈,
						소마,
						미하일,
						플위,
						윈브,
						나워,
						스커,
						아란,
						에반,
						루미,
						메세,
						팬텀,
						은월,
						데슬,
						데벤,
						블래,
						배메,
						와헌,
						제논,
						메카닉,
						카이저,
						엔버,
						카데나,
						아델,
						일리움,
						아크,
						호영,
						제로,
						키네
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
						FAIRE_MAP(히어로, 140),
						FAIRE_MAP(팔라, 140),
						FAIRE_MAP(닼나, 150),
						FAIRE_MAP(불독, 140),
						FAIRE_MAP(썬콜, 140),
						FAIRE_MAP(비숍, 128),
						FAIRE_MAP(보마, 160),
						FAIRE_MAP(신궁, 160),
						FAIRE_MAP(패파, 143),
						FAIRE_MAP(나로, 138),
						FAIRE_MAP(섀도어, 140),
						FAIRE_MAP(듀블, 139),
						FAIRE_MAP(바이퍼, 160),
						FAIRE_MAP(캡틴, 140),
						FAIRE_MAP(캐슈, 140),
						FAIRE_MAP(소마, 160),
						FAIRE_MAP(미하일, 140),
						FAIRE_MAP(플위, 140),
						FAIRE_MAP(윈브, 160),
						FAIRE_MAP(나워, 137),
						FAIRE_MAP(스커, 160),
						FAIRE_MAP(아란, 160),
						FAIRE_MAP(에반, 140),
						FAIRE_MAP(루미, 140),
						FAIRE_MAP(메세, 165),
						FAIRE_MAP(팬텀, 160),
						FAIRE_MAP(은월, 140),
						FAIRE_MAP(데슬, 140),
						FAIRE_MAP(데벤, 160),
						FAIRE_MAP(블래, 160),
						FAIRE_MAP(배메, 170),
						FAIRE_MAP(와헌, 170),
						FAIRE_MAP(제논, 160),
						FAIRE_MAP(메카닉, 140),
						FAIRE_MAP(카이저, 140),
						FAIRE_MAP(엔버, 165),
						FAIRE_MAP(카데나, 140),
						FAIRE_MAP(아델, 165),
						FAIRE_MAP(일리움, 128),
						FAIRE_MAP(아크, 165),
						FAIRE_MAP(호영, 160),
						FAIRE_MAP(제로, 129),
						FAIRE_MAP(키네, 160)
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