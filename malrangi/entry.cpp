#include "circular_play.h"
#include "log.h"
#include "ipmanage.h"
#include "report.h"

//#define BUILD_URUS
#define BUILD_DAILYBOSS
//#define BUILD_CALC

int
main(
	int argc,
	char* argv[])
{
	ClientApi::SET_CLIENT_STDPOS();
	Sleep(0x800);
	BossRaid a;
	static map<string, vector<BossRaid::SKILL>> MapVecSkills =
	{
		{
				"데벤",
				{
					{'1', BossRaid::SKILL::BUF},
					{'2', BossRaid::SKILL::BUF},
					{'3', BossRaid::SKILL::BUF},
					{'4', BossRaid::SKILL::BUF},
					{'T', BossRaid::SKILL::ASSIST, seconds(180)},
					{'E', BossRaid::SKILL::ASSIST, seconds(6)},
					{'Q', BossRaid::SKILL::UNITARY}
				}
			}
	};
	a.SetSkills(&MapVecSkills["데벤"]);

	a.RaidDoBattle(
		[]() -> bool
		{
			return false;
		},
7000,
true);
	try
	{
		USERCONF& UserInfo = *(USERCONF::GetInstance());
#if defined(BUILD_URUS)	
		UrusPlay Worker;
#elif defined(BUILD_DAILYBOSS)	
		DailyBossPlay Worker;
#elif defined(BUILD_CALC)	
		CalcPlay Worker;
#endif
		IpManage IpManager;
		string Uri;
		int SeqMapleId = 0;
		int UnhandledCount = 0;
		auto LEAVE_LOG = [&Uri](std::exception* ce)
		{
			if (nullptr != ce)
			{
				static int Seq = 0;
			//	Write(SNAP_DIR + to_string(++Seq) + ".jpg", SourceImageClient4Colored);
			}
			//WriteLog(((nullptr != ce) ? LOG_LEVEL::FAILURE : LOG_LEVEL::SUCCESS),
				//(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
			printf((Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
		};
		auto IsServerLoopCompleted = [](const vector<USERCONF::SERVER_INFO>& VecServers)
		{
			for each (const auto & Server in VecServers)
			{
				if (!Server.IsCompleted)
				{
					return false;
				}
			}
			return true;
		};
		auto IsCharacterLoopCompleted = [](const vector<USERCONF::SERVER_INFO>& VecServers)
		{
			for each (const auto & Server in VecServers)
			{
				for each (const auto & Character in Server.VecCharacter)
				{
					if (!Character.IsCompleted)
					{
						return false;
					}
				}
			}
			return true;
		};

		enum
		{
			NORMAL,
			SERVER_DISCONNECTION,
			ABNORMAL_CLIENT_TERMINATION,
			FORCE_TERMINATION_FOR_NETWORK_RECONNECT,
			FORCE_TERMINATION_FOR_IPRENEWAL,
			UNHANDLED,
			UNHANDLABLE,
		};
		int LogoutState = NORMAL;

		for (auto& NexonAccountInfo : UserInfo.VecNexonAccount)
		{
			for (auto& MapleIdInfo : NexonAccountInfo.VecMapleId)
			{
				Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id;
				bool IsPlayValid = false;
				for (auto & ServerInfo : MapleIdInfo.VecServer)
				{
					if (ServerInfo.VecCharacter.size() > 0)
					{
						IsPlayValid = true;
						break;
					}
				}
				if (!IsPlayValid)
				{
					continue;
				}
				
			__BOOTUP_PREPROCESSING__:
				if (FORCE_TERMINATION_FOR_NETWORK_RECONNECT == LogoutState)
				{
					IpManager.ConnectNetwork();
				}
				if (SeqMapleId % 4 == 0)
				{
					IpManager.Renew();
					SeqMapleId = 0;
				}

				switch (LogoutState)
				{
				case NORMAL:
					if (SeqMapleId % 4 != 0)
					{
						break;
					}
				case SERVER_DISCONNECTION:
				case ABNORMAL_CLIENT_TERMINATION:
				case FORCE_TERMINATION_FOR_NETWORK_RECONNECT:
				case FORCE_TERMINATION_FOR_IPRENEWAL:
				case UNHANDLED:
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::NetworkDisconnectedException & ec)
					{
						LEAVE_LOG(&ec);

						LogoutState = FORCE_TERMINATION_FOR_NETWORK_RECONNECT;
						goto __LOGOUT__;
					}
					catch (ClientApi::BootFailedException & ec)
					{
						LEAVE_LOG(&ec);

						LogoutState = UNHANDLED;
						goto __LOGOUT__;
					}
					break;
				}

				try
				{
					ClientApi::Login(NexonAccountInfo, MapleIdInfo);

					++SeqMapleId;
					LogoutState = NORMAL;
				}
				catch (ClientApi::GameNotConnectableException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = FORCE_TERMINATION_FOR_NETWORK_RECONNECT;

					goto __LOGOUT__;
				}
				catch (ClientApi::NexonLoginFailedException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;

					goto __LOGOUT__;
				}
				catch (ClientApi::Exception & e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;

					goto __LOGOUT__;
				}

				try
				{
					for (auto& ServerInfo : MapleIdInfo.VecServer)
					{
						if (ServerInfo.VecCharacter.size() == 0 || ServerInfo.IsCompleted)
						{
							continue;
						}
						Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName;

						try
						{
							ClientApi::SelectServer(ServerInfo, 27);
						}
						catch (ClientApi::ServerDelayException & e)
						{
							LEAVE_LOG(&e);
							LogoutState = NORMAL;
							KeybdEvent({ VK_RETURN, VK_ESCAPE });

							goto __LOGOUT__;
						}

						ClientApi::SelectCharacter(ServerInfo);

#if defined(BUILD_URUS)
						__REPLAY_APPLICATION__:
						try
						{
							ClientApi::EnterGame(MapleIdInfo);
						}
						catch (ClientApi::BlockFromCapchaException & ec)
						{
							LEAVE_LOG(&ec);

							LogoutState = FORCE_TERMINATION_FOR_IPRENEWAL;
							goto __LOGOUT__;
						}
						catch (ClientApi::SecondPasswordNotLiftException & ec)
						{
							LEAVE_LOG(&ec);

							LogoutState = UNHANDLED;
							goto __LOGOUT__;
						}

						try
						{
							Worker.Play(ServerInfo.VecCharacter[0]);

							ServerInfo.IsCompleted = true;
							LEAVE_LOG(nullptr);
						}
						catch (AppException & ae)
						{
							LEAVE_LOG(&ae);

							ClientApi::RemoveAllIngameWindows();
							ClientApi::ExitGame();

							if (ae.IsRetryAvailable)
							{
								goto __REPLAY_APPLICATION__;
							}
							else
							{
								ServerInfo.IsCompleted = true;
							}
						}
						ClientApi::ExitGame();
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
						int IndexCharacter = 0;
						for (auto& CharacterInfo : ServerInfo.VecCharacter)
						{
							if (CharacterInfo.IsCompleted)
							{
								continue;
							}
							Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName + "/" + CharacterInfo.ClassName;

						__REPLAY_APPLICATION__:
#ifdef BUILD_DAILYBOSS
							if (ARE_FLAGS_ON(CharacterInfo.Type))
							{
								try
								{
									ClientApi::EnterGame(MapleIdInfo);
								}
								catch (ClientApi::SecondPasswordNotLiftException & ec)
								{
									LEAVE_LOG(&ec);

									LogoutState = UNHANDLED;
									goto __LOGOUT__;
								}
								catch (ClientApi::BlockFromCapchaException & ec)
								{
									LEAVE_LOG(&ec);

									LogoutState = FORCE_TERMINATION_FOR_IPRENEWAL;
									goto __LOGOUT__;
								}
							}
							else
							{
								WriteLog(LOG_LEVEL::WARNING, "All flags are off. Forced to exit game.");
								CharacterInfo.IsCompleted = true;
								KeybdEvent(VK_RIGHT);

								continue;
							}
#else
							try
							{
								ClientApi::EnterGame(MapleIdInfo);
							}
							catch (ClientApi::BlockFromCapchaException & ec)
							{
								LEAVE_LOG(&ec);

								LogoutState = FORCE_TERMINATION_FOR_IPRENEWAL;
								goto __LOGOUT__;
							}
							catch (ClientApi::SecondPasswordNotLiftException & ec)
							{
								LEAVE_LOG(&ec);

								LogoutState = UNHANDLED;
								goto __LOGOUT__;
							}
#endif
							try
							{
#ifdef BUILD_DAILYBOSS
								Worker.Play(CharacterInfo);
#else
								Worker.Play(MapleIdInfo, CharacterInfo);
#endif
								CharacterInfo.IsCompleted = true;
							}
							catch (AppException & ae)
							{
								LEAVE_LOG(&ae);

								ClientApi::RemoveAllIngameWindows();
								ClientApi::ExitGame();

								if (ae.IsRetryAvailable)
								{
									goto __REPLAY_APPLICATION__;
								}
								else
								{
									CharacterInfo.IsCompleted = true;
									KeybdEvent(VK_RIGHT);
									continue;
								}
							}

							ServerInfo.IsCompleted = true;
							for (const auto& CharacterInfo : ServerInfo.VecCharacter)
							{
								if (!CharacterInfo.IsCompleted)
								{
									ServerInfo.IsCompleted = false;
									break;
								}
							}

							if (ServerInfo.IsCompleted && (SeqMapleId % 4 == 3))
							{
								goto __LOGOUT__;
							}
							else
							{
								ClientApi::ExitGame();
								KeybdEvent(VK_RIGHT);
							}
						}
#endif
						ServerInfo.IsCompleted = true;
						ClientApi::ExitCharacterWindow();
					}
				}
				catch (ClientApi::ServerDisconnectedException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = SERVER_DISCONNECTION;
				}
				catch (ClientApi::NetworkDisconnectedException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = FORCE_TERMINATION_FOR_NETWORK_RECONNECT;
				}
				catch (ClientApi::ClientAbnormalTerminationException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = ABNORMAL_CLIENT_TERMINATION;
				}
				catch (ClientApi::Exception & e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;
				}

			__LOGOUT__:
				if (UnhandledCount > 2)
				{
					LogoutState = UNHANDLABLE;
					UnhandledCount = 0;
				}

				switch (LogoutState)
				{
				case NORMAL:
					if (SeqMapleId % 4 == 3)
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
							LogoutState = UNHANDLED;

							goto __LOGOUT__;
						}
					}
					break;

				case FORCE_TERMINATION_FOR_IPRENEWAL:
					SeqMapleId = 0;
				case FORCE_TERMINATION_FOR_NETWORK_RECONNECT:
				case SERVER_DISCONNECTION:
				case ABNORMAL_CLIENT_TERMINATION:
				case UNHANDLED:
					if (UNHANDLED == LogoutState)
					{
						++UnhandledCount;
					}

					ClientApi::TerminateClient();
#if defined(BUILD_URUS)
					if (!IsServerLoopCompleted(MapleIdInfo.VecServer))
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
					if (!IsCharacterLoopCompleted(MapleIdInfo.VecServer))
#endif
					{
						goto __BOOTUP_PREPROCESSING__;
					}
					else
					{
						break;
					}
				
				case UNHANDLABLE:
					LogoutState = UNHANDLED;
					ClientApi::TerminateClient();
					break;

				default:
					WriteLog(LOG_LEVEL::CRITICAL, "INVALID LOGOUT STATE.\n");
					goto __EXIT__;
				}
			}
		}

		WriteLog(LOG_LEVEL::INFO, "All routines have been successfully complete.");

	__EXIT__:
		UserInfo.Destroy();
	}
	catch (MalrangiException & me)
	{
		WriteLog(LOG_LEVEL::CRITICAL, me.what());
	}

	system("shutdown -s -t 60");

	return EXIT_SUCCESS;
}
