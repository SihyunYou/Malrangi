#pragma once
#include "client_manage.h"
#include "ipmanage.h"

namespace Bsp
{
	class BasePlay
	{
	protected:
		Mat SourceImage;
		RECT NewRectClient;
	};

	class UrusRaid : protected BasePlay
	{
	public:
		UrusRaid(
			void) :
			TargetImageNpcMashur(Cvw::Read("res\\npc_mashur.jpg")),
			TargetImageBackgroundUrus(Cvw::Read("res\\background_urus.jpg")),
			TargetImageUnitInUrusBattleMap(Cvw::Read("res\\unit_in_urus_battle_map.jpg")),
			TargetImageButtonEnter(Cvw::Read("res\\button_enter.jpg"))
		{
			;
		}

	public:
		void Play(
			CONST CONF_INFO::ACCOUNT_INFO& AccountInfo)
		{
			for (int PlayCount = 0; PlayCount < 3; PlayCount++)
			{
				/*** 우르스 맵 입장 ***/
			RETRY_CLICK_MASHUR:
				try
				{
					Cvw::ClickMatchedTemplate(Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4), TargetImageNpcMashur, LEFT_CLICK, { 10, 30 });
				}
				catch (MatchFailedException & cwe)
				{
					goto RETRY_CLICK_MASHUR;
				}

				MouseEvent({ 440, 684 }, LEFT_CLICK);
				MouseEvent({ 487, 745 }, LEFT_CLICK);
				MouseEvent({ 820, 754 }, LEFT_CLICK);
				MouseEvent({ 772, 468 }, LEFT_CLICK);

				NewRectClient = CLIENT_ELEM::RECT_CLIENT4;
				NewRectClient.left += 100;
				NewRectClient.right -= 700;
				NewRectClient.bottom -= 500;

				Cvw::DoUntilMatchingTemplate
				(
					NewRectClient,
					TargetImageUnitInUrusBattleMap,
					[]()
					{
						MouseEvent({ 640, 429 }, LEFT_CLICK, 300);
						KeybdEventDown('E');
					},
					1000000
						);
				MouseEvent({ 897, 133 }, LEFT_CLICK);


				/*** 전투 중 ***/
				NewRectClient = CLIENT_ELEM::RECT_CLIENT4;
				NewRectClient.left += 900;
				NewRectClient.top += 530;

				BYTE CurrentDirectionKey = NULL;
				BYTE LastDirectionKey = NULL;
				INT CountNullOfCurrentDirection = 0;
				auto DoBattle = [&](void) -> void
				{
					INT Score = 0;
					RECT NewRect = CLIENT_ELEM::RECT_CLIENT4;
					NewRect.top += 156;
					NewRect.bottom -= 100;
					Mat SourceImage = Cvw::Capture(NewRect, 1);
					Mat MaskImage;

					try
					{
						Cvw::InRange(SourceImage, Scalar(242, 30, 195), Scalar(255, 60, 205), MaskImage);
						//Cvw::Show(Mask);
					}
					catch (IntegerDivisionByZeroException & cwe)
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

					if (Score >= -2 && Score <= 2)
					{
						CurrentDirectionKey = NULL;
					}
					else
					{
						LastDirectionKey = CurrentDirectionKey = (Score < 0) ? VK_LEFT : VK_RIGHT;
					}

					if (CountNullOfCurrentDirection > 20 && CurrentDirectionKey == NULL)
					{
						LastDirectionKey = (LastDirectionKey == VK_LEFT) ? VK_RIGHT : VK_LEFT;
						CountNullOfCurrentDirection = 0;
					}

					if (NULL == LastDirectionKey && NULL == CurrentDirectionKey)
					{
						KeybdEventContinued(VK_LEFT, 250);
						KeybdEvent('E', 250);
						KeybdEventContinued(VK_RIGHT, 250);
						KeybdEvent('E', 250);
					}
					else if (NULL != LastDirectionKey && NULL == CurrentDirectionKey)
					{
						KeybdEventContinued(LastDirectionKey, 250);
						KeybdEvent('E', 250);

						++CountNullOfCurrentDirection;
					}
					else // NULL != CurrentDirectionKey
					{
						KeybdEventContinued(CurrentDirectionKey, 250);
						KeybdEvent('E', 250);

						CountNullOfCurrentDirection = 0;
					}
				};

				Cvw::DoUntilMatchingTemplate
				(
					NewRectClient,
					TargetImageButtonEnter,
					DoBattle,
					1000000,
					0
				);
				WriteLog(LOG_LEVEL::SUCCESS, "Urus raid has been complete.\n");

				Sleep(1000);
				srand(time(NULL));
				imwrite(("C:\\" + to_string(rand() % 4243225) + ".jpg"), Cvw::Capture({ 978, 537, 1141, 561 }, 1));
				MouseEvent({ 1259, 668 }, LEFT_CLICK);
				try
				{
					Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 45000, 256, 0.99);
				}
				catch (TimeOutException & cwe)
				{
					WriteLog(LOG_LEVEL::FAILURE, cwe.what());

					throw;
				}


				/*** 퇴장 ***/
				if (2 == PlayCount)
				{
					return;
				}
				else
				{
					KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 5000);
					try
					{
						Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageNpcMashur, NONWORK, 45000, 256, 0.99);
					}
					catch (TimeOutException & cwe)
					{
						WriteLog(LOG_LEVEL::FAILURE, cwe.what());
						KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 5000);

						try
						{
							Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageNpcMashur, NONWORK, 45000, 256, 0.99);
						}
						catch (TimeOutException & cwe)
						{
							throw;
						}
					}
				}
			}
		}

	private:
		Mat TargetImageNpcMashur;
		Mat TargetImageBackgroundUrus;
		Mat TargetImageUnitInUrusBattleMap;
		Mat TargetImageButtonEnter;
	};

	class Zacum : protected BasePlay
	{
	public:
		Zacum(void) :
			TargetImageScluptureInDoorThroughZacum(Cvw::Read("res\\sclpture_in_door_through_zacum.jpg")),
			TargetImageNpcAdobis(Cvw::Read("res\\npc_adobis.jpg"))
		{
			;
		}

	public:
		enum LEVEL_OF_DIFFICULTY
		{
			EASY = 0,
			NORMAL,
			CHAOS
		};

	public:
		void MoveFromZ1ToZ2(LEVEL_OF_DIFFICULTY Level) /*** `자쿰으로 통하는 문` -> `자쿰의 제단 입구(이지/노말/카오스)` 이동 ***/
		{
			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2500);
			for (int i = 0; i < (int)Level; i++)
			{
				KeybdEvent(VK_DOWN, 600);
			}
			KeybdEvent(VK_RETURN);

			try
			{
				Cvw::DoUntilMatchingTemplate(RECT{ 950, 500, 1050, 600 }, TargetImageNpcAdobis, NONWORK, 10000);
			}
			catch (TimeOutException & cwe)
			{
				WriteLog(LOG_LEVEL::FAILURE, cwe.what());

				KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 2500);
				for (int i = 0; i < (int)Level; i++)
				{
					KeybdEvent(VK_DOWN, 600);
				}
				KeybdEvent(VK_RETURN);

				try
				{
					Cvw::DoUntilMatchingTemplate(RECT{ 950, 500, 1050, 600 }, TargetImageNpcAdobis, NONWORK, 10000);
				}
				catch (TimeOutException & cwe)
				{
					throw;
				}
			}
		}
		void MoveFromZ2ToZ1(void) /*** `자쿰의 제단 입구` -> `자쿰으로 통하는 문` 이동 ***/
		{
			KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 2000);
			Sleep(800);

			try
			{
				Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageScluptureInDoorThroughZacum, NONWORK, 10000);
			}
			catch (TimeOutException & cwe)
			{
				WriteLog(LOG_LEVEL::FAILURE, cwe.what());

				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2000);
				Sleep(800);

				try
				{
					Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageScluptureInDoorThroughZacum, NONWORK, 10000);
				}
				catch (TimeOutException & cwe)
				{
					throw;
				}
			}
		}
		void GetEyeOfFire(void)
		{
			MouseEvent({ 820, 563 }, LEFT_CLICK, 600);
			KeybdEvent(VK_DOWN, 400);
			KeybdEvent(VK_DOWN, 400);
			KeybdEvent(VK_RETURN, 400);
			KeybdEvent(VK_DOWN, 400);
			KeybdEvent(VK_RETURN, 400);
			KeybdEvent(VK_RETURN, 400);
			KeybdEvent(VK_RETURN, 400);
			KeybdEvent(VK_RETURN, 400);
		}

	protected:
		Mat TargetImageScluptureInDoorThroughZacum;
		Mat TargetImageNpcAdobis;
	};
	class ZacumRaid : public Zacum
	{
	public:
		ZacumRaid(void) :
			TargetImageEyeOfFire(Cvw::Read("res\\eye_of_fire.jpg")),
			TargetImageInventoryBar(Cvw::Read("res\\inventory_bar.jpg")),
			TargetImageButtonExpandingInventory(Cvw::Read("res\\button_expanding_inventory.jpg")),
			TargetImageUnitInAlterOfZacum(Cvw::Read("res\\unit_in_alter_of_zacum.jpg")),
			TargetImageCrystalOfBoss(Cvw::Read("res\\crystal_of_boss.jpg"))
		{
			;
		}

	public:
		void MoveFromZ2ToZ3(void)
		{
			MouseEvent({ 1000, 544 }, LEFT_CLICK, 600);
			KeybdEvent(VK_RETURN);

			try
			{
				Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageUnitInAlterOfZacum, NONWORK, 10000);
			}
			catch (TimeOutException & cwe)
			{
				WriteLog(LOG_LEVEL::FAILURE, cwe.what());

				Exc::MakeParty();
				MouseEvent({ 1000, 544 }, LEFT_CLICK, 600);
				KeybdEvent(VK_RETURN);

				try
				{
					Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageUnitInAlterOfZacum, NONWORK, 10000);
				}
				catch (TimeOutException & cwe)
				{
					throw;
				}
			}
		}


	public:
		void Play(
			CONST CONF_INFO::ACCOUNT_INFO::CHARACTER_SPECIALTY& CharacterSpecialty,
			BOOL IsReady)
		{
			/*** 자쿰의 제단 입장 전 준비 ***/
			if (!IsReady)
			{
				MoveFromZ2ToZ1();
				GetEyeOfFire();
				MoveFromZ1ToZ2(LEVEL_OF_DIFFICULTY::NORMAL);
			}
			MoveFromZ2ToZ3();


			/*** 자쿰의 제단 입장 ***/
			KeybdEventContinued(VK_RIGHT, 1000);
			if (CharacterSpecialty.RequiredBuf1 != NULL)
			{
				KeybdEvent(CharacterSpecialty.RequiredBuf1, 1000);
			}
			KeybdEvent(Conf.VirtualKeyset.Potion);


			/*** 불의 눈 던지기 ***/
			KeybdEvent(Conf.VirtualKeyset.Inventory);

			try
			{
				SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
				MouseEvent({ 25, 50 }, LEFT_CLICK);
			}
			catch (MatchFailedException & cwe)
			{
				SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageInventoryBar, LEFT_CLICK, { 40 * 2, 40 }, 600);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageButtonExpandingInventory, LEFT_CLICK, { 6, 6 }, 600);

				SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
				Cvw::ClickMatchedTemplate(SourceImage, TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
				MouseEvent({ 25, 50 }, LEFT_CLICK);
			}

			KeybdEvent(Conf.VirtualKeyset.Inventory, 3600);


			/*** 쿰돌이 ***/
			if (CharacterSpecialty.RequiredBuf2 != NULL)
			{
				KeybdEvent(CharacterSpecialty.RequiredBuf2, 1000);
			}
			Cvw::DoUntilMatchingTemplate
			(
				CLIENT_ELEM::RECT_CLIENT4,
				TargetImageCrystalOfBoss,
				[CharacterSpecialty]()
				{
					KeybdEventDown(CharacterSpecialty.Skill);
					MouseEvent({ 800, 294 }, LEFT_CLICK, 0);
				},
				36000,
					50
					);
			KeybdEventUp(CharacterSpecialty.Skill);


			/*** 줍기 ***/
			KeybdEventContinuedWithSubKey(VK_LEFT, Conf.VirtualKeyset.Picking, 2000);
			KeybdEventContinuedWithSubKey(VK_RIGHT, Conf.VirtualKeyset.Picking, 5000);
		}

	private:
		Mat TargetImageEyeOfFire;
		Mat TargetImageInventoryBar;
		Mat TargetImageButtonExpandingInventory;
		Mat TargetImageUnitInAlterOfZacum;
		Mat TargetImageCrystalOfBoss;
	};
	class ZacumCalc : public Zacum
	{
	public:
		ZacumCalc(void) :
			TargetImageButtonMeisterVill(Cvw::Read("res\\button_meistervill.jpg")),
			TargetImageItemCrystalOfBoss(Cvw::Read("res\\item_crystal_of_boss.jpg")),
			TargetImageNpcMsBrainy(Cvw::Read("res\\npc_msbrainy.jpg")),
			TargetImageItem100lv(Cvw::Read("res\\item_100lv.jpg")),
			TargetImageItem110lv(Cvw::Read("res\\item_110lv.jpg")),
			TargetImageItemCube(Cvw::Read("res\\item_cube.jpg"))
		{
			;
		}

	public:
		void Play(
			CONST CONF_INFO::ACCOUNT_INFO& AccountInfo)
		{
			MoveFromZ2ToZ1();
			GetEyeOfFire();
			MoveFromZ1ToZ2(LEVEL_OF_DIFFICULTY::CHAOS);

			/*** `자쿰의 제단 입구(카오스) -> `마이스터 빌` 이동 ***/
			KeybdEvent(Conf.VirtualKeyset.SpecialTechnology);
			SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImageButtonMeisterVill, LEFT_CLICK, { 5, 2 });
			KeybdEvent(VK_RETURN, 10000);
			KeybdEvent(Conf.VirtualKeyset.SpecialTechnology);


			/*** `마이스터 빌` -> `자유시장` 이동 ***/
			Exc::DownJump(1200);
			Exc::DownJump(1200);
			KeybdEventContinued(VK_LEFT, 1500);
			Sleep(300);
			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 3000);
			Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageNpcMsBrainy, NONWORK, 15000);


			/*** 아이템 팔기 ***/
			MouseEvent({ 980, 334 }, LEFT_CLICK); // 콜렉터(기본 장비창)
			for (int i = 0; i < 50; i++)
			{
				MouseEvent({ 735, 279 }, RIGHT_CLICK, 350);
			}
			Sleep(1000);

			MouseEvent({ 818, 244 }, LEFT_CLICK); // 기타창
			NewRectClient = CLIENT_ELEM::RECT_CLIENT4;
			NewRectClient.top += 220;
			NewRectClient.left += 710;
			for (int i = 0; i < 30; i++)
			{
				try
				{
					SourceImage = Cvw::Capture(NewRectClient);
					Cvw::ClickMatchedTemplate(SourceImage, TargetImageItemCrystalOfBoss, RIGHT_CLICK, { NewRectClient.left + 80, NewRectClient.top });
					KeybdEvent(VK_RETURN);
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}
			}
			KeybdEvent(VK_ESCAPE);


			/*** 창고에 아이템 넣기  ***/
			MouseEvent({ 310, 334 }, LEFT_CLICK);
			Exc::UnlockSecondPassword(AccountInfo.SecondPassword);

			// 장비템 넣기(아쿠아틱 + 응축)
			NewRectClient = CLIENT_ELEM::RECT_CLIENT4;
			NewRectClient.top += 270;
			NewRectClient.left += 690;
			for (int i = 0; i < 8; i++)
			{
				try
				{
					SourceImage = Cvw::Capture(NewRectClient);
					Cvw::ClickMatchedTemplate(SourceImage, TargetImageItem100lv, LEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					MouseEvent({ 810, 204 }, LEFT_CLICK);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}
			}
			for (int i = 0; i < 8; i++)
			{
				try
				{
					SourceImage = Cvw::Capture(NewRectClient);
					Cvw::ClickMatchedTemplate(SourceImage, TargetImageItem110lv, LEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					MouseEvent({ 810, 204 }, LEFT_CLICK);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}
			}


			// 소비템 넣기(수큐)
			MouseEvent({ 740, 264 }, LEFT_CLICK);
			for (int i = 0; i < 3; i++)
			{
				try
				{
					SourceImage = Cvw::Capture(NewRectClient, 0);
					Cvw::ClickMatchedTemplate(SourceImage, TargetImageItemCube, LEFT_CLICK, { NewRectClient.left, NewRectClient.top });
					MouseEvent({ 810, 204 }, LEFT_CLICK);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
					KeybdEvent(VK_RETURN);
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}
			}


			// 돈 넣기
			MouseEvent({ 715, 574 }, LEFT_CLICK);
			KeybdEvent(VK_RETURN);
			MouseEvent({ 640, 204 }, LEFT_CLICK);


			/*** `자유시장` -> `마이스터 빌` -> `자쿰의 제단 입구(카오스)` 이동 ***/
			KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 3000);
			Sleep(5000);
			Exc::MoveServerNext(10000);
			KeybdEvent(VK_UP, 5000);
		}

	private:
		Mat TargetImageButtonMeisterVill;
		Mat TargetImageItemCrystalOfBoss;
		Mat TargetImageNpcMsBrainy;
		Mat TargetImageItem100lv;
		Mat TargetImageItem110lv;
		Mat TargetImageItemCube;
	};
	class ZacumAndUrus : public Zacum
	{
	public:
		ZacumAndUrus() :
			TargetImagePictureUrusDimensionalMirror(Cvw::Read("res\\picture_urus_dimensional_mirror.jpg")),
			TargetImageMinimapMarkElnas(Cvw::Read("res\\minimap_mark_elnas.jpg")),
			TargetImageBackgroundUrus(Cvw::Read("res\\background_urus.jpg")),
			TargetImageNpcRene(Cvw::Read("res\\npc_rene.jpg"))
		{
			;
		}

	public:
		void MoveFromZ2ToUrus(void)
		{
			MoveFromZ2ToZ1();

			MouseEvent({ 820, 544 }, LEFT_CLICK);
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
			catch (CvWrappedException & cwe)
			{
				throw;
			}

			MouseEvent({ 58, 168 }, LEFT_CLICK);
			MouseEvent({ 566, 376 }, LEFT_CLICK);
			SourceImage = Cvw::Capture(CLIENT_ELEM::RECT_CLIENT4);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImagePictureUrusDimensionalMirror, LEFT_CLICK);
			MouseEvent({ 690, 596 }, LEFT_CLICK);

			try
			{
				Cvw::DoUntilMatchingTemplate(CLIENT_ELEM::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 30000);
			}
			catch (CvWrappedException & cwe)
			{
				throw;
			}

			Sleep(1000);
		}
		void MoveFromUrusToZ2(void)
		{
			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2000);
			Sleep(3000);

			Exc::MinimapRecognizer Recognizer({ 10, 64, 260, 160 }); // `엘나스` 미니맵
		RETRY:
			Recognizer.MoveToRelativeCriteria(185);
			Sleep(2000);
			Exc::Jump(EXC::JUMP_TYPE::DEMON);
			Exc::DownJump(1000);
			KeybdEvent(VK_UP, 3000);

			try
			{
				Cvw::DoUntilMatchingTemplate({ 500, 0, 650, 150 }, TargetImageNpcRene, NONWORK, 5000);
			}
			catch (TimeOutException & cwe)
			{
				WriteLog(LOG_LEVEL::FAILURE, cwe.what());

				KeybdEventContinued(VK_LEFT, 7000);
				goto RETRY;
			}

			MouseEvent({ 482, 566 }, LEFT_CLICK);
			KeybdEvent(VK_RETURN);
			KeybdEvent(VK_RETURN, 3000);

			Recognizer = RECT{ 10, 54, 190, 120 }; // `자쿰의 제단 입장` 미니맵
			Recognizer.MoveToRelativeCriteria(142);
			Sleep(1000);

			GetEyeOfFire();
			MoveFromZ1ToZ2(Bsp::Zacum::LEVEL_OF_DIFFICULTY::NORMAL);
		}

	private:
		Mat TargetImagePictureUrusDimensionalMirror;
		Mat TargetImageMinimapMarkElnas;
		Mat TargetImageBackgroundUrus;
		Mat TargetImageNpcRene;
	};
}



namespace Crcp
{
	class CircularPlay
	{
	public:
		virtual void Play(void) = NULL;
	};

	class UrusRaid : public CircularPlay
	{
	public:
		virtual void Play(void)
		{
			Bsp::UrusRaid Head;
			INT SeqAccount = 0;

			for each (const auto & AccountInfo in Conf.ArrAccount)
			{
				if (SeqAccount >= Conf.CountAccount)
				{
					break;
				}

				/*** 로그인 ***/
				try
				{
					if (SeqAccount % 4 == 0)
					{
						Exc::BootClient();
					}
					Exc::LoginInInitialStage(AccountInfo);
					Exc::SelectCharacter(1);
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}


				/*** 인게임 ***/
				try
				{
					Exc::EnterGame(AccountInfo);
					Exc::RemoveToolTip();
					Exc::BreakParty();

					Head.Play(AccountInfo);

					Exc::ExitGame();
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}


				/*** 로그아웃 ***/
				try
				{
					if (SeqAccount % 4 == 3)
					{
						Exc::TerminateClient();
						RenewIPAddress();
					}
					else
					{
						Exc::Logout();
					}
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}

				++SeqAccount;
			}
		}
	};

	class ZacumRaid : public CircularPlay
	{
	public:
		virtual void Play(void)
		{
			Bsp::ZacumRaid Branch;
			INT SeqAccount = 0;

			for each (auto & AccountInfo in Conf.ArrAccount)
			{
				if (0 == AccountInfo.VecCharacter.size())
				{
					continue;
				}

				/*** 로그인 ***/
				try
				{
					if (SeqAccount % 4 == 0)
					{
						Exc::BootClient();
					}
					Exc::LoginInInitialStage(AccountInfo);
					Exc::SelectCharacter(2);
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}


				/*** 인게임 ***/
				for each (auto & CharacterInfo in AccountInfo.VecCharacter)
				{
					try
					{
						Exc::EnterGame(AccountInfo);
						Exc::RemoveToolTip();
						Exc::MakeParty();

						Branch.Play(CharacterInfo, FALSE);

						Exc::ExitGame();
						KeybdEvent(VK_RIGHT);
					}
					catch (CvWrappedException & cwe)
					{
						WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
					}
				}


				/*** 로그아웃 ***/
				try
				{
					if (SeqAccount % 4 == 3)
					{
						Exc::TerminateClient();
						RenewIPAddress();
					}
					else
					{
						Exc::Logout();
					}
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}

				++SeqAccount;
			}
		}
	};
	class ZacumCalc : public CircularPlay
	{
	public:
		virtual void Play(void)
		{
			Bsp::ZacumCalc Branch;
			INT SeqAccount = 0;

			for each (auto & AccountInfo in Conf.ArrAccount)
			{
				if (0 == AccountInfo.VecCharacter.size())
				{
					continue;
				}

				/*** 로그인 ***/
				try
				{
					if (SeqAccount % 4 == 0)
					{
						Exc::BootClient();
					}
					Exc::LoginInInitialStage(AccountInfo);
					Exc::SelectCharacter(2);
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}


				/*** 인게임 ***/
				// 쿰돌이
				for each (auto & CharacterInfo in AccountInfo.VecCharacter)
				{
					try
					{
						Exc::EnterGame(AccountInfo);
						Exc::RemoveToolTip();

						Branch.Play(AccountInfo);

						Exc::ExitGame();
						KeybdEvent(VK_RIGHT);
					}
					catch (CvWrappedException & cwe)
					{
						WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
					}
				}

				/*** 로그아웃 ***/
				try
				{
					if (SeqAccount % 4 == 3)
					{
						Exc::TerminateClient();
						RenewIPAddress();
					}
					else
					{
						Exc::Logout();
					}
				}
				catch (CvWrappedException & cwe)
				{
					WriteLog(LOG_LEVEL::CRITICAL, cwe.what());
				}

				++SeqAccount;
			}
		}
	};
}
