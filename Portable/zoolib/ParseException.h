// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ParseException_h__
#define __ZooLib_ParseException_h__ 1
#include "zconfig.h"

#include <stdexcept>
#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ParseException

class ParseException : public std::runtime_error
	{
public:
	ParseException(const std::string& iWhat)
	:	runtime_error(iWhat)
		{}

	ParseException(const char* iWhat)
	:	runtime_error(iWhat)
		{}
	};

ZMACRO_NoReturn_Pre
inline bool sThrow_ParseException(const std::string& iMessage);
ZMACRO_NoReturn_Post

inline bool sThrow_ParseException(const std::string& iMessage)
	{ throw ParseException(iMessage); }

} // namespace ZooLib

#endif // __ZooLib_ParseException_h__
