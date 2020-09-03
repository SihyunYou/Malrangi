#include "circular_play.h"
#include "log.h"
#include "ipmanage.h"
#include "report.h"

int
main(
	int argc,
	char* argv[])
{
	ClientApi::SET_CLIENT_STDPOS();
	Sleep(0x800);

	try
	{
		USERCONF& UserInfo = *(USERCONF::GetInstance());
		IpManage IpManager;
		string Uri;
		int CountIndisponible = 0;
		bool EstForceARenoveller = false;
		auto LEAVE_LOG = [&Uri](std::exception* ce)
		{
			if (nullptr != ce)
			{
				static int Seq = 0;
				Write(SNAP_DIR + to_string(++Seq) + ".jpg", SourceImageClient4Colored);
			}
			WriteLog(((nullptr != ce) ? NIVEAU_DE_LOG::FAILURE : NIVEAU_DE_LOG::SUCCESS),
				(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
		};

		enum
		{
			NORMAL,
			SERVEUR_DECONNECTE,
			CLIENT_ANORMALEMENT_TERMINE,
			FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU,
			INDISPONIBLE,
			INCOMPATIBLE,
		};
		int EtatDeLogout = NORMAL;

		for (auto& NexonAccountInfo : UserInfo.VecNexonAccount)
		{
			for (auto& MapleIdInfo : NexonAccountInfo.VecMapleId)
			{
				Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id;
				auto IsMapleIdCompleted = [](const vector<USERCONF::SERVER_INFO>& VecServers)
				{
					for (const auto& Server : VecServers)
					{
#if defined(BUILD_URUS)
						if (!Server.IsCompleted)
						{
							return false;
						}
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
						for (const auto& Character : Server.VecCharacter)
						{
							if (!Character.IsCompleted)
							{
								return false;
							}
						}
#endif
					}
					return true;
				};

			__BOOTUP_PREPROCESSING__:
				if (MapleIdInfo.VecServer.size() == 0 || IsMapleIdCompleted(MapleIdInfo.VecServer))
				{
					continue;
				}
				
				do
				{
					IpManager.ConnectNetwork();
				} while (!IpManager.IsNetworkConnected());

				if (MapleIdInfo.EstAdressePremier || EstForceARenoveller)
				{
					while (!IpManager.Renouveler(MapleIdInfo.Adresse));
				}
				EstForceARenoveller = false;

				switch (EtatDeLogout)
				{
				case NORMAL:
					if (!MapleIdInfo.EstAdressePremier)
					{
						break;
					}
				case SERVEUR_DECONNECTE:
				case CLIENT_ANORMALEMENT_TERMINE:
				case FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU:
				case INDISPONIBLE:
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::NetworkDisconnectedException & e)
					{
						LEAVE_LOG(&e);

						EtatDeLogout = FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU;
						goto __LOGOUT__;
					}
					catch (ClientApi::Exception & e)
					{
						LEAVE_LOG(&e);

						EtatDeLogout = INDISPONIBLE;
						goto __LOGOUT__;
					}
					break;
				}

				try
				{
					ClientApi::Login(NexonAccountInfo, MapleIdInfo);

					EtatDeLogout = NORMAL;
				}
				catch (ClientApi::Exception & e)
				{
					LEAVE_LOG(&e);
					EtatDeLogout = INDISPONIBLE;

					goto __LOGOUT__;
				}

				try
				{
					for (auto& ServerInfo : MapleIdInfo.VecServer)
					{
#if defined(BUILD_URUS)
						if (0 == ServerInfo.NombreDeCharacter || ServerInfo.IsCompleted)
						{
							continue;
						}
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
						auto IsServerCompleted = [](const vector<USERCONF::CHARACTER_INFO>& VecCharacters)
						{
							for each (const auto & Character in VecCharacters)
							{
								if (!Character.IsCompleted)
								{
									return false;
								}
							}
							return true;
						};

						if (ServerInfo.VecCharacter.size() == 0 || IsServerCompleted(ServerInfo.VecCharacter))
						{
							continue;
						}
#endif
						Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName;
						auto EnterGame = [&]() -> decltype(EtatDeLogout)
						{
							try
							{
								ClientApi::EntrerDansGame(MapleIdInfo);
								return NORMAL;
							}
							catch (ClientApi::SecondPasswordNotLiftException & ec)
							{
								LEAVE_LOG(&ec);
								return INDISPONIBLE;
							}
							catch (ClientApi::BlockFromCapchaException & ec)
							{
								LEAVE_LOG(&ec);
								return FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU;
							}
						};

						try
						{
							ClientApi::EntrerDansServeur(ServerInfo, 27);
						}
						catch (ClientApi::ServerDelayException & e)
						{
							LEAVE_LOG(&e);
							EtatDeLogout = NORMAL;
							KeybdEvent({ VK_RETURN, VK_ESCAPE });

							goto __LOGOUT__;
						}

						ClientApi::SelecterCaractere(ServerInfo);
#if defined(BUILD_URUS)
						__REPLAY_APPLICATION__:
						if (NORMAL != (EtatDeLogout = EnterGame()))
						{
							goto __LOGOUT__;
						}

						auto EstIndexDerinierDeServeur = [&MapleIdInfo]()
						{
							int IndexServeur = 0;
							for (const auto& ServerInfo : MapleIdInfo.VecServer)
							{
								if (ServerInfo.IsCompleted && IndexServeur < MapleIdInfo.VecServer.size() - 1)
								{
									++IndexServeur;
								}
							}
							return IndexServeur == MapleIdInfo.VecServer.size() - 1;
						};
						bool IsAppExceptional = false;

						try
						{
							UrusPlay OuvrierA;
							OuvrierA.Play(EstIndexDerinierDeServeur() && MapleIdInfo.EstAdresseDernier);

							ServerInfo.IsCompleted = true;
							LEAVE_LOG(nullptr);
						}
						catch (AppException & ae)
						{
							LEAVE_LOG(&ae);

							IsAppExceptional = true;
							if (!ae.IsRetryAvailable)
							{
								ServerInfo.IsCompleted = true;
							}
						}

						if (MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].IsCompleted &&
							EstIndexDerinierDeServeur() && 
							MapleIdInfo.EstAdresseDernier)
						{
							goto __LOGOUT__;
						}
						else
						{
							if ((ServerInfo.IsCompleted && IsAppExceptional) || !ServerInfo.IsCompleted)
							{
								ClientApi::RemoveAllIngameWindows();
							}
							ClientApi::ExitGame();

							if (!ServerInfo.IsCompleted)
							{
								goto __REPLAY_APPLICATION__;
							}
						}
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
						for (auto& CharacterInfo : ServerInfo.VecCharacter)
						{
							if (CharacterInfo.IsCompleted)
							{
								continue;
							}
							Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName + "/" + CharacterInfo.ClassName;

						__REPLAY_APPLICATION__:
#ifdef BUILD_DAILYBOSS
							if (ARE_FLAGS_ON(CharacterInfo.Flag))
							{
								if (NORMAL != (EtatDeLogout = EnterGame()))
								{
									goto __LOGOUT__;
								}
							}
							else
							{
								WriteLog(NIVEAU_DE_LOG::WARNING, "All flags are off. Forced to exit game.");
								CharacterInfo.IsCompleted = true;
								KeybdEvent(VK_RIGHT);

								continue;
							}
#else
							if (NORMAL != (EtatDeLogout = EnterGame()))
							{
								goto __LOGOUT__;
							}
#endif

							bool IsAppExceptional = false;
							try
							{
#ifdef BUILD_DAILYBOSS
								DailyBossPlay Ouvrier;
								Ouvrier.Play(CharacterInfo);
#else
								CalcPlay Ouvrier;
								Ouvrier.Play(MapleIdInfo);
#endif
								CharacterInfo.IsCompleted = true;
							}
							catch (AppException & ae)
							{
								LEAVE_LOG(&ae);

								IsAppExceptional = true;
								if (!ae.IsRetryAvailable)
								{
									CharacterInfo.IsCompleted = true;
								}
							}

							if (MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].VecCharacter[MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].VecCharacter.size() - 1].IsCompleted &&
								MapleIdInfo.EstAdresseDernier)
							{
								goto __LOGOUT__;
							}
							else
							{
								if ((CharacterInfo.IsCompleted && IsAppExceptional) || !CharacterInfo.IsCompleted)
								{
									ClientApi::RemoveAllIngameWindows();
								}
								ClientApi::ExitGame();

								if (!CharacterInfo.IsCompleted)
								{
									goto __REPLAY_APPLICATION__;
								}
							}

							if (!ServerInfo.VecCharacter[ServerInfo.VecCharacter.size() - 1].IsCompleted)
							{
								KeybdEvent(VK_RIGHT);
							}
						}
#endif
						ClientApi::ExitCharacterWindow();
					}
				}
				catch (ClientApi::ServerDisconnectedException & e)
				{
					LEAVE_LOG(&e);
					EtatDeLogout = SERVEUR_DECONNECTE;
				}
				catch (ClientApi::NetworkDisconnectedException & e)
				{
					LEAVE_LOG(&e);
					EtatDeLogout = FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU;
				}
				catch (ClientApi::ClientAbnormalTerminationException & e)
				{
					LEAVE_LOG(&e);
					EtatDeLogout = CLIENT_ANORMALEMENT_TERMINE;
				}
				catch (ClientApi::Exception & e)
				{
					LEAVE_LOG(&e);
					EtatDeLogout = INDISPONIBLE;
				}

			__LOGOUT__:
				if (CountIndisponible > 2)
				{
					EtatDeLogout = INCOMPATIBLE;
					CountIndisponible = 0;
				}

				switch (EtatDeLogout)
				{
				case NORMAL:
					if (MapleIdInfo.EstAdresseDernier)
					{
						ClientApi::TerminateClient();
					}
					else
					{
						try
						{
							ClientApi::Logout();
						}
						catch (ClientApi::Exception & e)
						{
							LEAVE_LOG(&e);
							EtatDeLogout = INDISPONIBLE;

							goto __LOGOUT__;
						}
					}
					goto __CHECK_IF_LOOP_COMPLETED__;

				case FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU:
					EstForceARenoveller = true;
				case INDISPONIBLE:
					++CountIndisponible;
				case SERVEUR_DECONNECTE:
				case CLIENT_ANORMALEMENT_TERMINE:
					ClientApi::TerminateClient();
				__CHECK_IF_LOOP_COMPLETED__:
					if (!IsMapleIdCompleted(MapleIdInfo.VecServer))
					{
						goto __BOOTUP_PREPROCESSING__;
					}
					else
					{
						break;
					}
				
				case INCOMPATIBLE:
					EtatDeLogout = INDISPONIBLE;
					ClientApi::TerminateClient();
					break;

				default:
					WriteLog(NIVEAU_DE_LOG::CRITICAL, "L'etat de logout invalide.\n");
					goto __EXIT__;
				}
			}
		}

		WriteLog(NIVEAU_DE_LOG::INFO, "Tous les routines ont accompli.");

	__EXIT__:
		UserInfo.Destroy();
	}
	catch (MalrangiException & e)
	{
		WriteLog(NIVEAU_DE_LOG::CRITICAL, e.what());
		Sleep(300000000);
	}
	catch (exception & e)
	{
		WriteLog(NIVEAU_DE_LOG::CRITICAL, e.what());
		Sleep(300000000);
	}
	system("shutdown -s -t 60");

	return EXIT_SUCCESS;
}
