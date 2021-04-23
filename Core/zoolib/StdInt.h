// Copyright (c) 2008-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_StdInt_h__
#define __ZooLib_StdInt_h__ 1
#include "zconfig.h"

#include <cstddef> // For std::byte (when desired)
#include <cstdint>

#include "zoolib/size_t.h" // Brings ::size_t and/or std::size_t into namespace ZooLib

#if defined(ZMACRO_ProjectHeader_StdInt)
	#error ZMACRO_ProjectHeader_StdInt is not currently supported
#endif

// Macros for 64 bit integer constants
#if ZCONFIG(Compiler, MSVC)
	#define ZINT64_C(v) (v##i64)
	#define ZUINT64_C(v) (v##ui64)
#else
	#define ZINT64_C(v) (v##LL)
	#define ZUINT64_C(v) (v##ULL)
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark - Standard integers

#if 0 && ZCONFIG_CPP >= 2017
	// We're not ready to do this yet.
	using std::byte;
#else
	typedef unsigned char byte;
#endif

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

template <int size> struct IntTrait_T;

template <> struct IntTrait_T<4> { enum { eIs32Bit = 1, eIs64Bit = 0 }; };
template <> struct IntTrait_T<8> { enum { eIs32Bit = 0, eIs64Bit = 1 }; };

enum { kLongIs32Bit = IntTrait_T<sizeof(long)>::eIs32Bit };

enum { kIntIs32Bit = IntTrait_T<sizeof(int)>::eIs32Bit };

enum { kLongIs64Bit = IntTrait_T<sizeof(long)>::eIs64Bit };

enum { kIntIs64Bit = IntTrait_T<sizeof(int)>::eIs64Bit };

} // namespace ZooLib

#endif // __ZooLib_StdInt_h__
