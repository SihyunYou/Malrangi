#pragma once
#include "client.h"

class UrusRaid
{
public:
	UrusRaid() : RaidCount(0) {}

public:
	void Play(int SequenceNumber)
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
					KeybdEvent('W', 0x40);
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
		Sleep(0x400);

		Mat Image = Cvw::Capture(ClientApi::RECT_CLIENT4, 1);
		Cvw::Write(SNAP_DIR "urus-raid", to_string(++RaidCount), Image);
		VecImageRaidResult.push_back(Image);


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
			Sleep(0x400);;
		}
	}
	int TotalUp(void)
	{
		for each (const auto & Image in VecImageRaidResult)
		{
			;
		}
	}

private:
	int RaidCount;
	vector<Mat> VecImageRaidResult;
};

