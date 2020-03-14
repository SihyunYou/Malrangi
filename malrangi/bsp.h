#pragma once
#include "client.h"
#include "tesseract-binary-wrap.h"

class BasePlay
{
public:
	
};

class BossException : public MalrangiException
{
public:
	BossException(void) :
		MalrangiException(__CLASSNAME__) {}
	BossException(string Description) :
		MalrangiException(__CLASSNAME__ + "! " + Description) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};