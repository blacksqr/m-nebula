#ifndef N_STL_H
#define N_STL_H

#ifndef NO_STLPORT
    #include "stlport/vector"
    #include "stlport/algorithm"
	#include "stlport/deque"
	#include "stlport/list"
	#include "stlport/string"
	#include "stlport/map"
	#include "stlport/stack"
#else 
    #include <vector>
    #include <algorithm>
	#include <deque>
	#include <list>
	#include <string>
	#include <map>
	#include <stack>
#endif

#ifdef ULONG_MAX
#define	UNSETU		ULONG_MAX
#else
#define	UNSETU		0xffffffffUL
#endif

typedef std::wstring				stl_wstring;
typedef std::deque<float>			floats_q;
typedef std::vector<long>			longs_v;
typedef std::deque<long>			longs_q;
typedef std::list<long>				longs_l;
typedef std::vector<short> 			shorts_t;

#define	STRING_RESERVE	256

class stl_string : public std::string
{
public:
	stl_string() { this->reserve(STRING_RESERVE); }

	stl_string(const char* s)
	{
		this->reserve(STRING_RESERVE);
		this->assign(s);
	}

	stl_string(const stl_string& s)
	{
		this->reserve(STRING_RESERVE);
		this->assign(s);
	}

	stl_string(const std::string& s)
	{
		this->reserve(STRING_RESERVE);
		this->assign(s);
	}
	
	stl_string& operator=(const stl_string& s)
	{
		this->assign(s); return *this;
	}

	stl_string& operator=(const std::string& s)
	{
		this->assign(s); return *this;
	}

	stl_string& operator=(const char* s)
	{
		this->assign(s); return *this;
	}

	const char* c_str() const
	{
		if (!this->empty())
			return std::string::c_str();
		else 
			return 0;
	}
};

typedef std::vector<stl_string> 	strings_v;

class byte_buffer : public std::vector<unsigned char> {	
public:
	unsigned char* get_buffer() {
		return &((*this)[0]);
	}
};

class char_buffer : public std::vector<char> {	
public:
	char* get_buffer() {
		return &((*this)[0]);
	}
};

#endif