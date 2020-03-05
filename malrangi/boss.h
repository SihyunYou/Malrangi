#pragma once
#include "client.h"
#include "tesseract-binary-wrap.h"

class Boss
{
public:
	Boss() : PlayCount(0), ExceptionCount(0) {}

public:
	int PlayCount;
	int ExceptionCount;
};