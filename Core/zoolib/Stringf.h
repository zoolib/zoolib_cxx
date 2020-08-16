// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Stringf_h__
#define __ZooLib_Stringf_h__ 1
#include "zconfig.h"

#include <cstdarg>
#include <string>

// =================================================================================================

namespace ZooLib {

std::string sStringf(std::string iString, ...);

std::string sStringf(const char* iString, ...) ZMACRO_Attribute_Format_Printf(1,2);

std::string sStringv(const char* iString, std::va_list iArgs);

} // namespace ZooLib

#endif // __ZooLib_Stringf_h__
