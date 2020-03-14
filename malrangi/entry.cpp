#include "field.h"
#include "raid.h"
#include "calc.h"
#include "log.h"
#include "ipmanage.h"
#include "report.h"
#include "bridge.h"

#define BUILD_URUSRAID
//#define BUILD_ZACUMRAID
//#define BUILD_CALC
//#define BUILD_FIELDBOT

#define USING_IPRENEWER



int 
main(
	int argc,
	char* argv[])
{
	Sleep(0x800);

#if defined(BUILD_FIELDBOT)
	try
	{
		Bot Botter;
		Botter.Play();
	}
	catch (std::exception & me)
	{
		WriteLog(LOG_LEVEL::CRITICAL, me.what());
	}

#elif defined(BUILD_URUSRAID) || defined(BUILD_ZACUMRAID) || defined(BUILD_CALC)
	try
	{
		USERCONF& UserInfo = *(USERCONF::GetInstance());
#if defined(BUILD_URUSRAID)
		UrusRaid Worker;
#elif defined(BUILD_ZACUMRAID)
		ZacumRaid Worker;
#elif defined(BUILD_CALC)
		Calc Worker;
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
		auto LEAVE_LOG = [&Uri, &SeqSuccess, &SeqFailure](std::exception* ce) -> void
		{
			if (nullptr != ce)
			{
				Cvw::Write(SNAP_DIR "exception" + INT_TO_PNG((nullptr != ce) ? ++SeqFailure : ++SeqSuccess),
					Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));
			}
			WriteLog(((nullptr != ce) ? LOG_LEVEL::FAILURE : LOG_LEVEL::SUCCESS),
				(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
		};

		enum LOGOUT_STATE
		{
			NORMAL,
			SERVER_DISCONNECTION,
			ABNORMAL_CLIENT_TERMINATION,
			UNHANDLED // force termination
		};
		int LogoutState = NORMAL;


		for each (const auto& NexonAccountInfo in UserInfo.VecNexonAccount)
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
				switch (LogoutState)
				{
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
				}
				catch (ClientApi::NexonLoginFailedException& e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;

					goto __LOGOUT;
				}
				catch (ClientApi::MapleLoginFailedException & e)
				{
					LEAVE_LOG(&e);
					LogoutState = UNHANDLED;

					goto __LOGOUT;
				}


				for each (const auto & ServerInfo in MapleIdInfo.VecServer)
				{
					if (ServerInfo.VecCharacter.size() == 0)
					{
						continue;
					}
					Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName;

					try
					{
						try
						{
							ClientApi::SelectServer(ServerInfo, 27);
						}
						catch (ClientApi::ServerEntryFailedException & e)
						{
							ThrowClientLowestException();
						}
						ClientApi::SelectCharacter(1);

#if defined(BUILD_URUSRAID)
						try
						{
							ClientApi::EnterGame(MapleIdInfo);
						}
						catch (ClientApi::GameEntryException)
						{
							ThrowClientLowestException();
						}

						try
						{
							ClientApi::BreakParty();
							Worker.Play(ServerInfo.VecCharacter[0]);

							LEAVE_LOG(nullptr);
						}
						catch (BossException & e)
						{
							LEAVE_LOG(&e);
							ClientApi::RemoveAllIngameWindows();
						}

						try
						{
							ClientApi::ExitGame();
						}
						catch (ClientApi::GameExitException)
						{
							ThrowClientLowestException();
						}
#elif defined(BUILD_ZACUMRAID) || defined(BUILD_CALC)
						bool IsFirstRoutine = true;
						for each (auto & CharacterInfo in ServerInfo.VecCharacter)
						{
							Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName + "/" + CharacterInfo.CharacterType;
							try
							{
								ClientApi::EnterGame(MapleIdInfo);
							}
							catch (ClientApi::GameEntryException)
							{
								ThrowClientLowestException();
							}

							try
							{
#ifdef BUILD_ZACUMRAID
								ClientApi::MakeParty();
								if (IsFirstRoutine)
								{
									IsFirstRoutine = false;
									Worker.PlayFromUrus(CharacterInfo);
								}
								else
								{
									Worker.PlayFromZacum(CharacterInfo);
								}
#else
								if (IsFirstRoutine)
								{
									IsFirstRoutine = false;
									Worker.PlayFromUrus(MapleIdInfo);
								}
								else
								{
									Worker.PlayFromZacum(MapleIdInfo);
								}
#endif
							}
							catch (BossException & e)
							{
								LEAVE_LOG(&e);
								ClientApi::RemoveAllIngameWindows();
							}

							try
							{
								ClientApi::ExitGame();
							}
							catch (ClientApi::GameExitException)
							{
								ThrowClientLowestException();
							}

							KeybdEvent(VK_RIGHT);
						}
#endif

						try
						{
							ClientApi::ExitCharacterWindow();
						}
						catch (ClientApi::CharacterWindowExitException)
						{
							ThrowClientLowestException();
						}
					}
					catch (ClientApi::ServerDisconnectedException & e)
					{
						LEAVE_LOG(&e);
						LogoutState = SERVER_DISCONNECTION;

						goto __LOGOUT;
					}
					catch (ClientApi::ClientAbnormalTerminationException & e)
					{
						LEAVE_LOG(&e);
						LogoutState = ABNORMAL_CLIENT_TERMINATION;

						goto __LOGOUT;
					}
					catch (MalrangiException & e)
					{
						LEAVE_LOG(&e);
						LogoutState = UNHANDLED;

						goto __LOGOUT;
					}
				}

			/***************************************************************************************************************
			* E2 -> E1 / E0
			***************************************************************************************************************/
			__LOGOUT:
				switch(LogoutState)
				{
				case SUCCESS:
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
					if (SeqMapleId % 4 == 3)
					{
						ClientApi::TerminateClient();
					}
					else
					{
						KeybdEvent(VK_RETURN);
					}
					break;

				case ABNORMAL_CLIENT_TERMINATION:
				case UNHANDLED:
					ClientApi::TerminateClient();
					break;
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
#endif

	Sleep(10000000);
	return EXIT_SUCCESS;
}

