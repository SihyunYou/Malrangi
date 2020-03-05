#pragma once
#include "client.h"
#include <thread>

#pragma comment(lib,"winmm.lib")


class Bot
{
	struct BUF
	{
		BYTE Key;
		DWORD Delay;
		milliseconds Cooltime;
		system_clock::time_point LastRenewedTime;
	};

public:
	Bot()
	{
		try
		{
			VecTargetImage.push_back(Cvw::Read(TARGET_DIR "violetta.png"));
		}
		catch (EmptyMatException)
		{
			throw;
		}

		ClientApi::SET_CLIENT_STDPOS();
	}
	~Bot()
	{
		ThreadMatch.join();
	}

public:
	void Play()
	{
		ThreadMatch = thread(
			[this]()
			{
				while (true)
				{
					try
					{
						Mat SourceImage = Cvw::Capture(ClientApi::RECT_CLIENT4);
						for each (const auto & TargetImage in VecTargetImage)
						{
							Cvw::UnmatchTemplate(SourceImage, TargetImage);
						}
						Sleep(0x100);
					}
					catch (MatchSuccessedException)
					{
						PlaySoundA("gibbon-monkey-daniel_simon.wav", NULL, SND_ASYNC);
						Sleep(0x1600);
					}
				}
			}
		);
		VecBuf.push_back({ '2', 1000, seconds(180), system_clock::now() });
		VecBuf.push_back({ '3', 1200, seconds(60), system_clock::now() });
		VecBuf.push_back({ 'E', 800, seconds(3), system_clock::now() });
		VecBuf.push_back({ 'R', 1300, milliseconds(8100), system_clock::now() });
		
		BYTE CurrentDir = VK_LEFT;
		bool IsFirstLoop = true;
		int i = 0;
		while (true)
		{
			for(auto& Buf : VecBuf)
			{
				if (duration_cast<seconds>(system_clock::now() - Buf.LastRenewedTime) > Buf.Cooltime || IsFirstLoop)
				{
					Buf.LastRenewedTime += duration_cast<seconds>(system_clock::now() - Buf.LastRenewedTime);
					KeybdEvent(Buf.Key, Buf.Delay);
				}
			}
			IsFirstLoop = false;

			if (i % 32 == 0)
			{
				KeybdEventContinued(CurrentDir = (CurrentDir == VK_LEFT) ? VK_RIGHT : VK_LEFT, 100);
			}
			else
			{
				Sleep(100);
			}
			++i;
		}
	}

private:
	vector<BUF> VecBuf;
	thread ThreadMatch;
	vector<Mat> VecTargetImage;
};