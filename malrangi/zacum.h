#pragma once
#include "boss.h"

class Zacum : public Boss
{
protected:
	void MoveFromZ1ToZ2(int Level)
	{
		KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1200);
		for (int i = 0; i < (int)Level; i++)
		{
			KeybdEvent(VK_DOWN, 600);
		}
		KeybdEvent(VK_RETURN);

		try
		{
			Cvw::DoUntilMatchingTemplate(RECT{ 950, 500, 1050, 600 }, TargetImageNpcAdobis, NONWORK, 10000);
		}
		catch (MatchFailedException)
		{
			throw MalrangiException("MoveFromZ1ToZ2FailedException");
		}
	}
	void MoveFromZ2ToZ1(void)
	{
		static Mat TargetImage = Cvw::Read(TARGET_DIR "sclupture_z1.jpg");
		bool IsFailedAgain = false;

	MOVE:
		KeybdEventContinuedWithSubKey(!IsFailedAgain ? VK_LEFT : VK_RIGHT, VK_UP, 1500);
		try
		{
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImage, NONWORK, 10000);
		}
		catch (MatchFailedException)
		{
			if (!IsFailedAgain)
			{
				IsFailedAgain = true;
				goto MOVE;
			}
			throw MalrangiException("MoveFromZ2ToZ1FailedException");
		}
		Sleep(0x400);
	}
	void GetEyeOfFire(void)
	{
		bool IsFailedAgain = false;

	TALK:
		MouseEvent(820, 563, LEFT_CLICK);
		KeybdEvent(VK_DOWN, 400);
		KeybdEvent(VK_DOWN, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_DOWN, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_RETURN, 400);
		KeybdEvent(VK_RETURN, 400);

		try
		{
			Cvw::UnmatchTemplate(Cvw::Capture({ 440, 320, 550, 410 }), TargetImageNpcAdobis, 0.8);
		}
		catch (MatchSuccessedException)
		{
			if (!IsFailedAgain)
			{
				ClientApi::RemoveAllIngameWindows();

				IsFailedAgain = true;
				goto TALK;
			}
			throw MalrangiException("GetEyeOfFireFailedException");
		}
	}

protected:
	static const Mat TargetImageNpcAdobis;
};
const Mat Zacum::TargetImageNpcAdobis = Cvw::Read(TARGET_DIR "npc_adobis.jpg");


#undef SNAP_DIR
#define SNAP_DIR "snap//zacum-raid//"

class ZacumRaid : public Zacum
{
private:
	void MoveFromZ2ToZ3(void)
	{
		static const Mat TargetImage = Cvw::Read(TARGET_DIR "unit_z3.jpg");

		MouseEvent(1000, 544, LEFT_CLICK, 600);
		KeybdEvent(VK_RETURN);
		try
		{
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImage, NONWORK, 10000);
		}
		catch (MatchFailedException)
		{
			MouseEvent(1000, 544, LEFT_CLICK, 600);
			KeybdEvent(VK_RETURN);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImage, NONWORK, 10000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("MoveFromZ2ToZ3FailedException");
			}
		}
		Sleep(0x200);
	}

public:
	void Play(
		CONST USERCONF::CHARACTER_INFO& CharacterSpecialty,
		BOOL IsReady)
	{
		try
		{
			static const Mat TargetImageEyeOfFire = Cvw::Read(TARGET_DIR "eye_of_fire.jpg");
			static const Mat TargetImageInventoryBar = Cvw::Read(TARGET_DIR "inventory_bar.jpg");
			static const Mat TargetImageButtonExpandingInventory = Cvw::Read(TARGET_DIR "button_expanding_inventory.jpg");
			static const Mat TargetImageCrystalOfBoss = Cvw::Read(TARGET_DIR "crystal_of_boss.jpg");


			/*** 자쿰의 제단 입장 전 준비 ***/
			if (!IsReady)
			{
				MoveFromZ2ToZ1();
				GetEyeOfFire();
				MoveFromZ1ToZ2(1);
			}
			MoveFromZ2ToZ3();


			/*** 자쿰의 제단 입장 ***/
			KeybdEventContinued(VK_RIGHT, 1000);
			if (CharacterSpecialty.RequiredBuf1 != NULL)
			{
				KeybdEvent(CharacterSpecialty.RequiredBuf1, 1000);
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
			if (CharacterSpecialty.RequiredBuf2 != NULL)
			{
				KeybdEvent(CharacterSpecialty.RequiredBuf2, 1000);
			}

			try
			{
				Cvw::DoUntilMatchingTemplate(
					ClientApi::RECT_CLIENT4,
					TargetImageCrystalOfBoss,
					[CharacterSpecialty]()
					{
						KeybdEventDown(CharacterSpecialty.Skill);
						MouseEvent(800, 294, LEFT_CLICK, 0);
					},
					45000,
						50);
				KeybdEventUp(CharacterSpecialty.Skill);
			}
			catch (MatchFailedException)
			{
				;
			}


			/*** 줍기 ***/
			KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2000);
			KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 5000);
		}
		catch (MalrangiException)
		{
			Cvw::Write(SNAP_DIR "exception" + INT_TO_PNG(++ExceptionCount), Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));
			throw;
		}
	}
};

#undef SNAP_DIR
#define SNAP_DIR "snap//zacum-calc//"

class ZacumCalc : public Zacum
{
public:
	void Play(
		CONST USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		try
		{
			static const Mat TargetImageButtonMeisterVill(Cvw::Read(TARGET_DIR "button_meistervill.jpg"));
			static const Mat TargetImageMinimapMarkMeisterVill(Cvw::Read(TARGET_DIR "minimap_mark_meistervill.jpg"));
			static const Mat TargetImageItemCrystal(Cvw::Read(TARGET_DIR "item_crystal_of_boss.jpg"));
			static const Mat TargetImageNpcMsBrainy(Cvw::Read(TARGET_DIR "npc_msbrainy.jpg"));
			static const Mat TargetImageItem100lv(Cvw::Read(TARGET_DIR "item_100lv.jpg"));
			static const Mat TargetImageItem110lv(Cvw::Read(TARGET_DIR "item_110lv.jpg"));
			static const Mat TargetImageItemCube(Cvw::Read(TARGET_DIR "item_cube.jpg"));

			MoveFromZ2ToZ1();
			GetEyeOfFire();
			MoveFromZ1ToZ2(2);

			/*** `자쿰의 제단 입구(카오스) -> `마이스터 빌` 이동 ***/
			USERCONF::KEYSET_INFO& KeysetInfo = USERCONF::GetInstance()->VirtualKeyset;
			KeybdEvent(KeysetInfo.SpecialTechnology);
			try
			{
				Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageButtonMeisterVill, LEFT_CLICK, { 5, 2 });
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("ClickButtonMeisterVillFailedException");
			}

			KeybdEvent(VK_RETURN);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkMeisterVill, NONWORK, 30000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("MoveZ2ToMFailedException");
			}

			Sleep(0x400);
			KeybdEvent(KeysetInfo.SpecialTechnology);


			/*** `마이스터 빌` -> `자유시장` 이동 ***/
			ClientApi::DownJump(1500);
			ClientApi::DownJump(1500);
			KeybdEventContinued(VK_LEFT, 1000);
			Sleep(0x200);
			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2600);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMsBrainy, NONWORK, 15000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("MoveMToFFailedException");
			}


			/*** 아이템 팔기(콜렉터) ***/
			MouseEvent(980, 334, LEFT_CLICK);

			// 장비창
			for (int i = 0; i < 40; i++)
			{
				MouseEvent(735, 279, RIGHT_CLICK, 300);
			}

			// 기타창
			INT CountCrystal;
			RECT NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.top += 220;
			NewRectClient.left += 710;

			MouseEvent(818, 244, LEFT_CLICK);
			for (CountCrystal = 0; CountCrystal < 30; CountCrystal++)
			{
				try
				{
					Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItemCrystal, RIGHT_CLICK, { NewRectClient.left + 80, NewRectClient.top });
				}
				catch (MatchFailedException)
				{
					break;
				}

				KeybdEvent(VK_RETURN);
			}
			KeybdEvent(VK_ESCAPE);


			/*** 창고에 아이템 넣기  ***/
			if (0 < CountCrystal)
			{
				MouseEvent(310, 334, LEFT_CLICK);
				ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);

				// 장비템 넣기(아쿠아틱 + 응축)
				NewRectClient = ClientApi::RECT_CLIENT4;
				NewRectClient.top += 270;
				NewRectClient.left += 690;

				for (int i = 0; i < 8; i++)
				{
					MouseEvent(0, 750, CURSOR_MOVE);
					try
					{
						Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItem100lv, DLEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					}
					catch (MatchFailedException)
					{
						break;
					}

					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}

				for (int i = 0; i < 8; i++)
				{
					MouseEvent(0, 750, CURSOR_MOVE);
					try
					{
						Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItem110lv, DLEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					}
					catch (MatchFailedException)
					{
						break;
					}

					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}

				// 소비템 넣기(수큐)
				MouseEvent(740, 264, LEFT_CLICK);
				for (int i = 0; i < 3; i++)
				{
					MouseEvent(0, 750, CURSOR_MOVE);
					try
					{
						Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItemCube, DLEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					}
					catch (MatchFailedException)
					{
						break;
					}

					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}

				// 돈 넣기
				MouseEvent(715, 574, LEFT_CLICK);
				KeybdEvent(VK_RETURN);

				// 창고 창 닫기
				MouseEvent(640, 204, LEFT_CLICK);
			}


			/*** `자유시장` -> `마이스터 빌` -> `자쿰의 제단 입구(카오스)` 이동 ***/
		RETURN_TO_Z2:
			KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkMeisterVill, NONWORK, 20000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("MoveFToMFailedException");
			}
			Sleep(0x800);

			static bool IsForward = true;
			ClientApi::MoveServer(IsForward = (IsForward == true) ? false : true);
			try
			{
				Cvw::DoUntilMatchingTemplate(
					RECT{ 950, 500, 1050, 600 },
					TargetImageNpcAdobis,
					[](void) -> void
					{
						KeybdEvent(VK_UP, 0x80);
					},
					20000);
			}
			catch (MatchFailedException)
			{
				throw MalrangiException("MoveServerFailedException");
			}
			Sleep(0x400);
		}
		catch (MalrangiException)
		{
			Cvw::Write(SNAP_DIR "exception" + INT_TO_PNG(++ExceptionCount), Cvw::Capture(ClientApi::RECT_CLIENT4, IMREAD_COLOR));
		}
	}
};

#undef SNAP_DIR
#define SNAP_DIR "snap//"

class ZacumAndUrus : public Zacum
{
public:
	ZacumAndUrus() :
		TargetImagePictureUrusDimensionalMirror(Cvw::Read(TARGET_DIR "picture_urus_dimensional_mirror.jpg")),
		TargetImageMinimapMarkElnas(Cvw::Read(TARGET_DIR "minimap_mark_elnas.jpg")),
		TargetImageBackgroundUrus(Cvw::Read(TARGET_DIR "background_urus.jpg")),
		TargetImageNpcRene(Cvw::Read(TARGET_DIR "npc_rene.jpg"))
	{
		;
	}

public:
	void MoveFromZ2ToUrus(void)
	{
		MoveFromZ2ToZ1();

		MouseEvent(820, 544, LEFT_CLICK);
		KeybdEvent(VK_DOWN);
		KeybdEvent(VK_DOWN);
		KeybdEvent(VK_DOWN);
		KeybdEvent(VK_RETURN);
		KeybdEvent(VK_RETURN);
		KeybdEvent(VK_RETURN);

		try
		{
			Cvw::DoUntilMatchingTemplate({ 0, 0, 64, 64 }, TargetImageMinimapMarkElnas, NONWORK, 30000);
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		Sleep(0x400);
		MouseEvent(58, 168, LEFT_CLICK);
		MouseEvent(566, 376, LEFT_CLICK);
		Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImagePictureUrusDimensionalMirror, LEFT_CLICK);
		MouseEvent(690, 596, LEFT_CLICK);

		try
		{
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 30000);
		}
		catch (MatchFailedException)
		{
			throw;
		}

		Sleep(0x400);
	}
	void MoveFromUrusToZ2(void)
	{
		KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2000);
		Sleep(3000);

		ClientApi::MinimapRecognizer Recognizer({ 10, 64, 260, 160 }); // `엘나스` 미니맵
	RETRY:
		Recognizer.MoveToRelativeCriteria(185);
		Sleep(2000);
		ClientApi::Jump(ClientApi::JUMP_T::DEMON);
		ClientApi::DownJump(1000);
		KeybdEvent(VK_UP, 3000);

		try
		{
			Cvw::DoUntilMatchingTemplate({ 500, 0, 650, 150 }, TargetImageNpcRene, NONWORK, 5000);
		}
		catch (MatchFailedException)
		{
			KeybdEventContinued(VK_LEFT, 7000);
			goto RETRY;
		}

		MouseEvent(482, 566, LEFT_CLICK);
		KeybdEvent(VK_RETURN);
		KeybdEvent(VK_RETURN, 3000);

		Recognizer = RECT{ 10, 54, 190, 120 }; // `자쿰의 제단 입장` 미니맵
		Recognizer.MoveToRelativeCriteria(142);
		Sleep(1000);

		GetEyeOfFire();
		MoveFromZ1ToZ2(1);
	}

private:
	Mat TargetImagePictureUrusDimensionalMirror;
	Mat TargetImageMinimapMarkElnas;
	Mat TargetImageBackgroundUrus;
	Mat TargetImageNpcRene;
};
