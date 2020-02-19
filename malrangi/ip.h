#pragma once
#include "low_util.h"

void RenewIPAddress(
	void)
{
	static INT HADDR6 = 40;
	ShellExecuteA(NULL, "open", "ip_renewer.js", to_string(HADDR6++).c_str(), NULL, SW_SHOW);
}