#pragma once
#include "client.h"
#include "tesseract-binary-wrap.h"

class BasePlay
{
public:
	
};

class AppException : public MalrangiException
{
public:
	AppException(void) :
		MalrangiException(__CLASSNAME__) {}
	AppException(string Description) :
		MalrangiException(__CLASSNAME__ + "! " + Description) {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}
};