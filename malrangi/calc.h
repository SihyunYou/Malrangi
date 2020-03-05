#pragma once
#include "boss.h"
#include "bridge.h"

class Calc : public Boss
{
public:
	void PlayFromZacum(
		const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		Cross(Z2, Z1);
		Cross(Z1, Z2, 2);
		Cross(Z2, MeisterVill);
		Cross(MeisterVill, FreeMarket);
		Play(MapleIdInfo);
		Cross(FreeMarket, MeisterVill);
		Cross(MeisterVill, Z2);
	}
	void PlayFromUrus(
		const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		Cross(Urus, Elnas);
		Cross(Elnas, FreeMarket);
		Play(MapleIdInfo);
		Cross(FreeMarket, ElnasMarket);
		Cross(ElnasMarket, Elnas);
		Cross(Elnas, Urus);
	}

private:
	void Play(
		const USERCONF::MAPLEID_INFO& MapleIdInfo)
	{
		static const Mat TargetImageItemCrystal(Cvw::Read(TARGET_DIR "item_crystal_of_boss.jpg"));
		static const Mat TargetImageItem100lv(Cvw::Read(TARGET_DIR "item_100lv.jpg"));
		static const Mat TargetImageItem110lv(Cvw::Read(TARGET_DIR "item_110lv.jpg"));
		static const Mat TargetImageItemCube(Cvw::Read(TARGET_DIR "item_cube.jpg"));


		/*** ������ �ȱ�(�ݷ���) ***/
		MouseEvent(980, 334, LEFT_CLICK);

		// ���â
		for (int i = 0; i < 40; i++)
		{
			MouseEvent(735, 279, RIGHT_CLICK, 300);
		}

		// ��Ÿâ
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


		/*** â�� ������ �ֱ�  ***/
		if (0 < CountCrystal)
		{
			MouseEvent(310, 334, LEFT_CLICK);
			ClientApi::UnlockSecondPassword(MapleIdInfo.SecondPassword);

			// ����� �ֱ�(�����ƽ + ����)
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

			// �Һ��� �ֱ�(��ť)
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

			// �� �ֱ�
			MouseEvent(715, 574, LEFT_CLICK);
			KeybdEvent(VK_RETURN);

			// â�� â �ݱ�
			MouseEvent(640, 204, LEFT_CLICK);
		}
	}
};