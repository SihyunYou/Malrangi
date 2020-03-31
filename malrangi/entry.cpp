#include "circular_play.h"
#include "log.h"
#include "ipmanage.h"
#include "report.h"

#define BUILD_URUS
//#define BUILD_DAILYBOSS
//#define BUILD_CALC
#define USING_IPRENEWER

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
#if defined(BUILD_URUS)	
		UrusPlay Worker;
#elif defined(BUILD_DAILYBOSS)	
		DailyBossPlay Worker;
#elif defined(BUILD_CALC)	
		CalcPlay Worker;
#endif
#ifdef USING_IPRENEWER
		IPCONF& IpInfo = *(IPCONF::GetInstance());
		IpManage IpManager;
#else
		WriteLog(LOG_LEVEL::WARNING,
			"This program does not support ip renewal. (Flag USING_IPRENEWER is not defined.)"
			"The number of accounts written in conf file more than 5 is ignored and this program will be terminated itself.\n");
#endif

		string Uri;
		int SeqMapleId = 0;
		auto LEAVE_LOG = [&Uri](std::exception* ce)
		{
			if (nullptr != ce)
			{
				static int Seq = 0;
				Write(SNAP_DIR + to_string(++Seq) + ".jpg", SourceImageClient4Colored);
			}
			WriteLog(((nullptr != ce) ? LOG_LEVEL::FAILURE : LOG_LEVEL::SUCCESS),
				(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
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
			UNHANDLED
		};
		int LogoutState = NORMAL;

		for (auto & NexonAccountInfo : UserInfo.VecNexonAccount)
		{
			for (auto & MapleIdInfo : NexonAccountInfo.VecMapleId)
			{
				bool IsPlayValid = false;
				for each (auto & ServerInfo in MapleIdInfo.VecServer)
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
				
				Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id;
				if (SeqMapleId % 4 == 0)
				{
#ifdef USING_IPRENEWER
					IpManager.Renew(IpInfo.InternetInfo);
#else
					if (SeqMapleId > 3)
					{
						goto EXIT_ROUTINE;
					}
#endif
				}

			__BOOTUP__:
				switch (LogoutState)
				{
				case NORMAL:
					if (SeqMapleId % 4 != 0)
					{
						break;
					}

				case SERVER_DISCONNECTION:
				case ABNORMAL_CLIENT_TERMINATION:
				case UNHANDLED:
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::BootFailedException & bfe)
					{
						LEAVE_LOG(&bfe);

						goto __EXIT__;
					}
					break;
				}

				try
				{
					ClientApi::Login(NexonAccountInfo, MapleIdInfo);
					LogoutState = NORMAL;
				}
				catch (MalrangiException & e)
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

						int SeqNotCompletedCharacter = 1;
						for (; SeqNotCompletedCharacter <= ServerInfo.VecCharacter.size(); SeqNotCompletedCharacter++)
						{
							if (ServerInfo.VecCharacter[SeqNotCompletedCharacter].IsCompleted)
							{
								continue;
							}
							else
							{
								break;
							}
						}
						ClientApi::SelectCharacter(SeqNotCompletedCharacter);

#if defined(BUILD_URUS)
					__REPLAY_APPLICATION__:
						ClientApi::EnterGame(MapleIdInfo);

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
						for(auto & CharacterInfo : ServerInfo.VecCharacter)
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
								ClientApi::EnterGame(MapleIdInfo);
							}
							else
							{
								WriteLog(LOG_LEVEL::WARNING, "All flags are off. Forced to exit game.");
								CharacterInfo.IsCompleted = true;
								KeybdEvent(VK_RIGHT);

								continue;
							}
#else
							ClientApi::EnterGame(MapleIdInfo);
#endif
							try
							{	
#ifdef BUILD_DAILYBOSS
								Worker.Play(CharacterInfo);
#else
								Worker.Play(MapleIdInfo);
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

							if (++IndexCharacter == ServerInfo.VecCharacter.size())
							{
								ServerInfo.IsCompleted = true;
							}

							ClientApi::ExitGame();
							KeybdEvent(VK_RIGHT);
						}
#endif
						ClientApi::ExitCharacterWindow();
					}
				}
				catch (ClientApi::ServerDisconnectedException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = SERVER_DISCONNECTION;
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

				case SERVER_DISCONNECTION:
				case ABNORMAL_CLIENT_TERMINATION:
					ClientApi::TerminateClient();
#if defined(BUILD_URUS)
					if(!IsServerLoopCompleted(MapleIdInfo.VecServer))
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
					if(!IsCharacterLoopCompleted(MapleIdInfo.VecServer))
#endif
					{
						goto __BOOTUP__;
					}
					else
					{
						break;
					}
					

				case UNHANDLED:
					ClientApi::TerminateClient();
					break;

				default:
					WriteLog(LOG_LEVEL::CRITICAL, "INVALID LOGOUT STATE.\n");
					goto __EXIT__;
				}

				++SeqMapleId;
			}
		}

		WriteLog(LOG_LEVEL::INFO, "All routines have been successfully complete.");

	__EXIT__:
		UserInfo.Destroy();
		IpInfo.Destroy();
	}
	catch (MalrangiException & me)
	{
		WriteLog(LOG_LEVEL::CRITICAL, me.what());
	}

	Sleep(10000000);
	return EXIT_SUCCESS;
}

