#include "operation_circulaire.h"
#include "log.h"
#include "contrôler_ip.h"

int
main(
	int argc,
	char* argv[])
{
	ClientApi::SET_CLIENT_STDPOS();
	Sleep(0x800);

	USERCONF& UserInfo = *(USERCONF::GetInstance());
	try
	{
#ifdef INGAME_DIRECT
#ifdef BUILD_URUS
		UrusPlay Ouvrier;
		Ouvrier.Play(UserInfo.VecNexonAccount[0].VecMapleId[0].VecServer[0].VecCharacter[0], false);
#else
#ifdef BUILD_BOSS
		OperationBossJournal Ouvrier;
#else
		OperationCompte Ouvrier;
#endif
		Ouvrier.Play(UserInfo.VecNexonAccount[0].VecMapleId[0], 
			UserInfo.VecNexonAccount[0].VecMapleId[0].VecServer[0].VecCharacter[0]);
#endif
#else
		ClientApi::ÉTAT_DE_LOGOUT ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::NORMAL;
		IpManage IpManager;
		string Uri;
		int CountIndisponible = 0;

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
		auto EstMapleIdAccompli = [](const vector<USERCONF::SERVER_INFO>& VecServers)
		{
			for (const auto& Server : VecServers)
			{
#ifdef BUILD_URUS
				if (!Server.EstAccompli)
				{
					return false;
				}
#else
				for (const auto& Character : Server.VecCharacter)
				{
					if (!Character.EstAccompli)
					{
						return false;
					}
				}
#endif
			}
			return true;
		};
#ifndef BUILD_URUS
		auto EstServeurAccompli = [](const vector<USERCONF::CHARACTER_INFO>& VecCharacters)
		{
			for (const auto& Character : VecCharacters)
			{
				if (!Character.EstAccompli)
				{
					return false;
				}
			}
			return true;
		};
#endif

		for (auto& NexonAccountInfo : UserInfo.VecNexonAccount)
		{
			for (auto& MapleIdInfo : NexonAccountInfo.VecMapleId)
			{
				Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id;
				auto IsMapleIdCompleted = [](const vector<USERCONF::SERVER_INFO>& VecServers)
				{
					for (const auto& Server : VecServers)
					{
#ifdef BUILD_URUS
						if (!Server.EstAccompli)
						{
							return false;
						}
#else
						for (const auto& Character : Server.VecCharacter)
						{
							if (!Character.EstAccompli)
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

				if (MapleIdInfo.EstAdressePremière ||
					ClientApi::ÉTAT_DE_LOGOUT::FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU == ÉtatDeLogout)
				{
					while (!IpManager.Renouveler(MapleIdInfo.Adresse));
				}

				switch (ÉtatDeLogout)
				{
				case ClientApi::ÉTAT_DE_LOGOUT::NORMAL:
					if (!MapleIdInfo.EstAdressePremière)
					{
						break;
					}
				case ClientApi::ÉTAT_DE_LOGOUT::SERVEUR_DECONNECTE:
				case ClientApi::ÉTAT_DE_LOGOUT::CLIENT_ANORMALEMENT_TERMINE:
				case ClientApi::ÉTAT_DE_LOGOUT::FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU:
				case ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE:
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::Exception& e)
					{
						LEAVE_LOG(&e);

						ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;
						goto __LOGOUT__;
					}
					break;
				}

				try
				{
					ClientApi::Login(NexonAccountInfo, MapleIdInfo);

					ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::NORMAL;
				}
				catch (ClientApi::Exception& e)
				{
					LEAVE_LOG(&e);
					ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;

					goto __LOGOUT__;
				}

				try
				{
					for (auto& ServerInfo : MapleIdInfo.VecServer)
					{
#ifdef BUILD_URUS
						if (0 == ServerInfo.NombreDeCharacter || ServerInfo.EstAccompli)
						{
							continue;
						}

						if (MapleIdInfo.ServeurPourUrus != ServerInfo.NomDeServeur)
						{
							ServerInfo.EstAccompli = true;
							continue;
						}
#else
						auto IsServerCompleted = [](const vector<USERCONF::CHARACTER_INFO>& VecCharacters)
						{
							for each (const auto & Character in VecCharacters)
							{
								if (!Character.EstAccompli)
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
						Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.NomDeServeur;
						auto EnterGame = [&]() -> decltype(ÉtatDeLogout)
						{
							try
							{
								ClientApi::EntrerDansGame(MapleIdInfo);
								return ClientApi::ÉTAT_DE_LOGOUT::NORMAL;
							}
							catch (ClientApi::SecondPasswordNotLiftException& ec)
							{
								LEAVE_LOG(&ec);
								return ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;
							}
							catch (ClientApi::BlockFromCapchaException& ec)
							{
								LEAVE_LOG(&ec);
								return ClientApi::ÉTAT_DE_LOGOUT::FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU;
							}
						};

						try
						{
							ClientApi::EntrerDansServeur(ServerInfo, 27);
						}
						catch (ClientApi::ServerDelayException& e)
						{
							LEAVE_LOG(&e);
							ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::NORMAL;
							KeybdEvent({ VK_RETURN, VK_ESCAPE });

							goto __LOGOUT__;
						}

						ClientApi::SelecterCaractere(ServerInfo);
#ifdef BUILD_URUS
						__REPLAY_APPLICATION__:
						if (ClientApi::ÉTAT_DE_LOGOUT::NORMAL != (ÉtatDeLogout = EnterGame()))
						{
							goto __LOGOUT__;
						}

						auto EstIndexDerinierDeServeur = [&MapleIdInfo]()
						{
							int IndexServeur = 0;
							for (const auto& ServerInfo : MapleIdInfo.VecServer)
							{
								if (ServerInfo.EstAccompli && IndexServeur < MapleIdInfo.VecServer.size() - 1)
								{
									++IndexServeur;
								}
							}
							return IndexServeur == MapleIdInfo.VecServer.size() - 1;
						};

						int OptionDeSortir = AppException::SORTIR_GAME;
						bool EstAppExceptionnel = false;
						try
						{
							UrusPlay OuvrierA;

							OuvrierA.Play(ServerInfo.VecCharacter[0],
								EstIndexDerinierDeServeur() && MapleIdInfo.EstAdresseDernière);
							ServerInfo.EstAccompli = true;
							LEAVE_LOG(nullptr);
						}
						catch (AppException& ae)
						{
							LEAVE_LOG(&ae);

							EstAppExceptionnel = true;
							if (!ae.IsRetryAvailable)
							{
								ServerInfo.EstAccompli = true;
							}
						}

						if (MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].EstAccompli &&
							EstIndexDerinierDeServeur() &&
							MapleIdInfo.EstAdresseDernière)
						{
							goto __LOGOUT__;
						}
						else
						{
							if ((ServerInfo.EstAccompli && EstAppExceptionnel) || !ServerInfo.EstAccompli)
							{
								ClientApi::RemoveAllIngameWindows();
							}
							
							if (EstAppExceptionnel && OptionDeSortir == AppException::CHANGER_SERVEUR)
							{
								ClientApi::ChangerChaîne(true);
							}
							else
							{
								ClientApi::ExitGame();
							}

							if (!ServerInfo.EstAccompli)
							{
								goto __REPLAY_APPLICATION__;
							}
						}
#else
						for (auto& CharacterInfo : ServerInfo.VecCharacter)
						{
							if (CharacterInfo.EstAccompli)
							{
								continue;
							}
							Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.NomDeServeur + "/" + CharacterInfo.NomDeClasse;
#ifdef BUILD_BOSS
							OperationBossJournal Ouvrier;
#else
							OperationCompte Ouvrier;
#endif
						__REPLAY_APPLICATION__:
#ifdef BUILD_BOSS
							if (ARE_FLAGS_ON(CharacterInfo.Flag))
							{
								if (ClientApi::ÉTAT_DE_LOGOUT::NORMAL != (ÉtatDeLogout = EnterGame()))
								{
									goto __LOGOUT__;
								}
							}
							else
							{
								WriteLog(NIVEAU_DE_LOG::WARNING, "All flags are off. Forced to exit game.");
								CharacterInfo.EstAccompli = true;
								KeybdEvent(VK_RIGHT);

								continue;
							}
#else
							if (ÉTAT_DE_LOGOUT::NORMAL != (ÉtatDeLogout = EnterGame()))
							{
								goto __LOGOUT__;
							}
#endif

							int OptionDeSortir = AppException::SORTIR_GAME;
							bool EstAppExceptionnel = false;
							try
							{
								Ouvrier.Play(MapleIdInfo, CharacterInfo);
								CharacterInfo.EstAccompli = true;
							}
							catch (AppException& ae)
							{
								LEAVE_LOG(&ae);

								EstAppExceptionnel = true;
								OptionDeSortir = ae.OptionDeRéparation;
								if (!ae.IsRetryAvailable || ARE_FLAGS_OFF(CharacterInfo.Flag))
								{
									CharacterInfo.EstAccompli = true;
								}
							}

							if (MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].VecCharacter[MapleIdInfo.VecServer[MapleIdInfo.VecServer.size() - 1].VecCharacter.size() - 1].EstAccompli &&
								MapleIdInfo.EstAdresseDernière)
							{
								goto __LOGOUT__;
							}
							else
							{
								if ((CharacterInfo.EstAccompli && EstAppExceptionnel) || !CharacterInfo.EstAccompli)
								{
									ClientApi::EffaceTousWindows();
								}
								
								if (EstAppExceptionnel && OptionDeSortir == AppException::CHANGER_SERVEUR)
								{
									ClientApi::ChangerChaîne(true);
								}
								else
								{
									ClientApi::ExitGame();
								}

								if (!CharacterInfo.EstAccompli)
								{
									goto __REPLAY_APPLICATION__;
								}
							}

							KeybdEvent(VK_RIGHT, 0x400);
						}
#endif
						ClientApi::ExitCharacterWindow();
					}
				}
				catch (ClientApi::ExceptionCommun& e)
				{
					ÉtatDeLogout = e.CestQuoi();
					LEAVE_LOG(&e);
				}
				catch (ClientApi::Exception& e)
				{
					LEAVE_LOG(&e);
					ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;
				}

			__LOGOUT__:
				if (CountIndisponible > 2)
				{
					ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INCOMPATIBLE;
					CountIndisponible = 0;
				}

				switch (ÉtatDeLogout)
				{
				case ClientApi::ÉTAT_DE_LOGOUT::NORMAL:
					CountIndisponible = 0;
					if (MapleIdInfo.EstAdresseDernière)
					{
						ClientApi::TerminateClient();
					}
					else
					{
						try
						{
							ClientApi::Logout();
						}
						catch (ClientApi::ExceptionCommun& e)
						{
							ÉtatDeLogout = e.CestQuoi();
							LEAVE_LOG(&e);

							goto __LOGOUT__;
						}
						catch (ClientApi::Exception& e)
						{
							LEAVE_LOG(&e);
							ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;

							goto __LOGOUT__;
						}
					}
					goto __CHECK_IF_LOOP_COMPLETED__;

				case ClientApi::ÉTAT_DE_LOGOUT::FORCER_A_QUITTER_POUR_RAJUSTER_RESEAU:
				case ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE:
					++CountIndisponible;
				case ClientApi::ÉTAT_DE_LOGOUT::SERVEUR_DECONNECTE:
				case ClientApi::ÉTAT_DE_LOGOUT::CLIENT_ANORMALEMENT_TERMINE:
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

				case ClientApi::ÉTAT_DE_LOGOUT::INCOMPATIBLE:
					ÉtatDeLogout = ClientApi::ÉTAT_DE_LOGOUT::INDISPONIBLE;
					ClientApi::TerminateClient();
					break;

				default:
					WriteLog(NIVEAU_DE_LOG::CRITICAL, "로그아웃 상태가 유효하지 않음");
					goto __EXIT__;
				}
			}
		}
		WriteLog(NIVEAU_DE_LOG::INFO, "모든 루틴 수행을 완료하였습니다.");
	__EXIT__:
		UserInfo.Destroy();
#endif
	}
	catch (...)
	{
		WriteLog(NIVEAU_DE_LOG::CRITICAL, "처리되지 않은 예외로 인해 상위 루프에서 벗어났습니다.");
		//Show(SourceImageClient4);

		try
		{
			rethrow_exception(current_exception());
		}
		catch (const MalrangiException& const e)
		{
			WriteLog(NIVEAU_DE_LOG::CRITICAL, e.what());
		}
		catch (...)
		{
			WriteLog(NIVEAU_DE_LOG::CRITICAL, "MalrangiException 형식이 아닌 처리할 수 없는 예외가 throw되었음");
		}
	}

	Sleep(30000);
	return EXIT_SUCCESS;
}
