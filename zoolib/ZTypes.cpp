/* ------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "ZTypes.h"

/** \defgroup group_Types Types
Although the standard library defines a range of integer types in cstdint,
older versions of the library may not define the complete range, or 
the types may conflict with types used in platform specific headers. Also some
compilers differ in their representation of 64 bit integers. To harmonize
these disparate environments ZTypes.h defines a useful standard set of
integer types of known size and signedness that are tuned to be maximally
compatible with each target platform.
*/

/** \typedef int8
\ingroup group_Types
int8 is a signed 8 bit integer.
*/

/** \typedef int16
\ingroup group_Types
int16 is a signed 16 bit integer.
*/

/** \typedef int32
\ingroup group_Types
int32 is a signed 32 bit integer.
*/

/** \typedef int64
\ingroup group_Types
int64 is a signed 64 bit integer.
*/

/** \typedef uint8
\ingroup group_Types
uint8 is an unsigned 8 bit integer.
*/

/** \typedef uint16
\ingroup group_Types
uint16 is an unsigned 16 bit integer.
*/

/** \typedef uint32
\ingroup group_Types
uint32 is an unsigned 32 bit integer.
*/

/** \typedef uint64
\ingroup group_Types
uint64 is an unsigned 64 bit integer.
*/

/** \typedef bigtime_t
\ingroup group_Types
bigtime_t is a type from BeOS that's used to represent
microsecond-resolution times on all platforms.
*/

/** \typedef ZPointPOD
\ingroup group_Types
ZPointPOD is used in circumstances where a ZPoint would present
difficulties because of its ctor/dtor (in unions, mainly.).
*/

/** \typedef ZRectPOD
\ingroup group_Types
ZRectPOD is used in circumstances where a ZRect would present
difficulties because of its ctor/dtor (in unions, mainly.).
*/

/** \typedef ZType
\ingroup group_Types
ZType is a numeric representation of a range of basic types,
used most commonly in ZTuple and ZTupleValue.
*/

/**
Provides standard textual versions of ZType values.
*/
static const char* const sZTypeAsString[eZType_Max] =
	{
	"Null",
	"String",
	"!!NOTVALID!!", // Needed to maintain correct offsets.
	"Int8",
	"Int16",
	"Int32",
	"Int64",
	"Float",
	"Double",
	"Bool",
	"Pointer",
	"Raw",
	"Tuple",
	"RefCounted",
	"Rect",
	"Point",
	"Region",
	"ID",
	"Vector",
	"Type",
	"Time",
//##	"Name"
	};

const char* ZTypeAsString(ZType iType)
	{
	if (iType <= eZType_Time)
		return sZTypeAsString[iType];
	return "Unknown";
	}

char ZooLib::sGarbageBuffer[4096];
