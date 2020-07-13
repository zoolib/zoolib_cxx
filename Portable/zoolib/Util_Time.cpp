// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_Time.h"

#include "zoolib/Compat_cmath.h" // For fmod
#include "zoolib/Stringf.h"

#include "zoolib/ZDebug.h"

#include <stdio.h>
#include <time.h>

#if ZCONFIG_SPI_Enabled(Win)
	#include "zoolib/ZCompat_Win.h"
#endif

#if ZCONFIG_SPI_Enabled(POSIX)
	#include <sys/time.h>
#endif

using std::string;

namespace ZooLib {

// =================================================================================================
#pragma mark - Util_Time

static void spTimeToTM(time_t iTime_t, struct tm& oTM)
	{
	::gmtime_r(&iTime_t, &oTM);
	}

static time_t spTMToTime(const struct tm& iTM)
	{
	time_t result;

	#if 0

 	#elif defined(__sun__) || defined (__ANDROID__)

		// Sun doesn't provide timegm, assume things are going to work.
		//#warning "CHECK THIS"
		ZAssert(iTM.tm_isdst == 0);
		result = mktime(const_cast<struct tm*>(&iTM));

 	#else

		// timegm assumes iTM is in UTC.
		result = timegm(const_cast<struct tm*>(&iTM));

	#endif

	return result;
	}

/** iTime is assumed to be UTC-based, return a textual version
of the date/time in UTC. */
string Util_Time::sAsStringUTC(double iTime, const string& iFormat)
	{
	if (not iTime)
		return "invalid";

	struct tm theTM;
	spTimeToTM(time_t(iTime), theTM);

	char buf[256];
	strftime(buf, 255, iFormat.c_str(), &theTM);
	return buf;
	}

/** iTime is assumed to be UTC-based, return a textual version
of the date/time in the local time zone. */
string Util_Time::sAsStringLocal(double iTime, const string& iFormat)
	{
	if (not iTime)
		return "invalid";

	struct tm theTM;

	time_t theTimeT = time_t(iTime);
	::localtime_r(&theTimeT, &theTM);

	char buf[256];
	strftime(buf, 255, iFormat.c_str(), &theTM);
	return buf;
	}

static string spYmdHM(double iTime, bool iIncludeT)
	{
	if (iIncludeT)
		return Util_Time::sAsStringUTC(iTime, "%Y-%m-%dT%H:%M:");
	else
		return Util_Time::sAsStringUTC(iTime, "%Y-%m-%d %H:%M:");
	}

string Util_Time::sAsString_ISO8601(double iTime, bool iIncludeT)
	{
	// We've got about 10 significant digits in year (-9,999 to +9,999),
	// month, day, hour, minutes, and seconds, and no more than 17 in a double.
	// To get a leading zero in the seconds' tens column we add 100
	// to the count of seconds, so to get up to 7 digits in the fraction we
	// need to allow ten digits overall.
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%.10g", 100.0 + fmod(iTime, 60)).substr(1);
	}

string Util_Time::sAsString_ISO8601_s(double iTime, bool iIncludeT)
	{
	if (iIncludeT)
		return Util_Time::sAsStringUTC(iTime, "%Y-%m-%dT%H:%M:%S");
	else
		return Util_Time::sAsStringUTC(iTime, "%Y-%m-%d %H:%M:%S");
	}

string Util_Time::sAsString_ISO8601_ms(double iTime, bool iIncludeT)
	{
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%06.3f", fmod(iTime, 60));
	}

string Util_Time::sAsString_ISO8601_us(double iTime, bool iIncludeT)
	{
	const string YmdHM = spYmdHM(iTime, iIncludeT);
	return YmdHM + sStringf("%09.6f", fmod(iTime, 60));
	}

 double Util_Time::sFromString_ISO8601(const string& iString)
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
	return 0;
	}

} // namespace ZooLib
