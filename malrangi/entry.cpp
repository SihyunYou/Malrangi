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
		The number of accounts written in conf file more than 5 is ignored and this program will be terminated.");
#endif
	INT SeqMapleId = 0;
	BOOL IsIngameExceptional = FALSE;

#define LEAVE_LOG(cwe, a, b)		WriteLog(LOG_LEVEL::CRITICAL, ((string(cwe.what()) + "(" + a + "/" + b + ")\n").c_str()))
#define LEAVE_LOG(cwe, a, b, c)	WriteLog(LOG_LEVEL::CRITICAL, ((string(cwe.what()) + "(" + a + "/" + b + "/" + c + ")\n").c_str()))
	for each (const auto & AccountInfo in Conf->VecAccount)
	{
		for each (const auto & MapleIdInfo in AccountInfo.VecMapleId)
		{
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
			if (0 == MapleIdInfo.VecCharacterString.size())
			{
				continue;
			}
#endif

			/*** LOGIN ***/
			if (!IsIngameExceptional)
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
						Exc::BootClient();
					}
					catch (CvWrappedException & cwe)
					{
						LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

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
					Exc::BootClient();
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

					goto EXIT_ROUTINE;
				}
			}

			try
			{
				Exc::LoginInInitialStage(AccountInfo, MapleIdInfo);
			}
			catch (CvWrappedException & cwe)
			{
				LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

				goto LOGOUT;
			}
#ifdef URUS_RAID
			Exc::SelectCharacter(1);
#endif
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
			Exc::SelectCharacter(2);
#endif


			/*** INGAME ***/
			IsIngameExceptional = FALSE;
#ifdef URUS_RAID
			for (int CountPlay = 0; CountPlay < 3; CountPlay++)
			{
				try
				{
					Exc::EnterGame(MapleIdInfo);
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

					IsIngameExceptional = TRUE;
					break;
				}
				
				try
				{
					if (0 == CountPlay)
					{
						Exc::BreakParty();
					}
					Worker.Play(AccountInfo);
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

					Exc::RemoveAllIngameWindow();
				}

				try
				{
					Exc::ExitGame();
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

					IsIngameExceptional = TRUE;
					break;
				}
			}
#endif
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
			for each (auto & CharacterString in MapleIdInfo.VecCharacterString)
			{
				try
				{
					Exc::EnterGame(MapleIdInfo);
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id, CharacterString);

					IsIngameExceptional = TRUE;
					break;
				}

				try
				{
#ifdef ZACUM_RAID
					Exc::MakeParty();
					Worker.Play(Conf->MapCharacterInfo[CharacterString], FALSE);
#endif
#ifdef ZACUM_CALC
					Worker.Play(MapleIdInfo);
#endif
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id, CharacterString);

					Exc::RemoveAllIngameWindow();
				}

				try
				{
					Exc::ExitGame();
				}
				catch (CvWrappedException & cwe)
				{
					LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id, CharacterString);

					IsIngameExceptional = TRUE;
					break;
				}

				KeybdEvent(VK_RIGHT);
			}
#endif


			/*** LOGOUT / TERMINATION ***/
		LOGOUT:
			if (!IsIngameExceptional)
			{
				if (SeqMapleId % 4 == 3)
				{
					Exc::TerminateClient();
				}
				else
				{
					try
					{
						Exc::Logout();
					}
					catch (CvWrappedException & cwe)
					{
						LEAVE_LOG(cwe, AccountInfo.Id, MapleIdInfo.Id);

						IsIngameExceptional = TRUE;
						goto LOGOUT;
					}
				}
			}
			else
			{
				Exc::TerminateClient();
			}

			++SeqMapleId;
		}
	}
	WriteLog(LOG_LEVEL::INFO, "ALL ROUTINES HAVE BEEN SUCCESSFULLY COMPLETE.\n")
	
	EXIT_ROUTINE:
	Conf->Destroy();

	Sleep(10000000);
	return EXIT_SUCCESS;
}

