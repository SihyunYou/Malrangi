#pragma once
#include "malrangi.h"
#include "ip.h"

class IpManage
{
public:
	void Renew(IPCONF::INTERNET_INFO& InternetInfo)
	{
		ofstream File(IPCONF_PATH, ios::out | ios::trunc);
		if (InternetInfo.IsIpValid)
		{
			if (99 == InternetInfo.MacAddr.HADDR6)
			{
				InternetInfo.MacAddr.HADDR6 = 10;
			}
			else
			{
				++InternetInfo.MacAddr.HADDR6;
			}
		}
		else
		{
			InternetInfo.MacAddr.HADDR6 = 10;
		}

		ShellExecuteA(NULL, "open", IPMANAGER_PATH, to_string(InternetInfo.MacAddr.HADDR6).c_str(), NULL, SW_SHOW);
		File << InternetInfo.MacAddr.HADDR6;
		File.close();
	}
};