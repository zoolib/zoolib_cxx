// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Time_h__
#define __ZooLib_Time_h__ 1
#include "zconfig.h"

#include "zoolib/ZTypes.h" // For __int64

namespace ZooLib {

// =================================================================================================
#pragma mark - Time

namespace Time {

double sNow();
double sSystem();
double sAtBoot();

double sSinceBoot();

const __int64 kSecond = 1;
const __int64 kMinute = 60 * kSecond;
const __int64 kHour = 60 * kMinute;
const __int64 kDay = 24 * kHour;
const __int64 kWeek = 7 * kDay;
const __int64 kYear = 365 * kDay;

const __int64 kEpochDelta_1900_To_1904 = 4 * kYear; // 126,144,000
const __int64 kEpochDelta_1904_To_1970 = 66 * kYear + 17 * kDay; // 2,082,844,800
const __int64 kEpochDelta_1900_To_1970 = 70 * kYear + 17 * kDay; // 2,208,988,800
const __int64 kEpochDelta_1601_To_1970 = 369 * kYear + 89 * kDay; // 11,644,473,600

const __int64 kEpochDelta_1970_To_2001 = -978307200;
const __int64 kEpochDelta_2001_To_1970 = 978307200;

} // namespace Time
} // namespace ZooLib

#endif // __ZooLib_Time_h__
