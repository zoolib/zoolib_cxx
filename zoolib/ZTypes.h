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

#ifndef __ZTypes__
#define __ZTypes__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZStdInt.h"

// Pull in definition of size_t, which is heavily used by zoolib.
#include <stddef.h>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================

namespace ZUnicode {
// This template and its two specializations below let us
// determine cleanly whether wchar_t is 16 or 32 bits.
template <int s> struct Types_T {};

template <> struct Types_T<4>
	{
	typedef wchar_t utf32_t;
	typedef uint16 utf16_t;
	};

template <> struct Types_T<2>
	{
	typedef uint32 utf32_t;
	typedef wchar_t utf16_t;
	};

} // namespace ZUnicode

// Definitions of UTF32, UTF16 and UTF8
typedef ZUnicode::Types_T<sizeof(wchar_t)>::utf32_t UTF32;
typedef ZUnicode::Types_T<sizeof(wchar_t)>::utf16_t UTF16;
typedef char UTF8;

// =================================================================================================

// Use the ZFourCC inline if possible.
inline uint32 ZFourCC(uint8 a, uint8 b, uint8 c, uint8 d)
	{ return uint32((a << 24) | (b << 16) | (c << 8) | d); }

// And the macro if a compile-time constant is needed (case statements).
#define ZFOURCC(a,b,c,d) \
	((uint32)((((uint8)a) << 24) | (((uint8)b) << 16) | (((uint8)c) << 8) | (((uint8)d))))

// =================================================================================================

struct ZPointPOD
	{
	int32 h;
	int32 v;
	};

template <> int sCompare_T(const ZPointPOD& iL, const ZPointPOD& iR);

struct ZRectPOD
	{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
	};

template <> int sCompare_T(const ZRectPOD& iL, const ZRectPOD& iR);

// =================================================================================================

enum ZType
	{
	eZType_Null = 0,
	eZType_String = 1,
//	eZType_CString = 2,
	eZType_Int8 = 3,
	eZType_Int16 = 4,
	eZType_Int32 = 5,
	eZType_Int64 = 6,
	eZType_Float = 7,
	eZType_Double = 8,
	eZType_Bool = 9,
	eZType_Pointer = 10,
	eZType_Raw = 11,
	eZType_Tuple = 12,
	eZType_RefCounted = 13,
	eZType_Rect = 14,
	eZType_Point = 15,
	eZType_Region = 16,
	eZType_ID = 17,
	eZType_Vector = 18,
	eZType_Type = 19,
	eZType_Time = 20,
//##	eZType_Name = 21
	eZType_Max
	};

template <> inline int sCompare_T(const ZType& iL, const ZType& iR)
	{ return int(iL) - int(iR); }

const char* ZTypeAsString(ZType iType);

// =================================================================================================

// There are several places where we need a buffer for some other code
// to dump data into, the content of which we don't care about. Rather
// than having multiple static buffers, or requiring wasteful use of
// stack space (a problem on MacOS 9) we instead have a shared garbage
// buffer. The key thing to remember in using it is that it must never
// be read from -- there's no way to know what other code is also using it.

extern char sGarbageBuffer[4096];

// In many places we need a stack-based buffer. Ideally they'd be quite large
// but on MacOS 9 we don't want to blow the 24K - 32K that's normally available.
// This constant is 4096 on most platforms, 256 on MacOS 9.

#if ZCONFIG_SPI_Enabled(MacClassic)
	static const size_t sStackBufferSize = 256;
#else
	static const size_t sStackBufferSize = 4096;
#endif

// =================================================================================================
// For a discussion of the implementation of countof See section 14.3 of
// "Imperfect C++" by Matthew Wilson, published by Addison Wesley.

template<class T, int N>
uint8 (&byte_array_of_same_dimension_as(T(&)[N]))[N];

#define countof(x) sizeof(ZOOLIB_PREFIX::byte_array_of_same_dimension_as((x)))

// =================================================================================================

template <class T>
class Adopt_T
	{
	T* fP;
public:
	Adopt_T(T* iP) : fP(iP) {}
	T* Get() const { return fP; }
	};

template <class T>
class Adopt_T<T*>
	{
	T* fP;
public:
	Adopt_T(T* iP) : fP(iP) {}
	T* Get() const { return fP; }
	};

template <class P>
Adopt_T<P> Adopt(P iP)
	{ return Adopt_T<P>(iP); }

// =================================================================================================

typedef void* VoidStar_t;

template <> inline int sCompare_T(const VoidStar_t& iL, const VoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

typedef const void* ConstVoidStar_t;

template <> inline int sCompare_T(const ConstVoidStar_t& iL, const ConstVoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

// =================================================================================================

NAMESPACE_ZOOLIB_END

#endif // __ZTypes__
