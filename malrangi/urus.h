#pragma once
#include "boss.h"
#include "tesseract-binary-wrap.h"

#undef SNAP_DIR
#define SNAP_DIR "snap//urus-raid//"

class UrusRaid : public Boss
{
public:
	void Play(int SequenceNumber)
	{
		try
		{
			static Mat TargetImageBackgroundUrus(Cvw::Read(TARGET_DIR "background_urus.jpg"));
			static Mat TargetImageNpcMashur(Cvw::Read(TARGET_DIR "npc_mashur.jpg"));
			static Mat TargetImageButtonReady(Cvw::Read(TARGET_DIR "button_ready.jpg"));
			static Mat TargetImageUnitInUrusBattleMap(Cvw::Read(TARGET_DIR "unit_in_urus_battle_map.jpg"));
			static Mat TargetImageButtonEnter(Cvw::Read(TARGET_DIR "button_enter.jpg"));
			bool IsFailedAgain = false;


			/*** 우르스 매칭 준비 ***/
		BATTLE_ENTRY:
			MouseEvent(SequenceNumber != 0 ? 1140 : 892, 600, LEFT_CLICK);
			MouseEvent(440, 684, LEFT_CLICK);
			MouseEvent(487, 745, LEFT_CLICK);
			MouseEvent(820, 754, LEFT_CLICK);

			try
			{
				Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageButtonReady, LEFT_CLICK);
			}
			catch (MatchFailedException)
			{
				if (!IsFailedAgain)
				{
					ClientApi::RemoveAllIngameWindows();

					IsFailedAgain = true;
					goto BATTLE_ENTRY;
				}
				throw MalrangiException("ButtonReadyNotFound");
			}
			IsFailedAgain = false;


			/*** 우르스 매칭 ***/
			RECT NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.left += 100;
			NewRectClient.right -= 700;
			NewRectClient.bottom -= 500;
			try
			{
				Cvw::DoUntilMatchingTemplate(
					NewRectClient,
					TargetImageUnitInUrusBattleMap,
					[](void) -> void
					{
						MouseEvent(640, 429, LEFT_CLICK, 0x40);
						KeybdEvent('E', 0x40);
					},
					1800000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("BattleEntryTimeoutException");
			}


			/*** 전투 중 ***/
			MouseEvent(897, 133, LEFT_CLICK);

			BYTE CurrentDir, LastDir = NULL;
			INT CountNullOfCurrentDir = 0;
			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.left += 900;
			NewRectClient.top += 530;

			try
			{
				Cvw::DoUntilMatchingTemplate(
					NewRectClient,
					TargetImageButtonEnter,
					[&](void) -> void
					{
						INT Score = 0;
						RECT NewRect = ClientApi::RECT_CLIENT4;
						NewRect.top += 156;
						NewRect.bottom -= 100;
						Mat SourceImage = Cvw::Capture(NewRect, 1);
						Mat MaskImage;

						try
						{
							Cvw::InRange(SourceImage, Scalar(242, 30, 195), Scalar(255, 60, 205), MaskImage);
						}
						catch (IntegerDivisionByZeroException)
						{
							throw;
						}

						for (int y = 0; y < MaskImage.rows; ++y)
						{
							for (int x = 0; x < MaskImage.cols; ++x)
							{
								if (MaskImage.at<BYTE>(y, x) == 255)
								{
									if (x < 600)
									{
										--Score;
									}
									else if (x > 764)
									{
										++Score;
									}
								}
							}
						}

						if (Score >= -2 && Score <= 2) CurrentDir = NULL;
						else LastDir = CurrentDir = (Score < 0) ? VK_LEFT : VK_RIGHT;

						if (CountNullOfCurrentDir > 20 && CurrentDir == NULL)
						{
							LastDir = (LastDir == VK_LEFT) ? VK_RIGHT : VK_LEFT;
							CountNullOfCurrentDir = 0;
						}

						if (NULL == LastDir && NULL == CurrentDir)
						{
							KeybdEventContinued(VK_LEFT, 250);
							KeybdEvent('E', 250);
							KeybdEventContinued(VK_RIGHT, 250);
							KeybdEvent('E', 250);
						}
						else if (NULL != LastDir && NULL == CurrentDir)
						{
							KeybdEventContinued(LastDir, 250);
							KeybdEvent('E', 250);

							++CountNullOfCurrentDir;
						}
						else // NULL != CurrentDir
						{
							KeybdEventContinued(CurrentDir, 250);
							KeybdEvent('E', 250);

							CountNullOfCurrentDir = 0;
						}
					},
					1000000,
						0);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("BattleTimeoutException");
			}
			Sleep(1250);
			Cvw::Write(SNAP_DIR + INT_TO_PNG(++PlayCount), Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));


			/*** 퇴장 ***/
			MouseEvent(1259, 668, LEFT_CLICK);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 45000, 0x100, 0.99);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("UrusExitException");
			}
			Sleep(0x400);

			if (2 > SequenceNumber)
			{
				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 4000);
				try
				{
					Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMashur, NONWORK, 30000);
				}
				catch (MatchFailedException)
				{
					throw MalrangiException("NpcMashurNotFound");
				}
				Sleep(0x400);
			}
		}
		catch (MalrangiException)
		{
			Cvw::Write(SNAP_DIR "exception" + INT_TO_PNG(++ExceptionCount), Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));
			throw;
		}
	}
	int TotalUp(void)
	{
		int Sum = 0;
		auto StrProc = [](const string& s) -> decltype(auto)
		{
			string _s("");
			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] >= 48 && s[i] <= 57)
				{
					_s += s[i];
				}
			}

			return _s;
		};

		for(int i = 1; i <= PlayCount; i++)
		{
#define TMP_FILENAME "image.png"
#define FILEPATH (TMP_DIR TMP_FILENAME)
			Cvw::Write(FILEPATH, Cvw::Read(SNAP_DIR + INT_TO_PNG(i))({ 980, 534, 168, 30 }));
			Sum += stoi(Tsw::TranslateToString(FILEPATH, StrProc));

#undef FILEPATH
#undef TMP_FILENAME
		}

		return Sum;
	}
};

#undef SNAP_DIR
#define SNAP_DIR "snap//"