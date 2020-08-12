#pragma once
#include <Windows.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <initializer_list>
#include <thread>
#include <mutex>
using namespace std;

#include <chrono>
using namespace chrono;

#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#define TARGET_DIR				"target//"
#define SNAP_DIR					"log//"
#define CONF_DIR					"conf//"
#define TMP_DIR					"tmp//"
#define USERCONF_FILENAME	"user.conf"

#define IPMANAGER_PATH		"renouveler_ip.py"
#define USERCONF_PATH			CONF_DIR USERCONF_FILENAME

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

#define IS_FLAG_ON(_flags, _flag)	((bool)((_flags) & _flag))
#define ARE_FLAGS_ON(_flags)	((bool)(_flags))
#define ARE_FLAGS_OFF(_flags)	(!(bool)(_flags))
#define FALG(_flags, _flag)			((_flags) |= (flag))
#define UNFLAG(_flags, _flag)		((_flags) &= ~(_flag))