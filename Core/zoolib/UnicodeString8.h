// Copyright (c) 2012-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UnicodeString8_h__
#define __ZooLib_UnicodeString8_h__ 1
#include "zconfig.h"

#include "zoolib/UnicodeCU8.h"
#include "zoolib/Util_STL.h"

#include <string>

namespace ZooLib {

/** A basic_string specialization that holds a sequence of UTF8 code units.
It is almost certainly same type as std::string. */
typedef std::basic_string<UTF8> string8;

} // namespace ZooLib

#endif // __ZooLib_UnicodeString8_h__
