// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Time_h__
#define __ZooLib_Util_Time_h__ 1
#include "zconfig.h"

#include "zoolib/Time.h"

#include <string>

namespace ZooLib {
namespace Util_Time {

// =================================================================================================
#pragma mark - Util_Time

std::string sAsStringUTC(double iTime, const std::string& iFormat);
std::string sAsStringLocal(double iTime, const std::string& iFormat);

// Although these methods, by their names, claim compliance with ISO-8601, they really
// only deal with a subset. They do not address timezones, non 4-digit years etc. Basically,
// they work with strings of the form "YYYY-mm-ddTHH:MM:SS" or "YYYY-mm-dd HH:MM:SS", where
// SS can include a fractional value.

std::string sAsString_ISO8601(double iTime, bool iIncludeT);
std::string sAsString_ISO8601_s(double iTime, bool iIncludeT);
std::string sAsString_ISO8601_ms(double iTime, bool iIncludeT);
std::string sAsString_ISO8601_us(double iTime, bool iIncludeT);

double sFromString_ISO8601(const std::string& iString);

} // namespace Util_Time
} // namespace ZooLib

#endif // __ZooLib_Util_Time_h__
