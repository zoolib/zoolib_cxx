/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZStdInt__
#define __ZStdInt__
#include "zconfig.h"

// ==================================================
// The usual suite of types with known sizes and signedness.

#if defined(ZStdInt_ProjectHeader)

// zconfig.h has defined the name of a project-specific header
// providing definitions of our standard integer types.

#	include ZStdInt_ProjectHeader

#else

#	include "zoolib/ZCONFIG_SPI.h"

#	if ZCONFIG(Compiler, MSVC)

		typedef signed char int8;
		typedef unsigned char uint8;

		typedef signed short int16;
		typedef unsigned short uint16;

		typedef signed int int32;
		typedef unsigned int uint32;

		typedef __int64 int64;
		typedef unsigned __int64 uint64;

		typedef int64 bigtime_t;

#	elif ZCONFIG_SPI_Enabled(MacClassic) || ZCONFIG_SPI_Enabled(Carbon) && !ZCONFIG_SPI_Enabled(POSIX)

		#if ZCONFIG_SPI_Enabled(MacClassic)
			#include <MacTypes.h>
		#else
			#include <CoreServices/CoreServices.h>
		#endif

		typedef SInt8 int8;
		typedef UInt8 uint8;

		typedef SInt16 int16;
		typedef UInt16 uint16;

		typedef SInt32 int32;
		typedef UInt32 uint32;

		typedef SInt64 int64;
		typedef UInt64 uint64;

		typedef int64 bigtime_t;
		
#	else

		#include <stdint.h>

		typedef int8_t int8;
		typedef uint8_t uint8;

		typedef int16_t int16;
		typedef uint16_t uint16;

		typedef int32_t int32;
		typedef uint32_t uint32;

		typedef int64_t int64;
		typedef uint64_t uint64;

		typedef int64 bigtime_t;

	#endif

#endif // defined(ZStdInt_ProjectHeader)

#endif // __ZStdInt__
