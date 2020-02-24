#pragma once
#include "pathenv.h"

class IPCONF
{
public:
	IPCONF()
	{
		File.open(IPCONF_PATH, ios::in);
		if (File.is_open())
		{
			if (File.peek() == std::ifstream::traits_type::eof())
			{
				CurrentHADDR6 = 9;
			}
			else
			{
				File >> CurrentHADDR6;
				CurrentHADDR6 = ((CurrentHADDR6 >= 9) ? CurrentHADDR6 : 9);
			}
			File.close();
		}
		else
		{
			CurrentHADDR6 = 9;
		}
	}

public:
	void Renew(void)
	{
		ShellExecuteA(NULL, "open", IPMANAGER_PATH, to_string(++CurrentHADDR6).c_str(), NULL, SW_SHOW);

		File.open(IPCONF_PATH, ios::out | ios::trunc);
		File << CurrentHADDR6;
		File.close();
	}

private:
	int CurrentHADDR6;
	fstream File;
};