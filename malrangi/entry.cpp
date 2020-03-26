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
		int SeqSuccess, SeqFailure = 0;
		auto LEAVE_LOG = [&Uri, &SeqSuccess, &SeqFailure](std::exception* ce)
		{
			Write(SNAP_DIR + to_string(++SeqFailure) + ".jpg", SourceImageClient4Colored);
			WriteLog(((nullptr != ce) ? LOG_LEVEL::FAILURE : LOG_LEVEL::SUCCESS),
				(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
		};

		enum
		{
			NORMAL,
			SERVER_DISCONNECTION,
			ABNORMAL_CLIENT_TERMINATION,
			UNHANDLED // force termination
		};
		int LogoutState = NORMAL;

		for each (const auto & NexonAccountInfo in UserInfo.VecNexonAccount)
		{
			for each (const auto & MapleIdInfo in NexonAccountInfo.VecMapleId)
			{
				bool IsPlayValid = false;
				for each (const auto & ServerInfo in MapleIdInfo.VecServer)
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

				switch (LogoutState)
				{
				case NORMAL:
				case SERVER_DISCONNECTION:
					if (SeqMapleId % 4 != 0)
					{
						break;
					}

				case ABNORMAL_CLIENT_TERMINATION:
				case UNHANDLED:
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::BootFailedException & bfe)
					{
						LEAVE_LOG(&bfe);

						goto EXIT_ROUTINE;
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

					goto __LOGOUT;
				}

				try
				{
					for each (const auto & ServerInfo in MapleIdInfo.VecServer)
					{
						if (ServerInfo.VecCharacter.size() == 0)
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

							goto __LOGOUT;
						}

						ClientApi::SelectCharacter(1);
#if defined(BUILD_URUS)
						ClientApi::EnterGame(MapleIdInfo);
						try
						{
							Worker.Play(ServerInfo.VecCharacter[0]);
							LEAVE_LOG(nullptr);
						}
						catch (AppException & e)
						{
							LEAVE_LOG(&e);
							ClientApi::RemoveAllIngameWindows();
						}

						ClientApi::ExitGame();
#elif defined(BUILD_DAILYBOSS) || defined(BUILD_CALC)
						for each (auto & CharacterInfo in ServerInfo.VecCharacter)
						{
							Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName + "/" + CharacterInfo.ClassName;

							ClientApi::EnterGame(MapleIdInfo);
							try
							{
#ifdef BUILD_DAILYBOSS
								Worker.Play(CharacterInfo);
#else
								Worker.Play(MapleIdInfo);
#endif
							}
							catch (AppException & e)
							{
								LEAVE_LOG(&e);
								ClientApi::RemoveAllIngameWindows();
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
				catch (MalrangiException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;
				}

			__LOGOUT:
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
						catch (MalrangiException & e)
						{
							LEAVE_LOG(&e);
							LogoutState = UNHANDLED;

							goto __LOGOUT;
						}
					}
					break;

				case SERVER_DISCONNECTION:
				case ABNORMAL_CLIENT_TERMINATION:
				case UNHANDLED:
					ClientApi::TerminateClient();
					break;

				default:
					WriteLog(LOG_LEVEL::CRITICAL, "Invalid logout state.\n");
					goto EXIT_ROUTINE;
				}

				++SeqMapleId;
			}
		}

		WriteLog(LOG_LEVEL::INFO,
			"All routines have been successfully complete. (Success : %d, Failure : %d)\n");

	EXIT_ROUTINE:
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

