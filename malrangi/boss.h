#pragma once
#include "client.h"
#include "tesseract-binary-wrap.h"

class Boss
{
public:
	Boss() : SuccessCount(0), FailureCount(0) {}

public:
	int SuccessCount;
	int FailureCount;
};