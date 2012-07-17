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

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCompare.h"
#include "zoolib/ZCompat_cmath.h" // For NAN and isnan
#include "zoolib/ZDebug.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZTypes.h" // For countof

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(POSIX)
	#include <sys/time.h> // For timeval
#endif

#if defined(__MACH__)
	#include <mach/mach_time.h>
#endif

#if ZCONFIG_SPI_Enabled(MacClassic)
	#include <DriverServices.h> // For UpTime, Nanoseconds etc
#endif

#if ZCONFIG_SPI_Enabled(Carbon64)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,DriverServices.h) // For UpTime, Nanoseconds etc
#endif

#if ZCONFIG_SPI_Enabled(Win)
	#include "zoolib/ZCompat_Win.h"
	#include "zoolib/ZUtil_Win.h"
#endif

#if ZCONFIG_SPI_Enabled(Linux)
	#include <sys/sysinfo.h> // For sysinfo
	#include <cstdio> // For FILE and fopen
#endif

#if ZCONFIG_SPI_Enabled(BSD)
	#include <sys/types.h> // For sysctl etc.
	#include <sys/sysctl.h>
#endif

namespace ZooLib {

ZMACRO_CompareRegistration_T(ZTime)

// =================================================================================================
// MARK: - ZTime constants

const double ZTime::kSecond = 1;
const double ZTime::kMinute = 60 * kSecond;
const double ZTime::kHour = 60 * kMinute;
const double ZTime::kDay = 24 * kHour;
const double ZTime::kWeek = 7 * kDay;
const double ZTime::kYear = 365 * kDay;

const double ZTime::kEpochDelta_1900_To_1904 = 4 * kYear; // 126,144,000
const double ZTime::kEpochDelta_1904_To_1970 = 66 * kYear + 17 * kDay; // 2,082,844,800
const double ZTime::kEpochDelta_1900_To_1970 = 70 * kYear + 17 * kDay; // 2,208,988,800
const double ZTime::kEpochDelta_1601_To_1970 = 369 * kYear + 89 * kDay; // 11,644,473,600

const double ZTime::kEpochDelta_1970_To_2001 = -978307200.0;
const double ZTime::kEpochDelta_2001_To_1970 = 978307200.0;

// =================================================================================================
// MARK: - ZTime

ZTime::ZTime()
:	fVal(NAN)
	{}

ZTime::operator operator_bool() const
	{ return operator_bool_gen::translate(!isnan(fVal)); }

bool ZTime::operator<(const ZTime& iOther) const
	{ return sCompare_T(fVal, iOther.fVal) < 0; }

bool ZTime::operator==(const ZTime& iOther) const
	{ return sCompare_T(fVal, iOther.fVal) == 0; }

int ZTime::Compare(const ZTime& iOther) const
	{ return sCompare_T(fVal, iOther.fVal); }

ZTime ZTime::sNow()
	{
#if 0
#elif ZCONFIG_SPI_Enabled(POSIX)

	timeval theTimeVal;
	::gettimeofday(&theTimeVal, nullptr);
	return theTimeVal.tv_sec + double(theTimeVal.tv_usec) / 1e6;

#elif ZCONFIG_SPI_Enabled(Carbon64)

	UTCDateTime theUTCDateTime;
	::GetUTCDateTime(&theUTCDateTime, kUTCDefaultOptions);

	double result = double(theUTCDateTime.highSeconds) + double(theUTCDateTime.lowSeconds);
	result -= kEpochDelta_1904_To_1970;
	result += double(theUTCDateTime.fraction) / 65536.0;
	return result;

#elif ZCONFIG_SPI_Enabled(MacClassic)

	// This has a resolution of one second, not a lot of use really.
	unsigned long theDateTime;
	::GetDateTime(&theDateTime);
	return theDateTime - kEpochDelta_1904_To_1970;

#elif ZCONFIG_SPI_Enabled(Win)

	// FILETIME is a 64 bit count of the number of 100-nanosecond (1e-7)
	// periods since January 1, 1601 (UTC).
	FILETIME ft;
	::GetSystemTimeAsFileTime(&ft);

	// 100 nanoseconds is one ten millionth of a second, so divide the
	// count of 100 nanoseconds by ten million to get the count of seconds.
	double result = (*reinterpret_cast<int64*>(&ft)) / 1e7;

	// Subtract the number of seconds between 1601 and 1970.
	result -= kEpochDelta_1601_To_1970;
	return result;

#else

	#error Unsupported platform

#endif
	}

ZTime ZTime::sSystem()
	{
#if 0
#elif defined(__MACH__)
	static bool sInited;
	static double sRatio;
	if (not sInited)
		{
		mach_timebase_info_data_t theTBI;
		::mach_timebase_info(&theTBI);
		sRatio = double(theTBI.numer) / double(theTBI.denom) / 1e9;
		sInited = true;
		sAtomic_Barrier();
		}

	return double(::mach_absolute_time()) * sRatio;

#elif ZCONFIG_SPI_Enabled(POSIX)

	/* AG 2003-10-26.
	I've spent all day trying to find a monotonic clock for unix. Mozilla's
	PR_GetInterval has sensible implementations for classic MacOS, Irix and
	Solaris. But the unix implementation just uses gettimeofday.
	The POSIX 'clock_gettime' using CLOCK_MONOTONIC looked promising, even
	being supported on Linux (in librt). However on Linux it simply tracks
	gettimeofday and is thus not monotonic, at least across process invocations
	N.B. I didn't try changing the clock whilst a process was running.

	For the moment we're guaranteeing a monotonic clock by tracking the last
	value returned by gettimeofday and if it goes backwards we accumulate a
	delta that is applied to every result.
	*/

	static volatile double sLast = 0;
	static volatile double sDelta = 0;

	struct timeval theTimeVal;
	::gettimeofday(&theTimeVal, nullptr);
	double result = theTimeVal.tv_sec + double(theTimeVal.tv_usec) / 1e6;

	// I know that this is not threadsafe. However, we'd need to have two
	// threads at the right (wrong) place at the same time when the clock
	// has just changed, leading to sDelta getting incremented twice. But
	// we'll still see a monotonically increasing clock.
	if (sLast > result)
		sDelta += sLast - result;

	sLast = result;
	return result + sDelta;

#elif ZCONFIG_SPI_Enabled(Carbon64) || ZCONFIG_SPI_Enabled(MacClassic)

	Nanoseconds theNanoseconds = AbsoluteToNanoseconds(UpTime());
	return double(*reinterpret_cast<uint64*>(&theNanoseconds)) / 1e9;

#elif ZCONFIG_SPI_Enabled(Win)

	if (ZUtil_Win::sIsWinNT())
		{
		// On NT we try to use the performance counter of CPU 0. Some types of CPU
		// do not have performance counters that run in sync, so using the
		// counter from whatever CPU on which this code happens to be executed defeats
		// the whole point of sSystem.
		DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 1);

		uint64 frequency;
		::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
		if (frequency)
			{
			uint64 time;
			::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));

			::SetThreadAffinityMask(::GetCurrentThread(), oldmask);

			return double(time) / double(frequency);
			}

		::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
		}

	// Use GetTickCount if we're not on NT or QPC is not available.
	return double(::GetTickCount()) / 1e3;

#elif ZCONFIG_SPI_Enabled(BeOS)

	return double(::system_time()) / 1e6;

#else

	#error Unsupported platform

#endif
	}

ZTime ZTime::sBoot()
	{
#if ZCONFIG_SPI_Enabled(BSD)

	int theMIB[] = { CTL_KERN, KERN_BOOTTIME };
	struct timeval theTimeVal;
	size_t theLen = sizeof(theTimeVal);
	if (0 == ::sysctl(theMIB, countof(theMIB), &theTimeVal, &theLen, nullptr, 0))
		{
		if (theLen == sizeof(theTimeVal))
			return theTimeVal.tv_sec + double(theTimeVal.tv_usec) / 1e6;
		}
	return ZTime();

#else

	return sNow() - sSinceBoot();

#endif
	}

double ZTime::sSinceBoot()
	{
#if ZCONFIG_SPI_Enabled(Linux)

	if (FILE* theFILE = ::fopen("/proc/uptime", "r"))
		{
		double theUptime;
		int result = ::fscanf(theFILE, "%lf", &theUptime);
		::fclose(theFILE);
		if (result > 0)
			return theUptime;
		}

	#if defined(__ANDROID__)
		ZUnimplemented();
	#else
		struct sysinfo theSI;
		if (0 == ::sysinfo(&theSI))
			return theSI.uptime;
		return -1;
	#endif

#elif ZCONFIG_SPI_Enabled(BSD)

	return sNow() - sBoot();

#else

	// Assume that sSystem is returning elapsed time since boot.
	return sSystem().fVal;

#endif
	}

} // namespace ZooLib
