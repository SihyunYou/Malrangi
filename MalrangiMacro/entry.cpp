#include "app.h"

#define URUS_RAID
//#define ZACUM_RIAD
//#define ZACUM_CALC

int 
main(
	int argc,
	char* argv[])
{
	Sleep(2000);

#ifdef URUS_RAID
	UrusRaid Head;
#endif
#ifdef ZACUM_RAID
	ZacumRaid Head;
#endif
#ifdef ZACUM_CALC
	ZacumCalc Head;
#endif

	INT SeqMapleId = 0;
	BOOL IsIngameExceptional = FALSE;

	for each (const auto & AccountInfo in Conf.VecAccount)
	{
		for each (const auto & MapleIdInfo in AccountInfo.VecMapleId)
		{
#if defined(ZACUM_RAID) || defined(ZACUM_CALC)
			if (0 == MapleIdInfo.VecCharacter.size())
			{
				continue;
			}
#endif

			/*** 로그인 ***/
			if (!IsIngameExceptional)
			{
				if (SeqMapleId % 4 == 0)
				{
					RenewIPAddress();
					try
					{
						Exc::BootClient();
					}
					catch (CvWrappedException & cwe)
					{
						WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

						return EXIT_FAILURE;
					}
				}
			}
			else
			{
				if (SeqMapleId % 4 == 0)
				{
					RenewIPAddress();
				}

				try
				{
					Exc::BootClient();
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

					return EXIT_FAILURE;
				}
			}

			try
			{
				Exc::LoginInInitialStage(AccountInfo, MapleIdInfo);
			}
			catch (CvWrappedException & cwe)
			{
				WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

				return EXIT_FAILURE;
			}
			Exc::SelectCharacter(1);

			/*** 인게임 ***/
#ifdef URUS_RAID
			try
			{
				Exc::EnterGame(MapleIdInfo);
				Exc::RemoveToolTip();
				Exc::BreakParty();

				Head.Play(AccountInfo);

				Exc::ExitGame();
			}
			catch (CvWrappedException & cwe)
			{
				WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

				IsIngameExceptional = TRUE;
			}
#endif
#ifdef ZACUM_RAID
			for each (auto & CharacterInfo in MapleIdInfo.VecCharacter)
			{
				try
				{
					Exc::EnterGame(MapleIdInfo);
					Exc::RemoveToolTip();
					Exc::MakeParty();

					Head.Play(CharacterInfo, FALSE);

					Exc::ExitGame();
					KeybdEvent(VK_RIGHT);
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}
			}
#endif
#ifdef ZACUM_CALC
			for each (auto & CharacterInfo in MapleIdInfo.VecCharacter)
			{
				try
				{
					Exc::EnterGame(MapleIdInfo);
					Exc::RemoveToolTip();

					Head.Play(MapleIdInfo);

					Exc::ExitGame();
					KeybdEvent(VK_RIGHT);
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

					Exc::ResetIngameWindow();
				}
			}
#endif


			/*** 로그아웃 ***/
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
						WriteLog(LOG_LEVEL::CRITICAL, cwe.what());

						IsIngameExceptional = TRUE;
						goto LOGOUT_EXCEPTION;
					}
				}
			}
			else
			{
			LOGOUT_EXCEPTION:
				Exc::TerminateClient();
			}

			++SeqMapleId;
		}
	}
	Sleep(10000000);
}