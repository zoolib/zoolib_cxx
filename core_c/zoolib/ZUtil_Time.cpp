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

#include "zoolib/Stringf.h"

#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZCompat_cmath.h" // For fmod
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZBlockZero
#include "zoolib/ZStream.h"

#include <stdio.h>
#include <time.h>

#if ZCONFIG_SPI_Enabled(MacClassic) || ZCONFIG_SPI_Enabled(Carbon64)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,OSUtils.h)
#endif

#if ZCONFIG_SPI_Enabled(Win)
	#include "zoolib/ZCompat_Win.h"
#endif

#if ZCONFIG_SPI_Enabled(POSIX)
	#include <sys/time.h>
#endif

using std::string;

#if (__MWERKS__ && (!defined(_MSL_USING_MW_C_HEADERS) || _MSL_USING_MW_C_HEADERS)) \
	|| (ZCONFIG_SPI_Enabled(Win) && ZCONFIG(Compiler, GCC)) \
	|| (ZCONFIG_SPI_Enabled(Win) && ZCONFIG(Compiler, MSVC))
	#define DO_IT_OURSELVES 1
#else
	#define DO_IT_OURSELVES 0
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZUtil_Time

#if DO_IT_OURSELVES
// Return 1 if iYear (1900-based) is a leap year. 0 Otherwise
static int spLeapYear(int iYear)
	{
	if (iYear % 4)
		{
		// It's not a multiple of 4 --> is not a leap year
		return 0;
		}

	if (((iYear + 1900) % 400) == 0)
		{
		// Is a multiple of 4 and is a multiple of 400 --> is a leap year.
		return 1;
		}

	if ((iYear % 100) == 0)
		{
		// Is a multiple of 4, and not a multiple of 400,
		// and is a multiple of 100 --> is not a leap year.
		return 0;
		}

	// Is a multiple 4, and not a multiple of 400 and not a multiple of 100 --> is a leap year.
	return 1;
	}

static const uint32 kSecondsPerMinute = 60;
static const uint32 kSecondsPerHour = 60 * kSecondsPerMinute;
static const uint32 kSecondsPerDay = 24 * kSecondsPerHour;
static const uint32 spMonthToDays[2][13] =
	{
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};
#endif // DO_IT_OURSELVES

static void spTimeToTM(time_t iTime_t, struct tm& oTM)
	{
	#if DO_IT_OURSELVES

		// MSL assumes time_t values are in the local timezone, so it applies
		// the local-->UTC transform when gmtime is called. As MSL doesn't
		// support the timezone name field in struct tm it's simpler if we
		// just do the work ourselves.

		// iTime_t is 1970-based, so convert to 1900-based.
		uint32 timeSince1900 = iTime_t + ZTime::kEpochDelta_1900_To_1970;

		uint32 days = uint32(timeSince1900) / kSecondsPerDay;
		uint32 seconds = uint32(timeSince1900) % kSecondsPerDay;

		// January 1, 1900 was a Monday.
		oTM.tm_wday = (days + 1) % 7;

		uint32 years = 0;

		for (;;)
			{
			// AG 2003-10-28. This is taken from MSL, and it seems
			// incredibly ugly to have to iterate (as of 2005) 105 times
			// to figure out the number of years/number of days represented
			// by iTime_t.
			uint32 daysThisYear = spLeapYear(years) ? 366 : 365;

			if (days < daysThisYear)
				break;

			days -= daysThisYear;
			++years;
			}

		oTM.tm_year = years;
		oTM.tm_yday = days;

		uint32 months = 0;

		uint32 isLeapYear = spLeapYear(years);

		for (;;)
			{
			uint32 daysThruThisMonth = spMonthToDays[isLeapYear][months + 1];

			if (days < daysThruThisMonth)
				{
				days -= spMonthToDays[isLeapYear][months];
				break;
				}

			++months;
			}

		oTM.tm_mon = months;
		oTM.tm_mday = days + 1;

		oTM.tm_hour = seconds / kSecondsPerHour;

		seconds = seconds % kSecondsPerHour;

		oTM.tm_min = seconds / kSecondsPerMinute;
		oTM.tm_sec = seconds % kSecondsPerMinute;

		oTM.tm_isdst = 0;

	#else

		::gmtime_r(&iTime_t, &oTM);

	#endif
	}

static time_t spTMToTime(const struct tm& iTM)
	{
	time_t result;

	#if DO_IT_OURSELVES

		// MSL's mktime does no timezone conversion.
		result = mktime(const_cast<struct tm*>(&iTM));

 	#elif defined(__sun__) || defined (__ANDROID__)

		// Sun doesn't provide timegm, assume things are going to work.
		//#warning "CHECK THIS"
		ZAssert(iTM.tm_isdst == 0);
		result = mktime(const_cast<struct tm*>(&iTM));

 	#else

		// timegm assumes iTM is in UTC.
		result = timegm(const_cast<struct tm*>(&iTM));

	#endif

	// MSL may not use the correct epoch start
	//#warning "check this"
	return result;
//	return sFromSystemEpoch(result);
	}

#if DO_IT_OURSELVES
// MSL's strftime, in CW 8 and CW 9, does not handle timezone fields very well.
// On POSIX you call gmtime or localtime and end up with a tm structure
// containing a timezone value and a pointer to the textual name of that
// timezone, for use by %z and %Z respectively. On MSL you just get the
// current timezone value for %z, and usually an empty string for %Z.
// So we handle the %z and %Z fields ourselves.

static string spFormatTimeUTC(const struct tm& iTM, const string& iFormat)
	{
	string realFormat;
	realFormat.reserve(iFormat.size());

	for (string::const_iterator ii = iFormat.begin(); ii != iFormat.end(); ++ii)
		{
		if (*ii == '%')
			{
			++ii;
			if (ii == iFormat.end())
				break;
			if (*ii == 'z')
				{
				realFormat+= "+0000";
				}
			else if (*ii == 'Z')
				{
				realFormat += "UTC";
				}
			else
				{
				realFormat += '%';
				realFormat += *ii;
				}
			}
		else
			{
			realFormat += *ii;
			}
		}

	char buf[256];
	strftime(buf, 255, realFormat.c_str(), const_cast<struct tm*>(&iTM));
	return buf;
	}

static string spFormatTimeLocal(const struct tm& iTM, const string& iFormat, int iDeltaSeconds)
	{
	string realFormat;
	realFormat.reserve(iFormat.size());

	for (string::const_iterator ii = iFormat.begin(); ii != iFormat.end(); ++ii)
		{
		if (*ii == '%')
			{
			++ii;
			if (ii == iFormat.end())
				break;
			if (*ii == 'z')
				{
				int deltaMinutes = iDeltaSeconds / 60;
				char buf[6];
				if (deltaMinutes < 0)
					sprintf(buf, "-%02d%02d", (-deltaMinutes) / 60, (-deltaMinutes) % 60);
				else
					sprintf(buf, "+%02d%02d", deltaMinutes / 60, deltaMinutes % 60);
				realFormat += buf;
				}
			else if (*ii == 'Z')
				{
				// Ignore the Z field, just like MSL.
				}
			else
				{
				realFormat += '%';
				realFormat += *ii;
				}
			}
		else
			{
			realFormat += *ii;
			}
		}

	char buf[256];
	strftime(buf, 255, realFormat.c_str(), const_cast<struct tm*>(&iTM));
	return buf;
	}
#endif // DO_IT_OURSELVES

/** iTime is assumed to be UTC-based, return a textual version
of the date/time in UTC. */
string ZUtil_Time::sAsStringUTC(ZTime iTime, const string& iFormat)
	{
	if (not iTime)
		return "invalid";

	struct tm theTM;
	spTimeToTM(time_t(iTime.fVal), theTM);

	#if DO_IT_OURSELVES

		return spFormatTimeUTC(theTM, iFormat);

	#else

		char buf[256];
		strftime(buf, 255, iFormat.c_str(), &theTM);
		return buf;

	#endif
	}

#if DO_IT_OURSELVES
static int spUTCToLocalDelta()
	{
	#if ZCONFIG_SPI_Enabled(POSIX)

		struct timezone zone;

		if (::gettimeofday(nullptr, &zone) == -1)
			return 0;

		// tz_minuteswest is the number of minutes to be added to local
		// time to get UTC. sUTCDelta is the number of seconds to be added to UTC
		// to get the local time. So we multiply by negative 60.
		if (zone.tz_dsttime)
			{
			// tz_minuteswest does not itself account for DST.
			return -60 * (zone.tz_minuteswest - 60);
			}
		else
			{
			return -60 * zone.tz_minuteswest;
			}

	#elif ZCONFIG_SPI_Enabled(MacClassic) || ZCONFIG_SPI_Enabled(Carbon)

		MachineLocation loc;
		int delta = 0;

		::ReadLocation(&loc);

		// Don't bother if the user has not set the location
		if (loc.latitude != 0 && loc.longitude != 0 && loc.u.gmtDelta != 0)
			{
			delta = loc.u.gmtDelta & 0x00FFFFFF;

			if (delta & 0x00800000)
				delta |= 0xFF000000;
			}
		return delta;

	#elif ZCONFIG_SPI_Enabled(Win)

		TIME_ZONE_INFORMATION tzi;
		int delta = 0;
		switch (::GetTimeZoneInformation(&tzi))
			{
			case TIME_ZONE_ID_UNKNOWN:
				{
				delta = tzi.Bias;
				break;
				}
			case TIME_ZONE_ID_STANDARD:
				{
				delta = tzi.Bias + tzi.StandardBias;
				break;
				}
			case TIME_ZONE_ID_DAYLIGHT:
				{
				delta = tzi.Bias + tzi.DaylightBias;
				break;
				}
			}
		return -60 * delta;

	#else

		#error Unsupported platform

	#endif
	}
#endif // DO_IT_OURSELVES

/** iTime is assumed to be UTC-based, return a textual version
of the date/time in the local time zone. */
string ZUtil_Time::sAsStringLocal(ZTime iTime, const string& iFormat)
	{
	if (not iTime)
		return "invalid";

	struct tm theTM;

	#if DO_IT_OURSELVES

		int delta = spUTCToLocalDelta();
		time_t localTime = time_t(iTime.fVal + delta);
		spTimeToTM(localTime, theTM);
		return spFormatTimeLocal(theTM, iFormat, delta);

	#else

		time_t theTimeT = time_t(iTime.fVal);
		::localtime_r(&theTimeT, &theTM);

		char buf[256];
		strftime(buf, 255, iFormat.c_str(), &theTM);
		return buf;

	#endif
	}

static string spYmdHM(ZTime iTime, bool iIncludeT)
	{
	if (iIncludeT)
		return ZUtil_Time::sAsStringUTC(iTime, "%Y-%m-%dT%H:%M:");
	else
		return ZUtil_Time::sAsStringUTC(iTime, "%Y-%m-%d %H:%M:");
	}

string ZUtil_Time::sAsString_ISO8601(ZTime iTime, bool iIncludeT)
	{
	// We've got about 10 significant digits in year (-9,999 to +9,999),
	// month, day, hour, minutes, and seconds, and no more than 17 in a double.
	// To get a leading zero in the seconds' tens column we add 100
	// to the count of seconds, so to get up to 7 digits in the fraction we
	// need to allow ten digits overall.
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%.10g", 100.0 + fmod(iTime.fVal, 60)).substr(1);
	}

string ZUtil_Time::sAsString_ISO8601_s(ZTime iTime, bool iIncludeT)
	{
	if (iIncludeT)
		return ZUtil_Time::sAsStringUTC(iTime, "%Y-%m-%dT%H:%M:%S");
	else
		return ZUtil_Time::sAsStringUTC(iTime, "%Y-%m-%d %H:%M:%S");
	}

string ZUtil_Time::sAsString_ISO8601_ms(ZTime iTime, bool iIncludeT)
	{
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%06.3f", fmod(iTime.fVal, 60));
	}

string ZUtil_Time::sAsString_ISO8601_us(ZTime iTime, bool iIncludeT)
	{
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%09.6f", fmod(iTime.fVal, 60));
	}

 ZTime ZUtil_Time::sFromString_ISO8601(const string& iString)
 	{
 	const char* theCString = iString.c_str();
	struct tm theTM = {};

	bool gotIt = false;
	double seconds;
	if (6 == sscanf(theCString, "%04d-%02d-%02dT%02d:%02d:%lf",
		&theTM.tm_year, &theTM.tm_mon, &theTM.tm_mday,
		&theTM.tm_hour, &theTM.tm_min, &seconds))
		{
		theTM.tm_year -= 1900;
		theTM.tm_mon -= 1;
		gotIt = true;
		}
	else if (6 == sscanf(theCString, "%04d-%02d-%02d %02d:%02d:%lf",
		&theTM.tm_year, &theTM.tm_mon, &theTM.tm_mday,
		&theTM.tm_hour, &theTM.tm_min, &seconds))
		{
		theTM.tm_year -= 1900;
		theTM.tm_mon -= 1;
		gotIt = true;
		}

	if (gotIt)
		{
		time_t theTime = spTMToTime(theTM);
		return seconds + theTime;
		}
	return ZTime();
	}

} // namespace ZooLib
