#pragma once
#include "client.h"

class Calc
{
private:
	VALLOC MatchInfo;

protected:
	enum class CALC_EXCEPTION_CODE
	{

	};

	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		static const Mat TargetImageNpcCollector(Read(TARGET_DIR "npc//collector.jpg"));
		static const Mat TargetImageItemCrystal(Read(TARGET_DIR "item//crystal1.jpg"));
		static const Mat TargetImageItem100lv(Read(TARGET_DIR "item//100lv.jpg"));
		static const Mat TargetImageItem110lv(Read(TARGET_DIR "item//110lv.jpg"));
		static const Mat TargetImageItemCube(Read(TARGET_DIR "item//cube.jpg"));

		/*** 아이템 팔기(콜렉터) ***/
		MatchTemplate(SourceImageClient4, TargetImageNpcCollector, &MatchInfo);
		MouseEvent(MatchInfo.Location, LEFT_CLICK);

		// 장비창
		for (int i = 0; i < 45; i++)
		{
			MouseEvent({ 735, 305 }, RIGHT_CLICK, 300);
		}

		// 기타창
		int CountCrystal;
		RECT NewRectClient = ClientApi::RECT_CLIENT4;
		NewRectClient.top += 220;
		NewRectClient.left += 710;

		MouseEvent({ 818, 270 }, LEFT_CLICK);
		for (CountCrystal = 0; CountCrystal < 30; CountCrystal++)
		{
			if (MatchTemplate(Capture(NewRectClient), TargetImageItemCrystal, &MatchInfo))
			{
				MouseEvent(MatchInfo.Location + Point{ NewRectClient.left + 80, NewRectClient.top }, RIGHT_CLICK);
				KeybdEvent(VK_RETURN);
			}
			else
			{
				break;
			}
		}
		KeybdEvent(VK_ESCAPE);


		/*** 창고에 아이템 넣기  ***/
		if (0 < CountCrystal)
		{
			MouseEvent({ 310, 360 }, LEFT_CLICK);
			ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);

			NewRectClient = ClientApi::RECT_CLIENT4;
			NewRectClient.top += 270;
			NewRectClient.left += 690;

			// 장비템 넣기(아쿠아틱 + 응축)
			const array<Mat, 2> ArrTargetImageItemEquipment = { TargetImageItem100lv, TargetImageItem110lv };
			for each (const auto & TargetImage in ArrTargetImageItemEquipment)
			{
				for (int i = 0; i < 8; i++)
				{
					MouseEvent(POS_VOID, CURSOR_MOVE, 0x100);
					if (MatchTemplate(Capture(NewRectClient), TargetImage, &MatchInfo))
					{
						MouseEvent(MatchInfo.Location + Point{ NewRectClient.left, NewRectClient.top }, DLEFT_CLICK);
						KeybdEvent({ VK_RETURN, VK_RETURN });
					}
					else
					{
						break;
					}
				}
			}

			goto __RETURN;

			// 소비템 넣기(수큐)
			MouseEvent({ 740, 290 }, LEFT_CLICK);
			for (int i = 0; i < 3; i++)
			{
				MouseEvent(POS_VOID, CURSOR_MOVE, 0x100);
				if (MatchTemplate(Capture(NewRectClient), TargetImageItemCube, &MatchInfo))
				{
					MouseEvent(MatchInfo.Location + Point{ NewRectClient.left, NewRectClient.top }, DLEFT_CLICK);
					KeybdEvent({ VK_RETURN, VK_RETURN });
				}
				else
				{
					break;
				}
			}

			// 돈 넣기
			MouseEvent({ 715, 600 }, LEFT_CLICK);
			KeybdEvent(VK_RETURN);

			__RETURN:
		
			// 창고 창 닫기

			MouseEvent({ 640, 230 }, LEFT_CLICK);
			MouseEvent(POS_VOID, LEFT_CLICK);
		}
	}
};