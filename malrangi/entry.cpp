#include "urus.h"
#include "zacum.h"
#include "ipmanage.h"
#include "log.h"
#include "report.h"

//#define URUS_RAID
#define ZACUM_RAID
//#define ZACUM_CALC

#define USING_IPRENEWER

int 
main(
	int argc,
	char* argv[])
{
	Sleep(0x800);

	try
	{
		USERCONF& UserInfo = *(USERCONF::GetInstance());
#if defined(URUS_RAID)
		UrusRaid Worker;
#elif defined(ZACUM_RAID)
		ZacumRaid Worker;
#elif defined(ZACUM_CALC)
		ZacumCalc Worker;
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
				Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id;
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
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
#endif

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
					Uri = NexonAccountInfo.Id + "/" + MapleIdInfo.Id + "/" + ServerInfo.ServerName;
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
					if (0 == ServerInfo.VecCharacter.size())
					{
						continue;
					}
#endif

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

#if defined(URUS_RAID)
					ClientApi::SelectCharacter(1);
#elif defined(ZACUM_RAID) || defined(ZACUM_CALC)
					ClientApi::SelectCharacter(2);
#endif

					/***************************************************************************************************************
					* INGAME APPLICATION
					***************************************************************************************************************/
					IsIngameExceptional = false;
#if defined(URUS_RAID)
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
						for (int sn = 0; sn < 3; sn++)
						{
							Worker.Play(sn);
						}
						
						LOG_TREE(nullptr);
					}
					catch (MalrangiException & ae)
					{
						LOG_TREE(&ae);
						Cvw::Save(Cvw::Capture(ClientApi::RECT_CLIENT4), "urus-raid");
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
#elif defined(ZACUM_RAID) || defined(ZACUM_CALC)
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
#ifdef ZACUM_RAID
							ClientApi::MakeParty();
							Worker.Play(CharacterInfo, FALSE);
#else
							Worker.Play(MapleIdInfo);
#endif
						}
						catch (MalrangiException & ae)
						{
							LOG_TREE(&ae);

							static int ExceptionSequence = 0;
#ifdef ZACUM_RAID
#define FILENAME "zacum-raid"
#else
#define FILENAME "zacum-calc"
#endif
							Cvw::Write(SNAP_DIR FILENAME, to_string(++ExceptionSequence), Cvw::Capture(ClientApi::RECT_CLIENT4, 1));
#undef FILENAME
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
	catch (MalrangiException & cwe)
	{
		WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
	}

	Sleep(10000000);
	return EXIT_SUCCESS;
}

