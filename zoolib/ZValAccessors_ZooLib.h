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

#ifndef __ZValAccessors_ZooLib__
#define __ZValAccessors_ZooLib__ 1
#include "zconfig.h"

#include "zoolib/ZValAccessors.h"
#include "zoolib/ZRef_Counted.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

class ZValMap_ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark * ZValAccessors_ZooLib

#define ZMACRO_ZValAccessors_Decl_ZooLib(T) \
	ZMACRO_ZValAccessors_Decl_Entry(T, ID, uint64) \
	ZMACRO_ZValAccessors_Decl_Entry(T, Type, ZType) \
	ZMACRO_ZValAccessors_Decl_Entry(T, Time, ZTime) \
	ZMACRO_ZValAccessors_Decl_Entry(T, Rect, ZRectPOD) \
	ZMACRO_ZValAccessors_Decl_Entry(T, Point, ZPointPOD) \
	ZMACRO_ZValAccessors_Decl_Entry(T, RefCounted, ZRef<ZRefCountedWithFinalize>) \
	ZMACRO_ZValAccessors_Decl_Entry(T, Pointer, VoidStar_t) \

#define ZMACRO_ZValAccessors_Def_ZooLib(T) \
	ZMACRO_ZValAccessors_Def_Entry(T, ID, uint64) \
	ZMACRO_ZValAccessors_Def_Entry(T, Type, ZType) \
	ZMACRO_ZValAccessors_Def_Entry(T, Time, ZTime) \
	ZMACRO_ZValAccessors_Def_Entry(T, Rect, ZRectPOD) \
	ZMACRO_ZValAccessors_Def_Entry(T, Point, ZPointPOD) \
	ZMACRO_ZValAccessors_Def_Entry(T, RefCounted, ZRef<ZRefCountedWithFinalize>) \
	ZMACRO_ZValAccessors_Def_Entry(T, Pointer, VoidStar_t) \

NAMESPACE_ZOOLIB_END

#endif // __ZValAccessors_ZooLib__
