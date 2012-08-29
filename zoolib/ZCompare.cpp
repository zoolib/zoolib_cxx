/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompat_String.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZSingleton.h"
#include "zoolib/ZUtil_STL_map.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Functor_CompareCharStars (anonymous)

namespace { // anonymous

class Functor_CompareCharStars
	{
public:
	bool operator()(const char* iL, const char* iR) const
		{ return strcmp(iL, iR) < 0; }
	};

typedef std::map<const char*, ZCompare*, Functor_CompareCharStars> CompareMap;

} // anonymous namespace

// =================================================================================================
// MARK: - ZCompare

ZCompare::ZCompare(const char* iTypeName)
:	fTypeName(iTypeName)
	{ ZUtil_STL::sInsertMust(sSingleton<CompareMap>(), iTypeName, this); }

ZCompare::~ZCompare()
	{}

int ZCompare::Compare(const void* iL, const void* iR)
	{
	ZDebugStopf(0, ("ZCompare::Compare not overridden type '%s'", fTypeName));
	return 0;
	}

int ZCompare::sCompare(const char* iTypeName, const void* iL, const void* iR)
	{
	if (ZQ<ZCompare*> theQ = ZUtil_STL::sQGet(sSingleton<CompareMap>(), iTypeName))
		return (*theQ)->Compare(iL, iR);

	ZDebugStopf(0, ("ZCompare::sCompare called on unsupported type '%s'", iTypeName));
	return iL < iR ? -1 : iL > iR ? 1 : 0;
	}

// =================================================================================================
// MARK: - ZCompareRegistration_Void (anonymous)

namespace { // anonymous

class ZCompareRegistration_Void : public ZCompare
	{
public:
	ZCompareRegistration_Void() : ZCompare(typeid(void).name()) {}
	virtual int Compare(const void* iL, const void* iR)
		{ return 0; }
	} ZMACRO_Concat(sComparer_,__LINE__);

} // anonymous namespace

} // namespace ZooLib
