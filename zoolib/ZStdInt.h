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

// Macros for explicit 64 bit integers

#if ZCONFIG(Compiler, MSVC)
	#define ZINT64_C(v) (v##i64)
	#define ZUINT64_C(v) (v##ui64)
#else
	#define ZINT64_C(v) (v##LL)
	#define ZUINT64_C(v) (v##ULL)
#endif

// The usual suite of types with known sizes and signedness.

#if defined(ZProjectHeader_StdInt)

	// zconfig.h has defined the name of a project-specific header
	// providing definitions of our standard integer types.

	#include ZProjectHeader_StdInt

#else // defined(ZProjectHeader_StdInt)

	#include "zoolib/ZCONFIG_SPI.h"

	#if ZCONFIG_SPI_Enabled(MacOSX) \
		|| ZCONFIG_SPI_Enabled(MacClassic) \
		|| ZCONFIG_SPI_Enabled(Carbon)

		#if __arm__
			#include <MacTypes.h>
		#else
			#include ZMACINCLUDE3(CoreServices,CarbonCore,MacTypes.h)
		#endif

		// Doing this makes a lot of stuff more directly compatible on Mac.
		#ifndef _UINT32
			#define _UINT32 1
			typedef UInt32 uint32;
		#endif

	#endif

	#if ZCONFIG(Compiler, MSVC)

		#ifndef _INT8_T
			#define _INT8_T
			typedef	__int8 int8_t;
		#endif

		#ifndef _INT16_T
			#define _INT16_T
			typedef	__int16 int16_t;
		#endif

		#ifndef _INT32_T
			#define _INT32_T
			typedef	__int32 int32_t;
		#endif

		#ifndef _INT64_T
			#define _INT64_T
			typedef	__int64 int64_t;
		#endif

		#ifndef _UINT8_T_DECLARED
			#define _UINT8_T_DECLARED
			typedef unsigned __int8 uint8_t;
		#endif

		#ifndef _UINT16_T_DECLARED
			#define _UINT16_T_DECLARED
			typedef unsigned __int16 uint16_t;
		#endif

		#ifndef _UINT32_T_DECLARED
			#define _UINT32_T_DECLARED
			typedef unsigned __int32 uint32_t;
		#endif

		#ifndef _UINT64_T_DECLARED
			#define _UINT64_T_DECLARED
			typedef unsigned __int64 uint64_t;
		#endif

	#else

		#include <stdint.h>

	#endif

	typedef int8_t int8;
	typedef uint8_t uint8;

	typedef int16_t int16;
	typedef uint16_t uint16;

	#ifndef _INT32
		#ifndef _MSL_USING_MW_C_HEADERS
			#define _INT32 1
		#endif
		typedef int32_t int32;
	#endif

	#ifndef _UINT32
		#define _UINT32 1
		typedef uint32_t uint32;
	#endif

	typedef int64_t int64;
	typedef uint64_t uint64;

	typedef int64 bigtime_t;

#endif // defined(ZStdInt_ProjectHeader)

NAMESPACE_ZOOLIB_BEGIN

template <int size> class ZIntTrait_T;

template <> struct ZIntTrait_T<4> { enum { eIs32Bit = 1 }; };
template <> struct ZIntTrait_T<8> { enum { eIs32Bit = 0 }; };

enum { ZLongIs32Bit = ZIntTrait_T<sizeof(long)>::eIs32Bit };

enum { ZIntIs32Bit = ZIntTrait_T<sizeof(int)>::eIs32Bit };

NAMESPACE_ZOOLIB_END

#endif // __ZStdInt__
