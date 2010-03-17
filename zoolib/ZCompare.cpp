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
#include "zoolib/ZDebug.h"

#include <map>

using std::map;
using std::pair;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * CompareCharStars

namespace ZANONYMOUS {

class CompareCharStars
	{
public:
	bool operator()(const char* iL, const char* iR) const
		{ return std::strcmp(iL, iR) < 0; }
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZCompare

static int sInitCount;
static map<const char*, ZCompare*, CompareCharStars>* sMap;

ZCompare::ZCompare(const char* iTypeName)
	{
	if (++sInitCount == 1)
		{
		ZAssert(!sMap);
		sMap = new map<const char*, ZCompare*, CompareCharStars>;
		}
	sMap->insert(pair<const char*, ZCompare*>(iTypeName, this));
	}

ZCompare::~ZCompare()
	{
	if (--sInitCount == 0)
		{
		delete sMap;
		sMap = nullptr;
		}
	}

int ZCompare::sCompare(const char* iTypeName, const void* iL, const void* iR)
	{
	ZAssert(sMap);

	map<const char*, ZCompare*, CompareCharStars>::iterator i = sMap->find(iTypeName);
	if (i != sMap->end())
		return (*i).second->Compare(iL, iR);

	ZDebugStopf(0, ("ZCompare::sCompare called on unsupported type '%s'", iTypeName));
	return iL < iR ? -1 : iL > iR ? 1 : 0;
	}

NAMESPACE_ZOOLIB_END
