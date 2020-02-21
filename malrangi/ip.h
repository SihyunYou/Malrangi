#pragma once
#include "low_util.h"

void RenewIPAddress(
	void)
{
	static INT HADDR6 = 20;
	ShellExecuteA(NULL, "open", IPMANAGER_FILENAME, to_string(HADDR6++).c_str(), NULL, SW_SHOW);


} 