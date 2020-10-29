#pragma once
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
#include <map>
using namespace std;
using namespace cv;

#include <chrono>
using namespace chrono;

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
string ErreurDernière; // Pour l'erreur du niveau bas 

template <class F1, class F2, class F3>
struct overload_set : F1, F2, F3
{
	overload_set(F1 f1, F2 f2, F3 f3)
		: F1(f1), F2(f2), F3(f3)
	{}

	using F1::operator();
	using F2::operator();
	using F3::operator();
};

template <class F1, class F2, class F3>
overload_set<F1, F2, F3> overload(F1 f1, F2 f2, F3 f3)
{
	return overload_set<F1, F2, F3>(f1, f2, f3);
}

#define IS_FLAG_ON(_flags, _flag)	((bool)((_flags) & _flag))
#define ARE_FLAGS_ON(_flags)	((bool)(_flags))
#define ARE_FLAGS_OFF(_flags)	(!(bool)(_flags))
#define FALG(_flags, _flag)			((_flags) |= (flag))
#define UNFLAG(_flags, _flag)		((_flags) &= ~(_flag))

#define VRAI (1)
#define FAUX (0)
#define ERREUR (-1)
#define __RENDRE_VRAI ErreurDernière = "성공"; return VRAI;
#define __RENDRE_FAUX ErreurDernière = "실패(오류없음)"; return FAUX;
#define __RENDRE_ERREUR(_s) ErreurDernière = _s; return ERREUR;

//#define BUILD_URUS
#define BUILD_BOSS
//#define BUILD_COMPTE
#define INGAME_DIRECT