/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTicks__
#define __ZTicks__ 1
#include "zconfig.h"

#include "ZTypes.h"

#if ZCONFIG(OS, POSIX)  || ZCONFIG(OS, MacOSX)
#	include <sys/time.h> // For timeval
#endif

#if ZCONFIG(OS, Be)
#	include <interface/InterfaceDefs.h> // for get_click_speed
#	include <kernel/OS.h> // for system_time
#endif

// =================================================================================================

namespace ZTicks {

#if ZCONFIG(OS, Win32) || ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) // || ZCONFIG(OS, MacOSX)
bigtime_t sNow();

#elif ZCONFIG(OS, POSIX) || ZCONFIG(OS, MacOSX)
inline bigtime_t sNow()
	{
	timeval theTimeVal;
	::gettimeofday(&theTimeVal, nil);
	return (bigtime_t(theTimeVal.tv_sec) * 1000000LL) + bigtime_t(theTimeVal.tv_usec);
	}

#elif ZCONFIG(OS, Be)
inline bigtime_t sNow()
	{ return ::system_time(); }

#endif // ZCONFIG_OS


inline bigtime_t sPerSecond()
	{ return 1000000; }


#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)// || ZCONFIG(OS, MacOSX)
bigtime_t sDoubleClick();

#elif ZCONFIG(OS, Win32)
bigtime_t sDoubleClick();

#elif ZCONFIG(OS, POSIX) || ZCONFIG(OS, MacOSX)
inline bigtime_t sDoubleClick()
	{ return sPerSecond() / 10; }

#elif ZCONFIG(OS, Be)
inline bigtime_t sDoubleClick()
	{
	bigtime_t result;
	::get_click_speed(&result);
	return result;
	}
#endif

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)
bigtime_t sCaretFlash();

#else
inline bigtime_t sCaretFlash()
	{ return sPerSecond() / 5; }

#endif

} // namespace ZTicks

// =================================================================================================

#endif // __ZTicks__
