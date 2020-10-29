#pragma once
#include "client_api.h"

class UrusRaid
{
public:
	UrusRaid() : SeqPlay(1) {}

protected:
	int SeqPlay;

	enum class URUSRAID_EXCEPTION_CODE
	{
		PARTY_UNBREAKABLE,
		BUTTON_READY_NOT_FOUND,
		PLAY_SEQUENCE_OVER,
		BATTLE_ENTRY_TIMEOUT,
		BATTLE_TIMEOUT,
		EXIT_TO_U3_FAILED,
		NPC_MASHUR_NOT_FOUND
	};
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo,
		const bool IsQuickCompletionMode)
	{
		static const Mat TargetImageButtonReady(Read(TARGET_DIR "button//ready.jpg"));
		static const Mat TargetImageButtonEnter(Read(TARGET_DIR "button//enter.jpg"));
		static const Mat TargetImagePicUrus(Read(TARGET_DIR "pic//urus.jpg"));
		static const Mat TargetImageNpcMashur(Read(TARGET_DIR "npc//mashur.jpg"));
		static const Mat TargetImageMapU3 = Read(TARGET_DIR "map//u3.jpg");
		static const Mat TargetImageWindowUrusCompleted = Read(TARGET_DIR "window//urus_completed.jpg");
		static const Mat TargetImageWindowUrusPartyNotBroken = Read(TARGET_DIR "window//urus_party_not_broken.jpg");
		VALLOC MatchInfo;

		/*** 전투 전 ***/
		if (1 == SeqPlay)
		{
			ClientApi::OpererParty(2);
		}
		else if (3 < SeqPlay)
		{
			return;
		}

		bool IsRetry = false;
	__RETRY__:
		KeybdEventContinued(VK_LEFT, 2800);
		MouseEvent({ 1140, 630 }, CLIC_GAUCHE);
		MouseEvent({ 440, 710 }, CLIC_GAUCHE);
		MouseEvent({ 487, 771 }, CLIC_GAUCHE);
		MouseEvent({ 820, 780 }, CLIC_GAUCHE);
		MouseEvent({ 820, 780 }, CLIC_GAUCHE);

		Mat SourceImage = SourceImageClient4;
		if (MatchTemplate(SourceImage, TargetImageButtonReady, &MatchInfo))
		{
			MouseEvent(MatchInfo.Location, CLIC_GAUCHE);
		}
		else
		{
			if (MatchTemplate(SourceImage, TargetImageWindowUrusPartyNotBroken, &MatchInfo))
			{
				if (IsRetry)
				{
					throw URUSRAID_EXCEPTION_CODE::PARTY_UNBREAKABLE;
				}
				else
				{
					KeybdEvent(VK_ESCAPE);
					ClientApi::OpererParty(2);

					IsRetry = true;
					goto __RETRY__;
				}
			}

			if (MatchTemplate(SourceImage, TargetImageWindowUrusCompleted, &MatchInfo))
			{
				throw URUSRAID_EXCEPTION_CODE::PLAY_SEQUENCE_OVER;
			}
			else
			{
				throw URUSRAID_EXCEPTION_CODE::BUTTON_READY_NOT_FOUND;
			}
		}

#define Operate(d)		KeybdEventContinued(d, 80);	\
						KeybdEvent('E', 20);
		RECT NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.left += 100;
		NewRectClient.right -= 700;
		NewRectClient.bottom -= 500;
		int p = 0;

		if (!DoUntilMatchingTemplate(
			NewRectClient,
			TargetImagePicUrus,
			[&p]()
			{
				if ((++p % 0x20) == 0)
				{
					KeybdEvent(VK_RETURN, 0);
				}
			},
			seconds(600),
				1))
		{
			throw URUSRAID_EXCEPTION_CODE::BATTLE_ENTRY_TIMEOUT;
		}


			/*** 전투 중 ***/
			const auto StartTime = system_clock::now();
			BYTE CurrentDir, LastDir = NULL;
			int CountNullOfCurrentDir, CountInitialOperation = 0;
			int CountMaximumNullOfCurrentDir = 24;
			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.left += 900;
			NewRectClient.top += 530;

			MouseEvent({ 897, 159 }, CLIC_GAUCHE, 0);
			if (DoUntilMatchingTemplate(
				NewRectClient,
				TargetImageButtonEnter,
				[&](void)
				{
					if (duration_cast<seconds>(system_clock::now() - StartTime) > seconds(240))
					{
						Sleep(0x100);
						return;
					}

					int Score = 0;
					Mat MaskImage;
					RECT NewRect = ClientApi::RECT_CLIENT4;
					NewRect.top += 156;
					NewRect.bottom -= 100;

					InRange(Capture(NewRect, 1), Scalar(235, 30, 195), Scalar(255, 60, 205), MaskImage);

					for (int y = 0; y < MaskImage.rows; ++y)
					{
						for (int x = 0; x < MaskImage.cols; ++x)
						{
							if (MaskImage.at<BYTE>(y, x) == 255)
							{
								(x < 692) ? --Score : ++Score;
							}
						}
					}

					if (Score >= -2 && Score <= 2)
					{
						CurrentDir = NULL;
					}
					else
					{
						LastDir = CurrentDir = (Score < 0) ? VK_LEFT : VK_RIGHT;
					}

					if (CountNullOfCurrentDir > CountMaximumNullOfCurrentDir)
					{
						LastDir = (LastDir == VK_LEFT) ? VK_RIGHT : VK_LEFT;
						CountNullOfCurrentDir = 0;
						CountMaximumNullOfCurrentDir += 12;
					}

					if ((NULL == LastDir && NULL == CurrentDir) || CountInitialOperation < 24)
					{
						Operate(VK_LEFT);
						Operate(VK_RIGHT);
						++CountNullOfCurrentDir;
						++CountInitialOperation;
					}
					else if (NULL != LastDir && NULL == CurrentDir)
					{
						Operate(LastDir);
						++CountNullOfCurrentDir;
					}
					else // NULL != CurrentDir
					{
						Operate(CurrentDir);
						CountNullOfCurrentDir = 0;
					}
				},
				seconds(1000),
					0))
			{
				Sleep(2000);
			}
			else
			{
				throw URUSRAID_EXCEPTION_CODE::BATTLE_TIMEOUT;
			}
#undef Operate


			/*** 전투 후 ***/
			if (!IsQuickCompletionMode || (3 != SeqPlay))
			{
				static int SeqSuccess = 0;
				Write(SNAP_DIR "urus//" + to_string(++SeqSuccess) + ".jpg", SourceImageClient4Colored);
				MouseEvent({ 1259, 694 }, CLIC_GAUCHE);

				if (WaitUntilMatchingTemplate(
					ClientApi::RECT_CLIENT4,
					TargetImageMapU3,
					seconds(60)))
				{
					Sleep(1000);
				}
				else
				{
					throw URUSRAID_EXCEPTION_CODE::EXIT_TO_U3_FAILED;
				}

				if (SeqPlay <= 2)
				{
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 4000);
					if (WaitUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMashur, seconds(60)))
					{
						Sleep(1000);
					}
					else
					{
						throw URUSRAID_EXCEPTION_CODE::NPC_MASHUR_NOT_FOUND;
					}
				}
			}
	}
};