#pragma once
#include "malrangi.h"

class IpManage
{

public:
	void Renew(void)
	{
		ShellExecuteA(NULL, "open", IPMANAGER_PATH, NULL, NULL, SW_SHOW);
		Sleep(8000);
	}
};