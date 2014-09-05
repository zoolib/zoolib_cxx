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

#ifndef __ZStdInt_h__
#define __ZStdInt_h__ 1
#include "zconfig.h"

#include <cstddef> // For std::size_t

// Macros for 64 bit integer constants
#if ZCONFIG(Compiler, MSVC)
	#define ZINT64_C(v) (v##i64)
	#define ZUINT64_C(v) (v##ui64)
#else
	#define ZINT64_C(v) (v##LL)
	#define ZUINT64_C(v) (v##ULL)
#endif

// =================================================================================================
// MARK: - Standard integers
// Get the names int8, uint8, int16, uint16, int32, uint32,
// int64 and uint64 into namespace ZStdInt. if you provide
// a project-specific header, it must achieve the same result.

#if defined(ZProjectHeader_StdInt)

	// zconfig.h has defined the name of a project-specific header
	// providing definitions of our standard integer types.

	#include ZProjectHeader_StdInt

#else // defined(ZProjectHeader_StdInt)

	#if ZCONFIG(Compiler, MSVC) and not (defined(ZMACRO_StdIntAvailable) and ZMACRO_StdIntAvailable)

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

	#include "zoolib/ZCONFIG_SPI.h"

	// Pick up Mac's UInt32 if it's available
	#if ZCONFIG_SPI_Enabled(MacOSX) \
		|| ZCONFIG_SPI_Enabled(MacClassic) \
		|| ZCONFIG_SPI_Enabled(Carbon64)

		#if ZCONFIG(Compiler,Clang)
			#include <MacTypes.h>
		#else
			#include ZMACINCLUDE3(CoreServices,CarbonCore,MacTypes.h)
		#endif
	#endif

	namespace ZStdInt
		{
		typedef int8_t int8;
		typedef uint8_t uint8;

		typedef int16_t int16;
		typedef uint16_t uint16;

		typedef int32_t int32;

		// Doing this makes a lot of stuff more directly compatible on Mac.
		#if ZCONFIG_SPI_Enabled(MacOSX) \
			|| ZCONFIG_SPI_Enabled(MacClassic) \
			|| ZCONFIG_SPI_Enabled(Carbon64)
			typedef UInt32 uint32;
		#else
			typedef uint32_t uint32;
		#endif

		typedef int64_t int64;
		typedef uint64_t uint64;
		} // namespace ZStdInt

#endif // defined(ZStdInt_ProjectHeader)

// =================================================================================================

namespace ZooLib {

typedef signed char byte;

// Pull the ZStdInt names into namespace ZooLib. If you'd like to use
// these names unadorned, you can do 'using namespace ZooLib' or
// 'using namespace ZStdInt', or indeed use the same explicit injection.
using ZStdInt::int8;
using ZStdInt::uint8;
using ZStdInt::int16;
using ZStdInt::uint16;
using ZStdInt::int32;
using ZStdInt::uint32;
using ZStdInt::int64;
using ZStdInt::uint64;

using std::size_t;

template <int size> struct ZIntTrait_T;

template <> struct ZIntTrait_T<4> { enum { eIs32Bit = 1, eIs64Bit = 0 }; };
template <> struct ZIntTrait_T<8> { enum { eIs32Bit = 0, eIs64Bit = 1 }; };

enum { ZLongIs32Bit = ZIntTrait_T<sizeof(long)>::eIs32Bit };

enum { ZIntIs32Bit = ZIntTrait_T<sizeof(int)>::eIs32Bit };

enum { ZLongIs64Bit = ZIntTrait_T<sizeof(long)>::eIs64Bit };

enum { ZIntIs64Bit = ZIntTrait_T<sizeof(int)>::eIs64Bit };

} // namespace ZooLib

#endif // __ZStdInt_h__
