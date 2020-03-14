#pragma once
#include "bsp.h"
#include "bridge.h"

class UrusRaid : public BasePlay
{
public:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static Mat TargetImageBackgroundUrus(Cvw::Read(TARGET_DIR "background_urus.jpg"));
		static Mat TargetImageNpcMashur(Cvw::Read(TARGET_DIR "npc_mashur.jpg"));
		static Mat TargetImageButtonReady(Cvw::Read(TARGET_DIR "button_ready.jpg"));
		static Mat TargetImageUnitInUrusBattleMap(Cvw::Read(TARGET_DIR "unit_in_urus_battle_map.jpg"));
		static Mat TargetImageButtonEnter(Cvw::Read(TARGET_DIR "button_enter.jpg"));

		for (int sn = 0; sn < 3; sn++)
		{
			bool IsFailedAgain = false;

			/*** 우르스 매칭 준비 ***/
		BATTLE_ENTRY:
			MouseEvent(sn != 0 ? 1140 : 892, 600, LEFT_CLICK);
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
#define Operate(d)		KeybdEventContinued(d, 0x40);	\
							KeybdEvent('E', 0x20);

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
						MouseEvent(640, 429, LEFT_CLICK, 0x200);
					},
					seconds(1800),
						0);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("BattleEntryTimeoutException");
			}


			/*** 전투 중 ***/
			MouseEvent(897, 133, LEFT_CLICK, 0);

			const auto StartTime = system_clock::now();
			BYTE CurrentDir, LastDir = NULL;
			int CountNullOfCurrentDir, CountInitialOperation = 0;
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

						try
						{
							Cvw::InRange(Cvw::Capture(NewRect, 1), Scalar(242, 30, 195), Scalar(255, 60, 205), MaskImage);
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
									(x < 692) ? --Score : ++Score;
								}
							}
						}

						if (Score >= -2 && Score <= 2) CurrentDir = NULL;
						else LastDir = CurrentDir = (Score < 0) ? VK_LEFT : VK_RIGHT;

						if (CountNullOfCurrentDir > 160 && CurrentDir == NULL)
						{
							LastDir = (LastDir == VK_LEFT) ? VK_RIGHT : VK_LEFT;
							CountNullOfCurrentDir = 0;
						}


						if ((NULL == LastDir && NULL == CurrentDir) || CountInitialOperation < 24)
						{
							Operate(VK_LEFT);
							Operate(VK_RIGHT);
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
						0);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("BattleTimeoutException");
			}

			Sleep(1600);
			
			static int SeqUrus = 0;
			Cvw::Write(SNAP_DIR + INT_TO_PNG(++SeqUrus), Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));
#undef Operate

			/*** 퇴장 ***/
			MouseEvent(1259, 668, LEFT_CLICK);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, seconds(45), 0x100, 0.99);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("UrusExitException");
			}
			Sleep(0x400);

			if (2 > sn)
			{
				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 4000);
				try
				{
					Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMashur, NONWORK, seconds(30));
				}
				catch (MatchFailedException)
				{
					throw MalrangiException("NpcMashurNotFound");
				}
				Sleep(0x400);
			}
		}
	}
};

class ZacumRaid : public BasePlay
{
public:
	void PlayFromZacum(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		MoveFromZ2ToZ1();
		MoveFromZ1ToZ2(1);
		MoveFromZ2ToZ3();
		Play(CharacterInfo);
	}
	void PlayFromUrus(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		MoveFromUrusToElnas();
		MoveFromElnasToOffice();
		MoveFromOfficeToZ1();
		MoveFromZ1ToZ2(1);
		MoveFromZ2ToZ3();
		Play(CharacterInfo);
		MoveFromZ3ToZ2();
		MoveFromZ2ToZ1();
		MoveFromZ1ToElnas();
		MoveFromElnasToUrus();
	}

private:
	void Play(
		const USERCONF::CHARACTER_INFO& CharacterInfo)
	{
		static const Mat TargetImageEyeOfFire = Cvw::Read(TARGET_DIR "eye_of_fire.jpg");
		static const Mat TargetImageInventoryBar = Cvw::Read(TARGET_DIR "inventory_bar.jpg");
		static const Mat TargetImageButtonExpandingInventory = Cvw::Read(TARGET_DIR "button_expanding_inventory.jpg");
		static const Mat TargetImageCrystalOfBoss = Cvw::Read(TARGET_DIR "crystal_of_boss.jpg");
		
		/*** 자쿰의 제단 입장 ***/
		KeybdEventContinued(VK_RIGHT, 1000);
		if (CharacterInfo.RequiredBuf1 != NULL)
		{
			KeybdEvent(CharacterInfo.RequiredBuf1, 1000);
		}


		/*** 불의 눈 던지기 ***/
		USERCONF::KEYSET_INFO& KeysetInfo = USERCONF::GetInstance()->VirtualKeyset;
		KeybdEvent(KeysetInfo.Inventory);
		try
		{
			Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
			MouseEvent(25, 50, LEFT_CLICK);
		}
		catch (MatchFailedException)
		{
			try
			{
				Mat SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageInventoryBar, LEFT_CLICK, { 40 * 2, 40 }, 600);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageButtonExpandingInventory, LEFT_CLICK, { 6, 6 }, 600);

				SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("ThrowEyeOfFireFailedException");
			}

			MouseEvent(25, 50, LEFT_CLICK);
		}
		KeybdEvent(KeysetInfo.Inventory, 3600);


		/*** 쿰돌이 ***/
		if (CharacterInfo.RequiredBuf2 != NULL)
		{
			KeybdEvent(CharacterInfo.RequiredBuf2, 1000);
		}

		try
		{
			Cvw::DoUntilMatchingTemplate(
				ClientApi::RECT_CLIENT4,
				TargetImageCrystalOfBoss,
				[CharacterInfo]()
				{
					KeybdEventDown(CharacterInfo.Skill);
					MouseEvent(800, 294, LEFT_CLICK, 0);
				},
				seconds(120),
					50);
			KeybdEventUp(CharacterInfo.Skill);
		}
		catch (MatchFailedException)
		{
			;
		}


		/*** 줍기 ***/
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2000);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 5000);
	}
};


class RootAbyssRaid
{

};