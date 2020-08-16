// Copyright (c) 2009-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UnicodeString_h__
#define __ZooLib_UnicodeString_h__ 1
#include "zconfig.h"

#include "zoolib/UnicodeCU.h"
#include "zoolib/UnicodeString8.h"

namespace ZooLib {

/// A basic_string specialization that holds a sequence of UTF32 code units.
typedef std::basic_string<UTF32> string32;

/// A basic_string specialization that holds a sequence of UTF16 code units.
typedef std::basic_string<UTF16> string16;

} // namespace ZooLib

#endif // __ZooLib_UnicodeString_h__
