/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZUtil_Time__
#define __ZUtil_Time__ 1
#include "zconfig.h"

#include "zoolib/ZTime.h"

#include <string>

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Time

namespace ZUtil_Time {

std::string sAsStringUTC(ZTime iTime, const std::string& iFormat);
std::string sAsStringLocal(ZTime iTime, const std::string& iFormat);


// Although these methods, by their names, claim compliance with ISO-8601, they really
// only deal with a subset. They do not address timezones, non 4-digit years etc. Basically,
// they work with strings of the form "YYYY-mm-ddTHH:MM:SS" or "YYYY-mm-dd HH:MM:SS", where
// SS can include a fractional value.

std::string sAsString_ISO8601(ZTime iTime, bool iIncludeT);
std::string sAsString_ISO8601_s(ZTime iTime, bool iIncludeT);
std::string sAsString_ISO8601_us(ZTime iTime, bool iIncludeT);
std::string sAsString_ISO8601_ms(ZTime iTime, bool iIncludeT);

ZTime sFromString_ISO8601(const std::string& iString);

} // namespace ZUtil_Time

#endif // __ZUtil_Time__
