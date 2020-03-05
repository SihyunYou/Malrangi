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
		bool IsExgameExceptional = false;
		bool IsIngameExceptional = false;
		auto LOG_TREE = [&Uri](std::exception* ce) -> void
		{
			WriteLog(((nullptr != ce) ? LOG_LEVEL::FAILURE : LOG_LEVEL::SUCCESS), 
				(Uri + ((nullptr != ce) ? "? " : "") + ((nullptr != ce) ? ce->what() : "") + "\n").c_str());
		};

		for each (const auto& NexonAccountInfo in UserInfo.VecNexonAccount)
		{
			for each (const auto & MapleIdInfo in NexonAccountInfo.VecMapleId)
			{
				bool IsPlayValid = false;
				for each (const auto & ServerInfo in MapleIdInfo.VecServer)
				{
#if defined(BUILD_URUSRAID)
					if (ServerInfo.VecCharacter.size() > 0)
#elif defined(BUILD_ZACUMRAID) || defined(BUILD_CALC)
					if (ServerInfo.VecCharacter.size() > 1)
#endif
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

				/***************************************************************************************************************
				* E0 -> E1 / E1
				***************************************************************************************************************/
				if (!IsIngameExceptional && !IsExgameExceptional)
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
						try
						{
							ClientApi::BootClient();
						}
						catch (ClientApi::BootFailedException & bfe)
						{
							LOG_TREE(&bfe);

							goto EXIT_ROUTINE;
						}
					}
				}
				else
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

					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::BootFailedException & bfe)
					{
						LOG_TREE(&bfe);

						goto EXIT_ROUTINE;
					}
				}
				
				/***************************************************************************************************************
				* E1 -> E2
				***************************************************************************************************************/
				IsExgameExceptional = false;
				try
				{
					ClientApi::Login(NexonAccountInfo, MapleIdInfo);
				}
				catch (MalrangiException & ce)
				{
					LOG_TREE(&ce);
					IsExgameExceptional = true;

					goto LOGOUT;
				}

				for each (const auto & ServerInfo in MapleIdInfo.VecServer)
				{
#if defined(BUILD_URUSRAID)
					if (ServerInfo.VecCharacter.size() == 0)
#elif defined(BUILD_ZACUMRAID) || defined(BUILD_CALC)
					if (ServerInfo.VecCharacter.size() <= 1)
#endif
					{
						continue;
					}

					Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName;

					/***************************************************************************************************************
					* E2 -> E3
					***************************************************************************************************************/
					try
					{
						ClientApi::SelectServer(ServerInfo, 37);
					}
					catch (MalrangiException & ce)
					{
						LOG_TREE(&ce);
						IsExgameExceptional = true;

						goto LOGOUT;
					}

					ClientApi::SelectCharacter(1);

					/***************************************************************************************************************
					* INGAME APPLICATION
					***************************************************************************************************************/
					IsIngameExceptional = false;
#if defined(BUILD_URUSRAID)
					try
					{
						ClientApi::EnterGame(MapleIdInfo);
					}
					catch (ClientApi::ServerDisconnectedException & sde)
					{
						LOG_TREE(&sde);
						IsIngameExceptional = true;

						goto LOGOUT_DISCONNECTED;
					}
					catch (MalrangiException & ce)
					{
						LOG_TREE(&ce);
						IsIngameExceptional = true;

						goto LOGOUT;
					}

					try
					{
						ClientApi::BreakParty();
						Worker.Play(ServerInfo.VecCharacter[0]);
						
						LOG_TREE(nullptr);
					}
					catch (MalrangiException & ae)
					{
						LOG_TREE(&ae);
						ClientApi::RemoveAllIngameWindows();
					}

					try
					{
						ClientApi::ExitGame();
					}
					catch (MalrangiException & ce)
					{
						LOG_TREE(&ce);
						IsIngameExceptional = true;

						break;
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
						catch (ClientApi::ServerDisconnectedException & sde)
						{
							LOG_TREE(&sde);
							IsIngameExceptional = false;

							goto LOGOUT_DISCONNECTED;
						}
						catch (MalrangiException & ce)
						{
							LOG_TREE(&ce);
							IsIngameExceptional = true;

							goto LOGOUT;
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
						catch (MalrangiException & ae)
						{
							LOG_TREE(&ae);
							ClientApi::RemoveAllIngameWindows();
						}

						try
						{
							ClientApi::ExitGame();
						}
						catch (MalrangiException & ce)
						{
							LOG_TREE(&ce);
							IsIngameExceptional = true;

							break;
						}

						KeybdEvent(VK_RIGHT);
					}
#endif

					/***************************************************************************************************************
					* E3 -> E2
					***************************************************************************************************************/
					try
					{
						ClientApi::ExitCharacterWindow();
					}
					catch (MalrangiException & ce)
					{
						LOG_TREE(&ce);
						IsExgameExceptional = true;

						break;
					}
				}

			/***************************************************************************************************************
			* E2 -> E1 / E0
			***************************************************************************************************************/
			LOGOUT:
				if (!IsIngameExceptional && !IsExgameExceptional)
				{
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
						catch (MalrangiException & ce)
						{
							LOG_TREE(&ce);
							IsExgameExceptional = true;

							goto LOGOUT;
						}
					}
				}
				else
				{
					ClientApi::TerminateClient();
				}
				++SeqMapleId;
				continue;

			LOGOUT_DISCONNECTED:
				if (SeqMapleId % 4 == 3)
				{
					ClientApi::TerminateClient();
				}
				else
				{
					KeybdEvent(VK_RETURN);
				}
				++SeqMapleId;
			}
		}

		WriteLog(LOG_LEVEL::INFO, "ALL ROUTINES HAVE BEEN SUCCESSFULLY COMPLETE.\n");
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

