#pragma once
#include <string>
#include <exception>
using namespace std;

string GetClassTypeId(
	const char* lpFunctionMacro)
{
	string ClassTypeId = lpFunctionMacro;
	for (int i = ClassTypeId.length(); i >= 0; --i)
	{
		if (ClassTypeId[i] == ':')
		{
			return ClassTypeId.substr(0, i - 1);
		}
	}
}
#define __CLASSNAME__		(GetClassTypeId(__FUNCTION__))
#define __FEWHAT__			(__FUNCTION__ "FailedException")

class MalrangiException : public std::exception
{
public:
	MalrangiException(void) :
		Message(__CLASSNAME__) {}
	explicit MalrangiException(const char* Message) :
		Message(Message) {}
	explicit MalrangiException(std::string Message) :
		Message(Message) {}
	virtual ~MalrangiException() throw () {}
	virtual const char* what(void) const throw()
	{
		return Message.c_str();
	}

protected:
	std::string Message;
};