// Copyright (c) 2015-21 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Time_h__
#define __ZooLib_Time_h__ 1
#include "zconfig.h"

#include <chrono>

namespace ZooLib {

// =================================================================================================
#pragma mark - Time

namespace Time {

typedef std::chrono::duration<double,std::ratio<1>> Duration;

inline double sNow()
	{ return Duration(std::chrono::system_clock::now().time_since_epoch()).count(); }

inline double sSystem()
	{ return Duration(std::chrono::steady_clock::now().time_since_epoch()).count(); }

//double sAtBoot();
//double sSinceBoot();

const long long kSecond = 1;
const long long kMinute = 60 * kSecond;
const long long kHour = 60 * kMinute;
const long long kDay = 24 * kHour;
const long long kWeek = 7 * kDay;
const long long kYear = 365 * kDay;

const long long kEpochDelta_1900_To_1904 = 4 * kYear; // 126,144,000
const long long kEpochDelta_1904_To_1970 = 66 * kYear + 17 * kDay; // 2,082,844,800
const long long kEpochDelta_1900_To_1970 = 70 * kYear + 17 * kDay; // 2,208,988,800
const long long kEpochDelta_1601_To_1970 = 369 * kYear + 89 * kDay; // 11,644,473,600

const long long kEpochDelta_1970_To_2001 = -978307200;
const long long kEpochDelta_2001_To_1970 = 978307200;

} // namespace Time
} // namespace ZooLib

#endif // __ZooLib_Time_h__
