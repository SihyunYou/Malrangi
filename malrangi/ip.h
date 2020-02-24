#pragma once
#include "pathenv.h"

class IPCONF
{
public:
	IPCONF()
	{
		ifstream File;
		File.open(IPCONF_PATH, ios::in);
		if (File.is_open())
		{
			if (File.peek() == std::ifstream::traits_type::eof())
			{
				IsIpValid = false;
			}
			else
			{
				File >> CurrentHADDR6;
				IsIpValid = (CurrentHADDR6 >= 9) ? true : false;
			}
			File.close();
		}
		else
		{
			IsIpValid = false;
		}
	}
	void Renew(void)
	{
		ShellExecuteA(NULL, "open", IPMANAGER_PATH, to_string(++CurrentHADDR6).c_str(), NULL, SW_SHOW);

		ofstream File.open(IPCONF_PATH, ios::out | ios::trunc);
		File << CurrentHADDR6;
		File.close();
	}
public:
	int CurrentHADDR6;
	bool IsIpValid;
};