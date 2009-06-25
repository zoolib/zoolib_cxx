/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZValAccessors_Std__
#define __ZValAccessors_Std__ 1
#include "zconfig.h"

#include "zoolib/ZTypes.h"
#include "zoolib/ZValAccessors.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

typedef void* VoidStar_t;

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * ZMACRO_ZValAccessors_Std

typedef void* VoidStar_t;

#define ZMACRO_ZValAccessors_Decl_Std(T) \
	ZMACRO_ZValAccessors_Decl_Entry(Int8, int8) \
	ZMACRO_ZValAccessors_Decl_Entry(Int16, int16) \
	ZMACRO_ZValAccessors_Decl_Entry(Int32, int32) \
	ZMACRO_ZValAccessors_Decl_Entry(Int64, int64) \
	ZMACRO_ZValAccessors_Decl_Entry(Bool, bool) \
	ZMACRO_ZValAccessors_Decl_Entry(Float, float) \
	ZMACRO_ZValAccessors_Decl_Entry(Double, double) \
	ZMACRO_ZValAccessors_Decl_Entry(String, std::string) \

#define ZMACRO_ZValAccessors_Def_Std(T) \
	ZMACRO_ZValAccessors_Def_Entry(T, Int8, int8) \
	ZMACRO_ZValAccessors_Def_Entry(T, Int16, int16) \
	ZMACRO_ZValAccessors_Def_Entry(T, Int32, int32) \
	ZMACRO_ZValAccessors_Def_Entry(T, Int64, int64) \
	ZMACRO_ZValAccessors_Def_Entry(T, Bool, bool) \
	ZMACRO_ZValAccessors_Def_Entry(T, Float, float) \
	ZMACRO_ZValAccessors_Def_Entry(T, Double, double) \
	ZMACRO_ZValAccessors_Def_Entry(T, String, std::string) \


#endif // __ZValAccessors_Std__
