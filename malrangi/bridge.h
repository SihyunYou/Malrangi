#pragma once
#include "client.h"

#define Cross(FromA, ToB, n) Move##From##FromA##To##ToB(n)
#define MakeBridge(FromA, ToB) void Move##From##FromA##To##ToB

MakeBridge(Urus, Elnas)(void)
{
	static const Mat TargetImageMinimapMarkElnas(Cvw::Read(TARGET_DIR "minimap_mark_elnas.jpg"));

	KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 4000);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkElnas, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
MakeBridge(Elnas, Office)(void)
{
	static const Mat TargetImageNpcRene(Cvw::Read(TARGET_DIR "npc_rene.jpg"));

	KeybdEventContinued(VK_RIGHT, 10);
	KeybdEvent('C', 1000);
	KeybdEvent('C', 1000);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	KeybdEvent('X', 1300);
	ClientApi::Jump(ClientApi::JUMP_T::DEMON);
	ClientApi::DownJump(1000);

	KeybdEvent(VK_UP);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcRene, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromOfficeToZ1(void)
{
	static const Mat TargetImageMinimapMarkZacum(Cvw::Read(TARGET_DIR "minimap_mark_zacum.jpg"));

	MouseEvent(482, 566, LEFT_CLICK);
	KeybdEvent(VK_RETURN);
	KeybdEvent(VK_RETURN);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkZacum, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ1ToZ2(int Level)
{
	const Mat TargetImageNpcAdobis(Cvw::Read(TARGET_DIR "npc_adobis.jpg"));
	bool IsFailedAgain = false;

	ClientApi::MinimapRecognizer Recognizer({ 10, 54, 190, 120 });
	if (Recognizer.WhereAmI() < 100)
	{
		Recognizer.MoveToRelativeCriteria(143);
	}

GET_EYE_OF_FIRE:
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
			goto GET_EYE_OF_FIRE;
		}
		throw MalrangiException("GetEyeOfFireFailedException");
	}

	KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 1200);
	for (int i = 0; i < (int)Level; i++)
	{
		KeybdEvent(VK_DOWN, 600);
	}
	KeybdEvent(VK_RETURN);
	try
	{
		Cvw::DoUntilMatchingTemplate(RECT{ 950, 500, 1050, 600 }, TargetImageNpcAdobis, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ2ToZ3()
{
	static const Mat TargetImageUnitZ3(Cvw::Read(TARGET_DIR "unit_z3.jpg"));
	bool IsFailedAgain = false;

	MOVE:
	MouseEvent(1000, 544, LEFT_CLICK, 600);
	KeybdEvent(VK_RETURN);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageUnitZ3, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		if (!IsFailedAgain)
		{
			IsFailedAgain = true;
			goto MOVE;
		}
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ3ToZ2(void)
{
	const const Mat TargetImageNpcAdobis(Cvw::Read(TARGET_DIR "npc_adobis.jpg"));

	ClientApi::BreakParty();
	try
	{
		Cvw::DoUntilMatchingTemplate(RECT{ 950, 500, 1050, 600 }, TargetImageNpcAdobis, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ2ToZ1(void)
{
	static const Mat TargetImageScluptureZ1(Cvw::Read(TARGET_DIR "sclupture_z1.jpg"));
	bool IsFailedAgain = false;

MOVE:
	KeybdEventContinuedWithSubKey(!IsFailedAgain ? VK_LEFT : VK_RIGHT, VK_UP, 1500);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageScluptureZ1, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		if (!IsFailedAgain)
		{
			IsFailedAgain = true;
			goto MOVE;
		}
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ1ToElnas(void)
{
	static const Mat TargetImageMinimapMarkElnas(Cvw::Read(TARGET_DIR "minimap_mark_elnas.jpg"));

	MouseEvent(820, 563, LEFT_CLICK);
	KeybdEvent(VK_DOWN, 400);
	KeybdEvent(VK_DOWN, 400);
	KeybdEvent(VK_DOWN, 400);
	KeybdEvent(VK_RETURN, 400);
	KeybdEvent(VK_RETURN, 400);
	KeybdEvent(VK_RETURN, 400);

	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkElnas, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromElnasToUrus(void)
{
	static const Mat TargetImagePictureUrusDimensionalMirror(Cvw::Read(TARGET_DIR "picture_urus_dimensional_mirror.jpg"));
	static const Mat TargetImageBackgroundUrus(Cvw::Read(TARGET_DIR "background_urus.jpg"));

	MouseEvent(58, 168, LEFT_CLICK);
	MouseEvent(566, 376, LEFT_CLICK);
	Cvw::ClickMatchedTemplate(Cvw::Capture(ClientApi::RECT_CLIENT4), TargetImagePictureUrusDimensionalMirror, LEFT_CLICK);
	MouseEvent(690, 596, LEFT_CLICK);

	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageBackgroundUrus, NONWORK, seconds(30));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
void MoveFromZ2ToMeisterVill(void)
{
	static const Mat TargetImageButtonMeisterVill(Cvw::Read(TARGET_DIR "button_meistervill.jpg"));
	static const Mat TargetImageMinimapMarkMeisterVill(Cvw::Read(TARGET_DIR "minimap_mark_meistervill.jpg"));

	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
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
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkMeisterVill, NONWORK, seconds(30));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x400);
	KeybdEvent(USERCONF::GetInstance()->VirtualKeyset.SpecialTechnology);
}
void MoveFromMeisterVillToFreeMarket(void)
{
	static const Mat TargetImageNpcMsBrainy(Cvw::Read(TARGET_DIR "npc_msbrainy.jpg"));

	ClientApi::DownJump(1500);
	ClientApi::DownJump(1500);
	KeybdEventContinued(VK_LEFT, 1000);
	Sleep(0x200);
	KeybdEventContinuedWithSubKey(VK_RIGHT, VK_UP, 2600);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMsBrainy, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}
}
void MoveFromFreeMarketToMeisterVill(void)
{
	static const Mat TargetImageMinimapMarkMeisterVill = Cvw::Read(TARGET_DIR "minimap_mark_meistervill.jpg");

	KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkMeisterVill, NONWORK, seconds(20));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x800);
}
MakeBridge(MeisterVill, Z2)(void)
{
	static const Mat TargetImageNpcAdobis(Cvw::Read(TARGET_DIR "npc_adobis.jpg"));
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
			seconds(20));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x400);
}
MakeBridge(Elnas, FreeMarket)(void)
{
	static const Mat TargetImageNpcMsBrainy(Cvw::Read(TARGET_DIR "npc_msbrainy.jpg"));

	MouseEvent(58, 168, LEFT_CLICK);
	MouseEvent(634, 378, LEFT_CLICK);
	MouseEvent(840, 462, LEFT_CLICK);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageNpcMsBrainy, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}

	Sleep(0x200);
}
MakeBridge(FreeMarket, ElnasMarket)(void)
{
	static const Mat TargetImageMinimapMarkElnas(Cvw::Read(TARGET_DIR "minimap_mark_elnas.jpg"));

	KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1500);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageMinimapMarkElnas, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}
}
MakeBridge(ElnasMarket, Elnas)(void)
{
	static const Mat TargetImageButtonMove(Cvw::Read(TARGET_DIR "button_move.jpg"));

	KeybdEventContinued(VK_LEFT, 3800);
	KeybdEventContinuedWithSubKey(VK_LEFT, VK_UP, 1400);
	try
	{
		Cvw::DoUntilMatchingTemplate(ClientApi::RECT_CLIENT4, TargetImageButtonMove, NONWORK, seconds(15));
	}
	catch (MatchFailedException)
	{
		throw MalrangiException(__FEWHAT__);
	}
}