#pragma once
#include "malrangi.h"

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
			unsigned int Adresse;
			bool EstAdressePremière;
			bool EstAdresseDernière;
#ifdef BUILD_URUS
			string ServeurPourUrus;
#endif
			struct _SERVER_INFO
			{
				string NomDeServeur;
#ifdef BUILD_URUS
				bool EstAccompli;
#endif
				unsigned int NombreDeCharacter;

				_SERVER_INFO() : 
#ifdef BUILD_URUS
					EstAccompli(false), 
#endif
					NombreDeCharacter(0) {}

#ifndef BUILD_URUS
				int IndexDeCaractèreDisponible;
#endif
				struct _CHARACTER_INFO
				{
					enum _CODE_PROPRE
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
						데벤2,
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
						키네,
						MAXIMUM
					};
					string NomDeClasse;
					_CODE_PROPRE Code;
					unsigned int Vitesse;
					bool Stance;

					enum
					{
						ZACUM = 0b0001,
						BELLUM = 0b0010,
						VONBAN = 0b0100,
						BLOODYQUEEN = 0b1000,
						VONLEON_EASY = 0b00010000,
						VONLEON_NORMAL = 0b00100000,
						VONLEON_HARD = 0b01000000,
						PAPULATUS_EASY = 0b10000000,
						PAPULATUS_NORMAL = 0b000100000000,
						KAUNG = 0b001000000000,
						COMPTE = 0b100000000000,
						FLAG_MAXIMUM = COMPTE,
					};
					int FlagInvariable;
					int Flag;
#ifndef BUILD_URUS
					bool EstAccompli;
#endif
					_CHARACTER_INFO() 
#ifndef BUILD_URUS
						: EstAccompli(false) 
#endif
					{}
				};
				vector<_CHARACTER_INFO> VecCharacter;
			};
			vector<_SERVER_INFO> VecServer;

			_MAPLEID_INFO() : EstAdressePremière(false), EstAdresseDernière(false) {}
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
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO CHARACTER_INFO;
	typedef NEXONAC_INFO::_MAPLEID_INFO::_SERVER_INFO::_CHARACTER_INFO::_CODE_PROPRE CODE_PROPRE;
	map<string, tuple<CODE_PROPRE, unsigned int, bool>> MapCharacter;
	vector<NEXONAC_INFO> VecNexonAccount;
	KEYSET_INFO VirtualKeyset;


private:
	USERCONF(void)
	{
		ifstream File;
		File.open(USERCONF_PATH);

		if (File.is_open())
		{
			int ProfitEstimé = 0;
			MapCharacter =
			{
	#define FAIRE_MAP(_n, _v, _s) {#_n, tuple{CODE_PROPRE::_n, _v, _s}}
				FAIRE_MAP(히어로, 140, true),
				FAIRE_MAP(팔라, 140, true),
				FAIRE_MAP(닼나, 150, true),
				FAIRE_MAP(불독, 140, false),
				FAIRE_MAP(썬콜, 140, false),
				FAIRE_MAP(비숍, 128, false),
				FAIRE_MAP(보마, 160, false),
				FAIRE_MAP(신궁, 160, false),
				FAIRE_MAP(패파, 160, false),
				FAIRE_MAP(나로, 138, false),
				FAIRE_MAP(섀도어, 140, false),
				FAIRE_MAP(듀블, 138, false),
				FAIRE_MAP(바이퍼, 160, false),
				FAIRE_MAP(캡틴, 140, false),
				FAIRE_MAP(캐슈, 140, false),
				FAIRE_MAP(소마, 160, false),
				FAIRE_MAP(미하일, 140, true),
				FAIRE_MAP(플위, 140, false),
				FAIRE_MAP(윈브, 160, false),
				FAIRE_MAP(나워, 137, false),
				FAIRE_MAP(스커, 160, false),
				FAIRE_MAP(아란, 160, true),
				FAIRE_MAP(에반, 140, false),
				FAIRE_MAP(루미, 140, false),
				FAIRE_MAP(메세, 165, false),
				FAIRE_MAP(팬텀, 160, false),
				FAIRE_MAP(은월, 140, false),
				FAIRE_MAP(데슬, 140, true),
				FAIRE_MAP(데벤, 160, true),
				FAIRE_MAP(데벤2, 160, true),
				FAIRE_MAP(블래, 160, true),
				FAIRE_MAP(배메, 170, true),
				FAIRE_MAP(와헌, 170, false),
				FAIRE_MAP(제논, 160, false),
				FAIRE_MAP(메카닉, 140, false),
				FAIRE_MAP(카이저, 140, false),
				FAIRE_MAP(엔버, 165, false),
				FAIRE_MAP(카데나, 125, false),
				FAIRE_MAP(아델, 134, false),
				FAIRE_MAP(일리움, 128, false),
				FAIRE_MAP(아크, 165, true),
				FAIRE_MAP(호영, 160, false),
				FAIRE_MAP(제로, 159, true),
				FAIRE_MAP(키네, 160, false)
	#undef FAIRE_MAP
			};
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
					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						NombreDeCharacter += 1;
					CHARACTER_INFO CharacterInfo;
				
					Stream >> SubLine;
					CharacterInfo.NomDeClasse = SubLine;
					CharacterInfo.Code = get<0>(MapCharacter[SubLine]);
					CharacterInfo.Vitesse = get<1>(MapCharacter[SubLine]);
					CharacterInfo.Stance = get<2>(MapCharacter[SubLine]);

					Stream >> SubLine;
					SubLine.erase(std::remove(SubLine.begin(), SubLine.end(), '-'), SubLine.end());
					CharacterInfo.FlagInvariable = CharacterInfo.Flag = stoi(SubLine, nullptr, 2);

#ifdef BUILD_BOSS
					for (int p = 0; p < log2((long)CHARACTER_INFO::FLAG_MAXIMUM) + 1; p++)
					{
						switch (CharacterInfo.Flag & (1 << p))
						{
						case CHARACTER_INFO::ZACUM:
							ProfitEstimé += 100;
							break;
						case CHARACTER_INFO::BLOODYQUEEN:
						case CHARACTER_INFO::VONBAN:
						case CHARACTER_INFO::BELLUM:
							ProfitEstimé += 120;
							break;
						case CHARACTER_INFO::VONLEON_EASY:
							ProfitEstimé += 115;
							break;
						case CHARACTER_INFO::VONLEON_NORMAL:
							ProfitEstimé += 160;
							break;
						case CHARACTER_INFO::VONLEON_HARD:
							ProfitEstimé += 260;
							break;
						case CHARACTER_INFO::KAUNG:
							ProfitEstimé += 170;
							break;
						case CHARACTER_INFO::PAPULATUS_EASY:
							ProfitEstimé += 70;
							break;
						case CHARACTER_INFO::PAPULATUS_NORMAL:
							ProfitEstimé += 270;
							break;
						default:
							break;
						}
					}
#endif
					VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].
						VecServer[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId[VecNexonAccount[VecNexonAccount.size() - 1].VecMapleId.size() - 1].VecServer.size() - 1].
						VecCharacter.push_back(CharacterInfo);
				}
				else if (!strncmp("\t\t", Line, 2))
				{
					SERVER_INFO ServerInfo;

					Stream >> SubLine;
					ServerInfo.NomDeServeur = SubLine;
#ifndef BUILD_URUS
					Stream >> SubLine;
					ServerInfo.IndexDeCaractèreDisponible = stoi(SubLine);
#endif
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
					Stream >> SubLine;
					MapleIdInfo.Adresse = atoi(SubLine.c_str());
#ifdef BUILD_URUS
					Stream >> SubLine;
					MapleIdInfo.ServeurPourUrus = SubLine;
#endif
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

			bool EstUnieme = true;
			MAPLEID_INFO* MapleIdAvant = nullptr;
			for (auto& NexonAccountInfo : VecNexonAccount)
			{
				for (auto& MapleIdInfo : NexonAccountInfo.VecMapleId)
				{
					if (EstUnieme)
					{
						MapleIdInfo.EstAdressePremière = true;
						EstUnieme = false;
					}
					else if (MapleIdInfo.Adresse != MapleIdAvant->Adresse)
					{
						MapleIdInfo.EstAdressePremière = true;
						MapleIdAvant->EstAdresseDernière = true;
					}

					MapleIdAvant = &MapleIdInfo;
				}
			}

			VirtualKeyset.Inventory = 'I';
			VirtualKeyset.Party = VK_OEM_4;
			VirtualKeyset.Picking = VK_SPACE;
			VirtualKeyset.Potion = VK_CONTROL;
			VirtualKeyset.SpecialTechnology = 'Y';
			VirtualKeyset.Attack = VK_OEM_PERIOD;
			VirtualKeyset.Talk = VK_OEM_COMMA;

#ifdef BUILD_BOSS
			cout << "예상수익 : " << (double)ProfitEstimé / 10000 << endl;
#endif

#ifndef BUILD_URUS
			for (auto& NexonAccountInfo : VecNexonAccount)
			{
				for (auto& MapleIdInfo : NexonAccountInfo.VecMapleId)
				{
					for (auto& ServerInfo : MapleIdInfo.VecServer)
					{
						for (int p = 0; p < ServerInfo.IndexDeCaractèreDisponible - 1; p++)
						{
							ServerInfo.VecCharacter[p].EstAccompli = true;
						}
					}
				}
			}
#endif
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