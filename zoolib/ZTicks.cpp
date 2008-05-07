/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZTicks.h"

// =================================================================================================
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) // || ZCONFIG(OS, MacOSX)
#	if ZCONFIG(OS, MacOSX)
#		include <CarbonCore/DriverServices.h>
#		include <HIToolbox/Events.h>
#	else
#		include <DriverServices.h>
#		include <Events.h> // For GetDblTime & GetCaretTime
#	endif

bigtime_t ZTicks::sDoubleClick()
	{ return bigtime_t(::GetDblTime()) * 16667LL; }

bigtime_t ZTicks::sCaretFlash()
	{ return bigtime_t(::GetCaretTime()) * 16667LL; }

bigtime_t ZTicks::sNow()
	{
	Nanoseconds theTicks = AbsoluteToNanoseconds(UpTime());
	return *reinterpret_cast<uint64*>(&theTicks) / 1000;
	}

#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

// =================================================================================================
#if ZCONFIG(OS, Win32)
#include "ZWinHeader.h"

// Win32 sNow() cannot be inlined due to bug in CW compiler. -ec 01.01.03
bigtime_t ZTicks::sNow()
	{
	uint64 frequency;
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&frequency));
	if (!frequency)
		{
		// A zero frequency indicates there's no performance counter support,
		// so we fall back to GetTickCount.
		return bigtime_t(::GetTickCount()) * 1000;
		}

	uint64 time;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
	return bigtime_t(double(time) / double(frequency) * 1e6);
	}


bigtime_t ZTicks::sDoubleClick()
	{ return bigtime_t(::GetDoubleClickTime()) * 1000; }

#endif // ZCONFIG(OS, Win32)
