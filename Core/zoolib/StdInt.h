// Copyright (c) 2008-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_StdInt_h__
#define __ZooLib_StdInt_h__ 1

#include "zconfig.h"

#include "zoolib/size_t.h" // Brings ::size_t and/or std::size_t into namespace ZooLib

// Macros for 64 bit integer constants
#if ZCONFIG(Compiler, MSVC)
	#define ZINT64_C(v) (v##i64)
	#define ZUINT64_C(v) (v##ui64)
#else
	#define ZINT64_C(v) (v##LL)
	#define ZUINT64_C(v) (v##ULL)
#endif

// =================================================================================================
#pragma mark - Standard integers
// Get the names int8, uint8, int16, uint16, int32, uint32,
// int64 and uint64 into namespace ZStdInt. if you provide
// a project-specific header, it must achieve the same result.

#if defined(ZMACRO_ProjectHeader_StdInt)

	// zconfig.h has defined the name of a project-specific header
	// providing definitions of our standard integer types.

	#include ZMACRO_ProjectHeader_StdInt

#else // defined(ZMACRO_ProjectHeader_StdInt)

	#include <stdint.h>

	namespace ZStdInt
		{
		typedef int8_t int8;
		typedef uint8_t uint8;

		typedef int16_t int16;
		typedef uint16_t uint16;

		typedef int32_t int32;
		typedef uint32_t uint32;

		typedef int64_t int64;
		typedef uint64_t uint64;
		} // namespace ZStdInt

#endif // defined(ZMACRO_ProjectHeader_StdInt)

// =================================================================================================

namespace ZooLib {

typedef unsigned char byte;

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

template <int size> struct ZIntTrait_T;

template <> struct ZIntTrait_T<4> { enum { eIs32Bit = 1, eIs64Bit = 0 }; };
template <> struct ZIntTrait_T<8> { enum { eIs32Bit = 0, eIs64Bit = 1 }; };

enum { ZLongIs32Bit = ZIntTrait_T<sizeof(long)>::eIs32Bit };

enum { ZIntIs32Bit = ZIntTrait_T<sizeof(int)>::eIs32Bit };

enum { ZLongIs64Bit = ZIntTrait_T<sizeof(long)>::eIs64Bit };

enum { ZIntIs64Bit = ZIntTrait_T<sizeof(int)>::eIs64Bit };

} // namespace ZooLib

#endif // __ZooLib_StdInt_h__
