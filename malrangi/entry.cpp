#include "app.h"

//#define URUS_RAID
//#define ZACUM_RAID
#define ZACUM_CALC

#define USING_IPRENEWER

int 
main(
	int argc,
	char* argv[])
{
	Sleep(2000);

#ifdef URUS_RAID
	UrusRaid Worker;
#endif
#ifdef ZACUM_RAID
	ZacumRaid Worker;
#endif
#ifdef ZACUM_CALC
	ZacumCalc Worker;
#endif
	CONF_INFO* Conf = CONF_INFO::GetInstance();
#ifndef USING_IPRENEWER
	WriteLog(LOG_LEVEL::WARNING, 
		"This program does not support ip renewal. (Flag USING_IPRENEWER is not defined.) \
		The number of accounts written in conf file more than 5 is ignored and this program will be terminated itself.\n");
#endif
	INT SeqMapleId = 0;
	BOOL IsExgameExceptional = FALSE;
	BOOL IsIngameExceptional = FALSE;

#define LEAVE_LOG(cwe, a, b)		WriteLog(LOG_LEVEL::CRITICAL, ((string(cwe.what()) + "(" + a + "/" + b + ")\n").c_str()))
#define LEAVE_LOG(cwe, a, b, c)	WriteLog(LOG_LEVEL::CRITICAL, ((string(cwe.what()) + "(" + a + "/" + b + "/" + c + ")\n").c_str()))
	for each (const auto & NexonAccountInfo in Conf->VecNexonAccount)
	{
		for each (const auto & MapleIdInfo in NexonAccountInfo.VecMapleId)
		{
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
			bool IsPlayValid = false;
			for each(const auto& ServerInfo in MapleIdInfo.VecServer)
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

		LOGIN:
			if (!IsIngameExceptional && !IsExgameExceptional)
			{
				if (SeqMapleId % 4 == 0)
				{
#ifdef USING_IPRENEWER
					RenewIPAddress();
#else 
					if (Conf->VecAccount.size() > 3)
					{
						goto EXIT_ROUTINE;
					}
#endif			
					try
					{
						ClientApi::BootClient();
					}
					catch (ClientApi::ClientException & ce)
					{
						LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

						goto EXIT_ROUTINE;
					}
				}
			}
			else
			{
				if (SeqMapleId % 4 == 0)
				{
#ifdef USING_IPRENEWER
					RenewIPAddress();
#else 
					if (Conf->VecAccount.size() > 3)
					{
						goto EXIT_ROUTINE;
					}
#endif	
				}

				try
				{
					ClientApi::BootClient();
				}
				catch (ClientApi::ClientException & ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					goto EXIT_ROUTINE;
				}
			}

			IsExgameExceptional = FALSE;
			try
			{
				ClientApi::Login(NexonAccountInfo, MapleIdInfo);
			}
			catch (ClientApi::ClientException & ce)
			{
				LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

				IsExgameExceptional = TRUE;
				goto LOGOUT;
			}


			for each (const auto & ServerInfo in MapleIdInfo.VecServer)
			{
				if (0 == ServerInfo.VecCharacter.size())
				{
					continue;
				}

				try
				{
					ClientApi::SelectServer(ServerInfo);
				}
				catch (ClientApi::ClientException& ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					IsExgameExceptional = TRUE;
					goto LOGOUT;
				}

#ifdef URUS_RAID
				ClientApi::SelectCharacter(1);
#endif
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
				ClientApi::SelectCharacter(2);
#endif

				IsIngameExceptional = FALSE;
#ifdef URUS_RAID
				try
				{
					ClientApi::EnterGame(MapleIdInfo);
				}
				catch (ClientApi::ServerDisconnectedException & ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					goto ABNORMAL_LOGOUT;
				}
				catch (ClientApi::ClientException &ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					IsIngameExceptional = TRUE;
					break;
				}
				

				/****************************************************************************
				* APPLICATION
				****************************************************************************/
				try
				{
					ClientApi::BreakParty();
					Worker.Play(NexonAccountInfo);
				}
				catch (BasePlay::AppException & ae)
				{
					LEAVE_LOG(ae, NexonAccountInfo.Id, MapleIdInfo.Id);

					ClientApi::RemoveAllIngameWindows();
				}


				/****************************************************************************
				* I1 -> E3
				****************************************************************************/
				try
				{
					ClientApi::ExitGame();
				}
				catch (ClientApi::ClientException & ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					IsIngameExceptional = TRUE;
					break;
				}

				
#endif
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
				for each (auto & CharacterInfo in ServerInfo.VecCharacter)
				{
					try
					{
						ClientApi::EnterGame(MapleIdInfo);
					}
					catch (ClientApi::ServerDisconnectedException & ce)
					{
						LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

						goto ABNORMAL_LOGOUT;
					}
					catch (ClientApi::ClientException & ce)
					{
						LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

						IsIngameExceptional = TRUE;
						break;
					}

					try
					{
#ifdef ZACUM_RAID
						ClientApi::MakeParty();
						Worker.Play(CharacterInfo, FALSE);
#endif
#ifdef ZACUM_CALC
						Worker.Play(MapleIdInfo);
#endif
					}
					catch (BasePlay::AppException & ae)
					{
						LEAVE_LOG(ae, NexonAccountInfo.Id, MapleIdInfo.Id);

						ClientApi::RemoveAllIngameWindows();
					}


					try
					{
						ClientApi::ExitGame();
					}
					catch (ClientApi::ClientException & ce)
					{
						LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

						IsIngameExceptional = TRUE;
						break;
					}

					KeybdEvent(VK_RIGHT);
				}
#endif

				try
				{
					ClientApi::ExitCharacterWindow();
				}
				catch (ClientApi::ClientException & ce)
				{
					LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

					IsExgameExceptional = TRUE;
					break;
				}
			}


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
					catch (ClientApi::ClientException & ce)
					{
						LEAVE_LOG(ce, NexonAccountInfo.Id, MapleIdInfo.Id);

						IsExgameExceptional = TRUE;
						goto LOGOUT;
					}
				}
			}
			else
			{
				ClientApi::TerminateClient();
			}

			if (FALSE)
			{
			ABNORMAL_LOGOUT:
				KeybdEvent(VK_RETURN);
			}

			++SeqMapleId;
		}
	}

	WriteLog(LOG_LEVEL::INFO, "ALL ROUTINES HAVE BEEN SUCCESSFULLY COMPLETE.\n");
EXIT_ROUTINE:
	Conf->Destroy();

	Sleep(10000000);
	return EXIT_SUCCESS;
}

