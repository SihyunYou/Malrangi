#pragma once
#include "client.h"
#include <chrono>
using namespace chrono;

class Bot
{
public:
	struct BUF
	{
		BYTE Key;
		DWORD Delay;
		seconds Cooltime;
		system_clock::time_point LastRenewedTime;
	};
	vector<BUF> VecBuf;

	void Play()
	{
		BYTE CurrentDir = VK_LEFT;
	
		VecBuf.push_back({ '2', 1000, seconds(180), system_clock::now() });
		VecBuf.push_back({ '3', 1200, seconds(60), system_clock::now() });
		VecBuf.push_back({ 'E', 800, seconds(6), system_clock::now() });
		VecBuf.push_back({ 'R', 1300, seconds(8), system_clock::now() });

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

			if (i % 24 == 0)
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
};