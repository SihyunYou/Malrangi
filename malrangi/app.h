#pragma once
#include "cvwrap.h"
#include "client.h"
#include "ip.h"

#define WAIT Sleep(512)

class BasePlay
{
public:
	class AppException : public std::exception
	{
	public:
		AppException(void) :
			Message(__CLASSNAME__) {}
		explicit AppException(const char* Message) :
			Message(Message) {}
		explicit AppException(string Message) :
			Message(Message) {}
		virtual ~AppException() throw () {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}

	protected:
		std::string Message;
	};

protected:
	Mat SourceImage;
	RECT NewRectClient;
};

class UrusRaid : protected BasePlay
{
public:
	UrusRaid(void)
	{
		;
	}

	class NpcMashurNotFoundException : public AppException
	{
	public:
		NpcMashurNotFoundException(void) :
			AppException(__CLASSNAME__) {}
		virtual const char* what(void) const throw()
		{
			return Message.c_str();
		}
	};



public:
	void Play(
		CONST CONF_INFO::NEXONAC_INFO& AccountInfo)
	{
		static Mat TargetImageBackgroundUrus(Cvw::Read("res\\background_urus.jpg"));
		static Mat TargetImageNpcMashur(Cvw::Read("res\\npc_mashur.jpg"));
		static Mat TargetImageButtonEnter1(Cvw::Read("res\\button_enter_urus1.jpg"));
		static Mat TargetImageUnitInUrusBattleMap(Cvw::Read("res\\unit_in_urus_battle_map.jpg"));
		static Mat TargetImageButtonEnter2(Cvw::Read("res\\button_enter_urus2.jpg"));
		static int SeqPlay = 0;

		for (int CountPlay = 0; CountPlay < 3; CountPlay++)
		{
			/*** 우르스 맵 입장 ***/
			try
			{
				Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageNpcMashur, LEFT_CLICK, { 10, 30 });
			}
			catch (MatchFailedException)
			{
				throw AppException("NpcMashurNotFound");
			}

			MouseEvent({ 440, 684 }, LEFT_CLICK);
			MouseEvent({ 487, 745 }, LEFT_CLICK);
			MouseEvent({ 820, 754 }, LEFT_CLICK);
			try
			{
				Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageButtonEnter1, LEFT_CLICK);
			}
			catch (MatchFailedException)
			{
				throw AppException("ButtonEnterNotFound");
			}

			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.left += 100;
			NewRectClient.right -= 700;
			NewRectClient.bottom -= 500;
			try
			{
				Cvw::DoUntilMatchingTemplate(
					NewRectClient,
					TargetImageUnitInUrusBattleMap,
					[]()
					{
						MouseEvent({ 640, 429 }, LEFT_CLICK, 0x80);
						KeybdEvent('E', 0x80);
					},
					600000);
			}
			catch (MatchFailedException & cwe)
			{
				throw;
			}


			/*** 전투 중 ***/
			MouseEvent({ 897, 133 }, LEFT_CLICK);

			BYTE CurrentDirectionKey = NULL;
			BYTE LastDirectionKey = NULL;
			INT CountNullOfCurrentDirection = 0;
			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.left += 900;
			NewRectClient.top += 530;
			auto DoBattle = [&](void) -> void
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

			try
			{
				Cvw::DoUntilMatchingTemplate(
					NewRectClient,
					TargetImageButtonEnter2,
					DoBattle,
					1000000,
					0);
			}
			catch (MatchFailedException)
			{
				throw AppException("BattleTimeoutException");
			}
			
			Sleep(0x400);
			imwrite("C:\\" + to_string(++SeqPlay) + ".jpg", Cvw::Capture(ClientApi::RECT_CLIENT4, 1));

			MouseEvent({ 1259, 668 }, LEFT_CLICK);
			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 45000, 256, 0.99);
			}
			catch (MatchFailedException)
			{
				throw AppException("UrusExitException");
			}

			if (2 > CountPlay)
			{
				KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 5000);
			}
			WriteLog(LOG_LEVEL::SUCCESS, "Urus raid has been complete. (Seq : %d)\n", SeqPlay);
		}
	}
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

protected:
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
		catch (MatchFailedException & cwe)
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
			catch (MatchFailedException & cwe)
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
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageScluptureInDoorThroughZacum, NONWORK, 10000);
		}
		catch (MatchFailedException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2000);
			Sleep(800);

			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageScluptureInDoorThroughZacum, NONWORK, 10000);
			}
			catch (MatchFailedException & cwe)
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
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageUnitInAlterOfZacum, NONWORK, 10000);
		}
		catch (MatchFailedException & cwe)
		{
			WriteLog(LOG_LEVEL::FAILURE, cwe.what());

			ClientApi::MakeParty();
			MouseEvent({ 1000, 544 }, LEFT_CLICK, 600);
			KeybdEvent(VK_RETURN);

			try
			{
				Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageUnitInAlterOfZacum, NONWORK, 10000);
			}
			catch (MatchFailedException & cwe)
			{
				throw;
			}
		}
	}


public:
	void Play(
		CONST CONF_INFO::NEXONAC_INFO::MAPLEID_INFO::SERVER_INFO::CHARACTER_INFO& CharacterSpecialty,
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
	

		/*** 불의 눈 던지기 ***/
		CONF_INFO::KEYSET_INFO& KeysetInfo = CONF_INFO::GetInstance()->VirtualKeyset;
		KeybdEvent(KeysetInfo.Inventory);
		try
		{
			SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
			MouseEvent({ 25, 50 }, LEFT_CLICK);
		}
		catch (MatchFailedException & cwe)
		{
			SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImageInventoryBar, LEFT_CLICK, { 40 * 2, 40 }, 600);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImageButtonExpandingInventory, LEFT_CLICK, { 6, 6 }, 600);

			SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
			Cvw::ClickMatchedTemplate(SourceImage, TargetImageEyeOfFire, LEFT_CLICK, { 10, 10 }, 600);
			MouseEvent({ 25, 50 }, LEFT_CLICK);
		}

		KeybdEvent(KeysetInfo.Inventory, 3600);


		/*** 쿰돌이 ***/
		if (CharacterSpecialty.RequiredBuf2 != NULL)
		{
			KeybdEvent(CharacterSpecialty.RequiredBuf2, 1000);
		}
		Cvw::DoUntilMatchingTemplate(
			ClientApi::RECT_CLIENT4,
			TargetImageCrystalOfBoss,
			[CharacterSpecialty]()
			{
				KeybdEventDown(CharacterSpecialty.Skill);
				MouseEvent({ 800, 294 }, LEFT_CLICK, 0);
			},
			36000,
			50);
		KeybdEventUp(CharacterSpecialty.Skill);


		/*** 줍기 ***/
		KeybdEventContinuedWithSubKey(VK_LEFT, KeysetInfo.Picking, 2000);
		KeybdEventContinuedWithSubKey(VK_RIGHT, KeysetInfo.Picking, 5000);
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
		TargetImageMinimapMarkMeisterVill(Cvw::Read("res\\minimap_mark_meistervill.jpg")),
		TargetImageItemCrystal(Cvw::Read("res\\item_crystal_of_boss.jpg")),
		TargetImageNpcMsBrainy(Cvw::Read("res\\npc_msbrainy.jpg")),
		TargetImageItem100lv(Cvw::Read("res\\item_100lv.jpg")),
		TargetImageItem110lv(Cvw::Read("res\\item_110lv.jpg")),
		TargetImageItemCube(Cvw::Read("res\\item_cube.jpg"))
	{
		;
	}

public:
	void Play(
		CONST CONF_INFO::NEXONAC_INFO::MAPLEID_INFO& MapleIdInfo)
	{
		MoveFromZ2ToZ1();
		GetEyeOfFire();
		MoveFromZ1ToZ2(LEVEL_OF_DIFFICULTY::CHAOS);

		/*** `자쿰의 제단 입구(카오스) -> `마이스터 빌` 이동 ***/
		CONF_INFO::KEYSET_INFO& KeysetInfo = CONF_INFO::GetInstance()->VirtualKeyset;
		KeybdEvent(KeysetInfo.SpecialTechnology);
		try
		{
			Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImageButtonMeisterVill, LEFT_CLICK, { 5, 2 });
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		KeybdEvent(VK_RETURN);
		try
		{
			Cvw::DoUntilMatchingTemplate({ 0, 0, 64, 64 }, TargetImageMinimapMarkMeisterVill, NONWORK, 30000);
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		WAIT;
		KeybdEvent(KeysetInfo.SpecialTechnology);


		/*** `마이스터 빌` -> `자유시장` 이동 ***/
		ClientApi::DownJump(1500);
		ClientApi::DownJump(1500);
		KeybdEventContinued(VK_LEFT, 1000);
		WAIT;
		KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2600);
		try
		{ 
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMsBrainy, NONWORK, 15000);
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}


		/*** 아이템 팔기(콜렉터) ***/
		MouseEvent({ 980, 334 }, LEFT_CLICK); 
		
		// 장비창
		for (int i = 0; i < 40; i++)
		{
			MouseEvent({ 735, 279 }, RIGHT_CLICK, 300);
		}

		// 기타창
		INT CountCrystal;
		NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.top += 220;
		NewRectClient.left += 710;

		MouseEvent({ 818, 244 }, LEFT_CLICK);
		for(CountCrystal = 0; CountCrystal < 30; CountCrystal++)
		{
			try
			{
				Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItemCrystal, RIGHT_CLICK, { NewRectClient.left + 80, NewRectClient.top });
			}
			catch (MatchFailedException & cwe)
			{
				break;
			}

			KeybdEvent(VK_RETURN);
		}
		KeybdEvent(VK_ESCAPE);


		/*** 창고에 아이템 넣기  ***/
		if (0 < CountCrystal)
		{
			MouseEvent({ 310, 334 }, LEFT_CLICK);
			ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);

			// 장비템 넣기(아쿠아틱 + 응축)
			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.top += 270;
			NewRectClient.left += 690;

			for (int i = 0; i < 8; i++)
			{
				try
				{
					Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient), TargetImageItem100lv, LEFT_CLICK, { NewRectClient.left, NewRectClient.top });
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}

				MouseEvent({ 810, 204 }, LEFT_CLICK);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
			}

			for (int i = 0; i < 8; i++)
			{
				try
				{
					Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient),TargetImageItem110lv,LEFT_CLICK,{ NewRectClient.left, NewRectClient.top });
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}

				MouseEvent({ 810, 204 }, LEFT_CLICK);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
			}

			// 소비템 넣기(수큐)
			MouseEvent({ 740, 264 }, LEFT_CLICK);
			for (int i = 0; i < 3; i++)
			{
				try
				{
					Cvw::ClickMatchedTemplate(Cvw::Capture(NewRectClient),TargetImageItemCube,LEFT_CLICK,{ NewRectClient.left, NewRectClient.top });
				}
				catch (MatchFailedException & cwe)
				{
					break;
				}

				MouseEvent({ 810, 204 }, LEFT_CLICK);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
				KeybdEvent(VK_RETURN);
			}

			// 돈 넣기
			MouseEvent({ 715, 574 }, LEFT_CLICK);
			KeybdEvent(VK_RETURN);

			// 창고 창 닫기
			MouseEvent({ 640, 204 }, LEFT_CLICK);
		}


		/*** `자유시장` -> `마이스터 빌` -> `자쿰의 제단 입구(카오스)` 이동 ***/
	RETURN_TO_Z2:
		KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
		try
		{
			Cvw::DoUntilMatchingTemplate({ 0, 0, 64, 64 }, TargetImageMinimapMarkMeisterVill, NONWORK, 20000);
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		WAIT;
		ClientApi::MoveServer(true);
		try
		{
			Cvw::DoUntilMatchingTemplate({ 0, 0, 64, 64 }, TargetImageMinimapMarkMeisterVill, NONWORK, 20000);
		}
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		WAIT;
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
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		WAIT;
	}

private:
	Mat TargetImageButtonMeisterVill;
	Mat TargetImageMinimapMarkMeisterVill;
	Mat TargetImageNpcMsBrainy;
	Mat TargetImageItemCrystal;
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
		catch (MatchFailedException & cwe)
		{
			throw;
		}

		WAIT;
		MouseEvent({ 58, 168 }, LEFT_CLICK);
		MouseEvent({ 566, 376 }, LEFT_CLICK);
		SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
		Cvw::ClickMatchedTemplate(SourceImage, TargetImagePictureUrusDimensionalMirror, LEFT_CLICK);
		MouseEvent({ 690, 596 }, LEFT_CLICK);

		try
		{
			Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, 30000);
		}
		catch (CvWrappedException & cwe)
		{
			throw;
		}

		WAIT;
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
		catch (MatchFailedException & cwe)
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
		MoveFromZ1ToZ2(Zacum::LEVEL_OF_DIFFICULTY::NORMAL);
	}

private:
	Mat TargetImagePictureUrusDimensionalMirror;
	Mat TargetImageMinimapMarkElnas;
	Mat TargetImageBackgroundUrus;
	Mat TargetImageNpcRene;
};
