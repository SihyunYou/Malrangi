#pragma once
#include "map.h"

class AppException : public MalrangiException
{
public:
	bool IsRetryAvailable;
	enum 
	{
		CHANGER_SERVEUR,
		SORTIR_GAME
	};
	int OptionDeRéparation;
	AppException(string Description, bool IsRetryAvailable, int OptionDeRéparation = SORTIR_GAME) :
		MalrangiException(__CLASSNAME__ + "! " + Description),
		IsRetryAvailable(IsRetryAvailable) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};

#ifdef BUILD_URUS
#include "urus.h"

class UrusPlay : private UrusRaid, private Pont
{
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo,
		const bool IsQuickCompletionMode)
	{
		int MapPrésent;
		auto Traverser = [this, &CharacterInfo, &MapPrésent](int A, int B)
		{
			BougerDeAÀB(A, B, CharacterInfo);
			return MapPrésent = B;
		};

		try
		{
			switch (MapPrésent = WhereAmI())
			{
			case Unknown:
				throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

			default:
			{
				switch (MapPrésent)
				{
				case FreeMarket:
					Traverser(FreeMarket, ElnasMarket);
					break;

				case Zacum2_2:
					Traverser(Zacum2_2, Zacum1);
					break;

				case VonLeon2:
					Traverser(VonLeon2, VonLeon1);
					break;
				}
			}
			case ElnasMarket:
			case Zacum1:
			case RootAbyss1:
			case VonLeon1:
				Traverser(MapPrésent, Elnas);
			case Elnas:
				Traverser(Elnas, Urus);
			case Urus:
				break;
			}

			for (; SeqPlay <= 3; SeqPlay++)
			{
				UrusRaid::Play(CharacterInfo, IsQuickCompletionMode);
			}
		}
		catch (tuple<PONT_EXCEPTION_CODE, int, int> CollectivitéDeCode)
		{
			string DescriptionDeCode;
			switch (get<0>(CollectivitéDeCode))
			{
#define CERR(_c) case PONT_EXCEPTION_CODE::_c: \
				DescriptionDeCode = #_c; \
				break;

				CERR(A_INVALID);
				CERR(B_MANQUÉ);
				CERR(B_INVALID);
				CERR(INPUT_ERRONÉ);
				CERR(B_INACCESSIBLE);
#undef CERR
			}

			bool IsRetryAvailable;
			switch (get<0>(CollectivitéDeCode))
			{
			case PONT_EXCEPTION_CODE::B_MANQUÉ:
				IsRetryAvailable = true;
				break;
			default:
				IsRetryAvailable = false;
			}

			throw AppException(
				MapFileName[get<1>(CollectivitéDeCode)] + "에서 " +
				MapFileName[get<2>(CollectivitéDeCode)] + "로 이동 중 실패 : " +
				DescriptionDeCode,
				IsRetryAvailable);
		}
		catch (PONT_EXCEPTION_CODE)
		{
			throw AppException("예외코드가 tuple 형식으로 throw되지 않음", false);
		}
		catch (URUSRAID_EXCEPTION_CODE& ExceptionCode)
		{
			switch (ExceptionCode)
			{
			case URUSRAID_EXCEPTION_CODE::BUTTON_READY_NOT_FOUND:
			case URUSRAID_EXCEPTION_CODE::BATTLE_ENTRY_TIMEOUT:
			case URUSRAID_EXCEPTION_CODE::BATTLE_TIMEOUT:
			case URUSRAID_EXCEPTION_CODE::EXIT_TO_U3_FAILED:
			case URUSRAID_EXCEPTION_CODE::NPC_MASHUR_NOT_FOUND:
				++SeqPlay;
				break;

			case URUSRAID_EXCEPTION_CODE::PARTY_UNBREAKABLE:
			case URUSRAID_EXCEPTION_CODE::PLAY_SEQUENCE_OVER:
			default:
				SeqPlay = 3;
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
#endif
#if defined(BUILD_BOSS)
#include "boss.h"
#include "compte.h"

class OperationBossJournal :
	private Boss, private Compte, private Pont
{
public:
	OperationBossJournal() :
		CountIndisponible(0) {}

	[[noreturn]] virtual void Transplanter(const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		throw MalrangiException("사용하지 않는 기본 가상함수 Transplanter가 호출됨");
	}
	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo,
		USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		int A, B, MapPrésent;
		auto Traverser = [this, &CharacterInfo, &A, &B, &MapPrésent](int _A, int _B)
		{
			BougerDeAÀB(A = _A, B = _B, CharacterInfo);
			return MapPrésent = B;
		};
		auto ArriverÀElnas = [Traverser](int& Départ)
		{
			switch (Départ)
			{
			case Urus:
				Traverser(Urus, Elnas);
				break;
			case FreeMarket:
				Traverser(FreeMarket, ElnasMarket);
			case ElnasMarket:
				Traverser(ElnasMarket, Elnas);
				break;

			case Zacum3_2:
				Traverser(Zacum3_2, Zacum2_2);
			case Zacum2_2:
				Traverser(Zacum2_2, Zacum1);
			case Zacum1:
				Traverser(Zacum1, Elnas);
				break;

			case RootAbyss3_1:
			case RootAbyss3_3:
			case RootAbyss3_4:
				Traverser(Départ, RootAbyss1);
			case RootAbyss1:
				Traverser(RootAbyss1, Elnas);
				break;

			case VonLeon3_1:
			case VonLeon3_2:
			case VonLeon3_3:
			case VonLeon2:
				Traverser(Départ, VonLeon1);
			case VonLeon1:
				Traverser(VonLeon1, Elnas);
				break;

			case Kaung2:
				Traverser(Kaung2, Kaung1);
			case Kaung1:
				Traverser(Kaung1, Elnas);
				break;

			case Papulatus2_1:
			case Papulatus2_2:
				Traverser(Départ, Papulatus1);
			case Papulatus1:
				Traverser(Papulatus1, Elnas);
				break;
			}

			Départ = Elnas;
		};

		ZacumRaid::Transplanter(CharacterInfo);
		RootAbyssBloodyQueenRaid::Transplanter(CharacterInfo);
		RootAbyssVonBanRaid::Transplanter(CharacterInfo);
		BellumRaid::Transplanter(CharacterInfo);
		VonLeonRaid::Transplanter(CharacterInfo);
		KaungRaid::Transplanter(CharacterInfo);
		PapulatusRaid::Transplanter(CharacterInfo);

		int p = 0, q;
		int FlagSeul[] =
		{
			USERCONF::CHARACTER_INFO::BELLUM,
			USERCONF::CHARACTER_INFO::BLOODYQUEEN,
			USERCONF::CHARACTER_INFO::VONBAN,
			USERCONF::CHARACTER_INFO::VONLEON_HARD,
			USERCONF::CHARACTER_INFO::VONLEON_NORMAL,
			USERCONF::CHARACTER_INFO::VONLEON_EASY,
			USERCONF::CHARACTER_INFO::KAUNG,
			USERCONF::CHARACTER_INFO::PAPULATUS_NORMAL,
			USERCONF::CHARACTER_INFO::PAPULATUS_EASY,
			USERCONF::CHARACTER_INFO::ZACUM,
		};

		string NomDeBoss;

		try
		{
			for (; p < sizeof(FlagSeul) / sizeof(p); p++)
			{
				if (!IS_FLAG_ON(CharacterInfo.Flag, FlagSeul[p]))
				{
					continue;
				}

				switch (FlagSeul[p])
				{
				case USERCONF::CHARACTER_INFO::ZACUM:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
					{
						switch (MapPrésent)
						{
						case FreeMarket:
							Traverser(FreeMarket, ElnasMarket);
							break;

						case RootAbyss3_3:
						case RootAbyss3_1:
							Traverser(MapPrésent, RootAbyss1);
							break;

						default:
							ArriverÀElnas(MapPrésent);
						}
					case Elnas:
					case ElnasMarket:
					case RootAbyss1:
						Traverser(MapPrésent, Zacum1);
					case Zacum1:
						Traverser(Zacum1, Zacum2_2);
					case Zacum2_2:
						Traverser(Zacum2_2, Zacum3_2);
						UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
					}

					Joeur(&ZacumRaid::Play, "자쿰", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::VONLEON_EASY:
				case USERCONF::CHARACTER_INFO::VONLEON_NORMAL:
				case USERCONF::CHARACTER_INFO::VONLEON_HARD:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
						Traverser(MapPrésent, VonLeon2);
					case VonLeon2:
						UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
						Traverser(VonLeon2, VonLeon3_1 + log2(FlagSeul[p] / (int)USERCONF::CHARACTER_INFO::VONLEON_EASY));
					case VonLeon3_1:
					case VonLeon3_2:
					case VonLeon3_3:;
					}

					Joeur(&VonLeonRaid::Play, "반레온", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::BLOODYQUEEN:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
					case RootAbyss3_3:
					case RootAbyss3_4:
						Traverser(MapPrésent, RootAbyss1);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
						Traverser(RootAbyss1, RootAbyss2_1);
						Traverser(RootAbyss2_1, RootAbyss3_1);
					case RootAbyss3_1:;
					}

					Joeur(&RootAbyssBloodyQueenRaid::Play, "블러디퀸", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::VONBAN:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
					case RootAbyss3_1:
					case RootAbyss3_4:
						Traverser(MapPrésent, RootAbyss1);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
						Traverser(RootAbyss1, RootAbyss2_3);
						Traverser(RootAbyss2_3, RootAbyss3_3);
					case RootAbyss3_3:;
					}

					Joeur(&RootAbyssVonBanRaid::Play, "반반", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::BELLUM:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
					case RootAbyss3_1:
					case RootAbyss3_3:
						Traverser(MapPrésent, RootAbyss1);
					case RootAbyss1:
						UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
						Traverser(RootAbyss1, RootAbyss2_4);
						Traverser(RootAbyss2_4, RootAbyss3_4);
					case RootAbyss3_4:;
					}

					Joeur(&BellumRaid::Play, "벨룸", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::KAUNG:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
						Traverser(MapPrésent, Kaung1);
					case Kaung1:
						KaungRaid::Préparer(CharacterInfo);
						Traverser(Kaung1, Kaung2);
					}

					UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
					Joeur(&KaungRaid::Play, "카웅", CharacterInfo);
				}
				break;

				case USERCONF::CHARACTER_INFO::PAPULATUS_EASY:
				case USERCONF::CHARACTER_INFO::PAPULATUS_NORMAL:
				{
					switch (MapPrésent = WhereAmI())
					{
					case Unknown:
						throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

					default:
						ArriverÀElnas(MapPrésent);
					case Elnas:
						Traverser(MapPrésent, Papulatus1);
					case Papulatus1:
						switch (FlagSeul[p])
						{
						case USERCONF::CHARACTER_INFO::PAPULATUS_EASY:
							Traverser(Papulatus1, Papulatus2_1);
							break;
						case USERCONF::CHARACTER_INFO::PAPULATUS_NORMAL:
							Traverser(Papulatus1, Papulatus2_2);
							break;
						}
					case Papulatus2_1:
					case Papulatus2_2:;
					}

					UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
					Joeur(&PapulatusRaid::Play, "파풀라투스", CharacterInfo);
				}
				break;
				}
				}
			}

			if (CharacterInfo.FlagInvariable > 0b0011101 ||
				ClientApi::ObtenirNombreDeItem(Read(TARGET_DIR "item//crystal.jpg")) > 9)
			{
				switch (MapPrésent = WhereAmI())
				{
				case Unknown:
					throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

				default:
					ArriverÀElnas(MapPrésent);
				case Elnas:
					Traverser(Elnas, FreeMarket);
				case FreeMarket:
					break;
				}

				Compte::Play(MapleIdInfo);
			}
		}
		catch (BOSSRAID_EXCEPTION_DESCRIPTION Desription)
		{
			throw AppException(Desription, ARE_FLAGS_ON(CharacterInfo.Flag));
		}
		catch (PONT_EXCEPTION_CODE Code)
		{
			string DescriptionDeCode;     
			switch (Code)
			{
			case PONT_EXCEPTION_CODE::A_INVALID:
				DescriptionDeCode = "A_INVALID)"; break;
			case PONT_EXCEPTION_CODE::B_MANQUÉ:
				DescriptionDeCode = "B_MANQUE)"; break;
			case PONT_EXCEPTION_CODE::B_INVALID:
				DescriptionDeCode = "B_INVALID)"; break;
			case PONT_EXCEPTION_CODE::B_INACCESSIBLE:
				DescriptionDeCode = "B_INACCESSIBLE)"; break;
			case PONT_EXCEPTION_CODE::INPUT_ERRONÉ:
				DescriptionDeCode = "INPUT_ERRONE)"; break;
			}

			bool IsRetryAvailable;
			switch (Code)
			{
			case PONT_EXCEPTION_CODE::B_INACCESSIBLE:
				UNFLAG(CharacterInfo.Flag, FlagSeul[p]);
			case PONT_EXCEPTION_CODE::B_INVALID:
				++CountIndisponible;
			case PONT_EXCEPTION_CODE::B_MANQUÉ:
				IsRetryAvailable = CharacterInfo.Flag ? true : false;
				if (++CountIndisponible > 10)
				{
					IsRetryAvailable = false;
				}
				break;
			default:
				IsRetryAvailable = false;
			}
			
			int OptionDeRéparation = AppException::CHANGER_SERVEUR;
			switch (A)
			{
			case Urus:
			case Zacum3_2:
			case RootAbyss2_1:
			case RootAbyss3_1:
			case RootAbyss2_3:
			case RootAbyss3_3:
			case RootAbyss2_4:
			case RootAbyss3_4:
			case VonLeon3_1:
			case VonLeon3_2:
			case VonLeon3_3:
			case Kaung2:
			case Papulatus2_1:
			case Papulatus2_2:
				OptionDeRéparation = AppException::SORTIR_GAME; 
			}
			throw AppException(
				MapFileName[A] + "에서 " + MapFileName[B] + "로 이동 중 실패 (" + DescriptionDeCode,
				IsRetryAvailable,
				OptionDeRéparation);
		}
		catch (const MalrangiException& const Exception)
		{
			throw AppException(Exception.what(), false);
		}
		catch (...)
		{
			throw AppException("예외가 지정형식대로 throw되지 않음", false);
		}
	}

private:
	int CountIndisponible;
};
#endif 
#if defined(BUILD_COMPTE)
#include "compte.h"

class OperationCompte : private Compte, private Pont
{
public:
	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo,
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		int MapPrésent;
		auto Traverser = [this, &CharacterInfo, &MapPrésent](int A, int B)
		{
			BougerDeAÀB(A, B, CharacterInfo);
			return MapPrésent = B;
		};

		try
		{
			switch (MapPrésent = WhereAmI())
			{
			case Unknown:
				throw tuple(PONT_EXCEPTION_CODE::A_INVALID, Unknown, Unknown);

			default:
			{
				switch (MapPrésent)
				{
				case Urus:
					Traverser(Urus, Elnas);
					break;

				case ElnasMarket:
					Traverser(ElnasMarket, Elnas);
					break;

				case Zacum2_2:
					Traverser(Zacum2_2, Zacum1);
				case Zacum1:
					Traverser(Zacum1, Elnas);
					break;

				case RootAbyss1:
					Traverser(RootAbyss1, Elnas);
					break;

				case VonLeon2:
					Traverser(VonLeon2, VonLeon1);
				case VonLeon1:
					Traverser(VonLeon1, Elnas);
					break;
				}
			}
			case Elnas:
				Traverser(Elnas, FreeMarket);
			case FreeMarket:
				break;
			}
			Compte::Play(MapleIdInfo);
		}
		catch (tuple<PONT_EXCEPTION_CODE, int, int> CollectivitéDeCode)
		{
			string DescriptionDeCode;
			switch (get<0>(CollectivitéDeCode))
			{
			case PONT_EXCEPTION_CODE::A_INVALID:
				DescriptionDeCode = "A_INVALID)"; break;
			case PONT_EXCEPTION_CODE::B_MANQUÉ:
				DescriptionDeCode = "B_MANQUE)"; break;
			case PONT_EXCEPTION_CODE::B_INVALID:
				DescriptionDeCode = "B_INVALID)"; break;
			case PONT_EXCEPTION_CODE::B_INACCESSIBLE:
				DescriptionDeCode = "B_INACCESSIBLE)"; break;
			case PONT_EXCEPTION_CODE::INPUT_ERRONÉ:
				DescriptionDeCode = "INPUT_ERRONE)"; break;
			}

			bool IsRetryAvailable;
			switch (get<0>(CollectivitéDeCode))
			{
			case PONT_EXCEPTION_CODE::B_MANQUÉ:
				IsRetryAvailable = true;
				break;
			default:
				IsRetryAvailable = false;
			}

			throw AppException(
				MapFileName[get<1>(CollectivitéDeCode)] + "에서 " +
				MapFileName[get<2>(CollectivitéDeCode)] + "로 이동 중 실패 : " +
				DescriptionDeCode,
				IsRetryAvailable);
		}
		catch (PONT_EXCEPTION_CODE)
		{
			throw AppException("예외코드가 tuple 형식으로 throw되지 않음", false);
		}
		catch (CALC_EXCEPTION_CODE ExceptionCode)
		{
			throw AppException("Manque a faire compte", true);
		}
	}
};
#endif
