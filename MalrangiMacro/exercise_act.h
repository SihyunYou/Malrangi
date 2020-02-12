#pragma once
#include "exercise.h"

namespace Exc
{
	enum class JUMP_TYPE
	{
		CYGNUS,
		DEMON,
		NOVA,
		V_MATRIX,
		ZERO,
		WIZARD,
	};
	void Jump(
		JUMP_TYPE JumpType,
		DWORD MillisecondsRestTime = 1000)
	{
		switch (JumpType)
		{
		case JUMP_TYPE::DEMON:
			KeybdEvent(VK_MENU, 60);
			KeybdEvent(VK_UP, 60);
			KeybdEvent(VK_UP, 2500);
			break;

		case JUMP_TYPE::CYGNUS:
			KeybdEvent(VK_MENU, 60);

			KeybdEventDown(VK_UP);
			KeybdEvent(VK_MENU, 60);
			KeybdEventUp(VK_UP);
			break;

		case JUMP_TYPE::NOVA:
			KeybdEvent('C', 2000);
			break;

		case JUMP_TYPE::V_MATRIX:
			KeybdEvent('L', 180);
			break;

		case JUMP_TYPE::ZERO:
			KeybdEventDown(VK_UP);
			KeybdEvent('F', 100);
			KeybdEventUp(VK_UP);
			break;

		case JUMP_TYPE::WIZARD:
			KeybdEventDown(VK_UP);
			KeybdEvent(VK_SHIFT, 100);
			KeybdEventUp(VK_UP);
			break;
		}
		Sleep(MillisecondsRestTime);
	}
	void DownJump(
		DWORD MilliSecondsRestTime = 1000)
	{
		keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), 0, 0);
		KeybdEvent(VK_MENU, MilliSecondsRestTime);
		keybd_event(VK_DOWN, MapVirtualKey(VK_DOWN, 0), KEYEVENTF_KEYUP, 0);
	}

	/****************************************************************************
* Recognizing
****************************************************************************/

	class MinimapRecognizer
	{
	public:
		class CharacterNotFoundException : public CvWrappedException
		{
		public:
			CharacterNotFoundException::CharacterNotFoundException(void) :
				CvWrappedException(__CLASSNAME__ + "A specified object cannot be recognized in the minimap.\n") {}
			virtual const char* CharacterNotFoundException::what(void)
			{
				return Message.c_str();
			}
		};

	public:
		MinimapRecognizer::MinimapRecognizer(
			RECT RectMinimap) :
			RectMinimap(RectMinimap)
		{
			;
		}
		MinimapRecognizer operator=(
			RECT RectMinimap)
		{
			return MinimapRecognizer(RectMinimap);
		}

	protected:
		enum class OBJECT
		{
			YELLOW,
			RED,
		};

	public:
		void MoveToRelativeCriteria(
			INT MinimapRelativeCriteria)
		{
			auto GetCurrentMiniMapPosition = [this](MinimapRecognizer::OBJECT ObjectToGet) ->INT
			{
				try
				{
					Mat SourceMiniMapImage = Cvw::Capture(RectMinimap, IMREAD_COLOR);
					//Cvw::Show(SourceMiniMapImage);
					Mat Mask;
					if (OBJECT::YELLOW == ObjectToGet)
					{
						Cvw::InRange(SourceMiniMapImage, Scalar(20, 235, 245), Scalar(40, 245, 255), Mask);
					}
					//Cvw::Show(Mask);
					for (int y = 0; y < Mask.rows; ++y)
					{
						for (int x = 0; x < Mask.cols; ++x)
						{
							if (Mask.at<BYTE>(y, x) == 255)
							{
								return x;
							}
						}
					}
				}
				catch (EmptyMatException & cwexception)
				{
					throw;
				}
				catch (IntegerDivisionByZeroException & cwexception)
				{
					throw;
				}

				throw MinimapRecognizer::CharacterNotFoundException();
			};
			INT RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria;

			if (RelativeDistance < 0)
			{

				KeybdEventDown(VK_RIGHT);
				while (RelativeDistance <= 0)
				{
					std::cout << GetCurrentMiniMapPosition(OBJECT::YELLOW) << endl;
					RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria + 2;
				}
				KeybdEventUp(VK_RIGHT);
			}
			else if (RelativeDistance > 0)
			{
				KeybdEventDown(VK_LEFT);
				while (RelativeDistance >= 0)
				{
					std::cout << GetCurrentMiniMapPosition(OBJECT::YELLOW) << endl;
					RelativeDistance = GetCurrentMiniMapPosition(OBJECT::YELLOW) - MinimapRelativeCriteria - 2;
				}
				KeybdEventUp(VK_LEFT);
			}
		}

	private:
		RECT RectMinimap;
	};
}
namespace EXC = Exc;