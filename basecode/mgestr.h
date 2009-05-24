#ifndef _MGE_STR_H_
#define _MGE_STR_H_

#include "basedefs.h"
#include <string>

/// @file mgestr.h This file defines some basic string types
/// @todo DCHAR, define own string manipulation functions or macros (perhaps just wrappers around stl)

namespace firc
{
#ifdef MGE_UNICODE
	typedef std::wstring dstring;
//	#define mge_dstring std::wstring;
#else
	typedef std::string dstring;
//	#define mge_dstring std::string;
#endif
}

#ifdef MGE_UNICODE
	#define _D(x) (L#x)
#else
	#define _D(x) (x)
#endif

#endif // _MGE_STR_H_
