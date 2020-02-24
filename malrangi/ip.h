#pragma once
#include "pathenv.h"

class IPCONF
{
public:
	typedef struct _INTERNET_INFO
	{
		struct _WANMAC_ADDR
		{
			int HADDR5;
			int HADDR6;
		};
		_WANMAC_ADDR MacAddr;
		bool IsIpValid;
	}INTERNET_INFO;
	typedef INTERNET_INFO::_WANMAC_ADDR WANMAC_ADDR;

	INTERNET_INFO InternetInfo;

private:
	IPCONF()
	{
		ifstream File;
		File.open(IPCONF_PATH, ios::in);
		if (File.is_open())
		{
			if (File.peek() == std::ifstream::traits_type::eof())
			{
				InternetInfo.IsIpValid = false;
			}
			else
			{
				File >> InternetInfo.MacAddr.HADDR6;
				InternetInfo.IsIpValid = (InternetInfo.MacAddr.HADDR6 >= 9) ? true : false;
			}
			File.close();
		}
		else
		{
			InternetInfo.IsIpValid = false;
		}
	}

public:
	static IPCONF* GetInstance(void)
	{
		if (nullptr == Instance)
		{
			Instance = new IPCONF();
		}
		return Instance;
	}
	void Destroy(void)
	{
		if (nullptr != Instance)
		{
			delete Instance;
			Instance = nullptr;
		}
	}
private:
	static IPCONF* Instance;
};
IPCONF* IPCONF::Instance = nullptr;