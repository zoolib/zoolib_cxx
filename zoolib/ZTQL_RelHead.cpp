/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZTQL_RelHead.h"

#include "zoolib/ZUtil_STL.h"

using std::set;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::RelHead

RelHead::RelHead()
	{}

RelHead::RelHead(const ZTName& iName)
:	fNames(&iName, &iName + 1)
	{}

RelHead::RelHead(const set<ZTName>& iNames)
:	fNames(iNames)
	{}

RelHead::RelHead(set<ZTName>& ioNames, bool iKnowWhatImDoing)
	{
	ZAssert(iKnowWhatImDoing);
	fNames.swap(ioNames);
	}

RelHead::RelHead(const vector<ZTName>& iNames)
:	fNames(iNames.begin(), iNames.end())
	{}

RelHead::RelHead(const ZTName* iNames, size_t iCount)
:	fNames(iNames, iNames + iCount)
	{}

RelHead::RelHead(const RelHead& iOther)
:	fNames(iOther.fNames)
	{}

RelHead::~RelHead()
	{}

RelHead& RelHead::operator=(const RelHead& iOther)
	{
	fNames = iOther.fNames;
	return *this;
	}

bool RelHead::operator==(const RelHead& iOther) const
	{ return fNames == iOther.fNames; }

bool RelHead::operator!=(const RelHead& iOther) const
	{ return fNames != iOther.fNames; }

RelHead& RelHead::operator&=(const RelHead& iOther)
	{
	set<ZTName> result;

	set_intersection(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

RelHead RelHead::operator&(const RelHead& iOther) const
	{
	set<ZTName> result;

	set_intersection(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return RelHead(result, true);
	}

RelHead& RelHead::operator|=(const RelHead& iOther)
	{
	set<ZTName> result;

	set_union(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

RelHead RelHead::operator|(const RelHead& iOther) const
	{
	set<ZTName> result;

	set_union(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return RelHead(result, true);
	}

RelHead& RelHead::operator|=(const ZTName& iName)
	{
	fNames.insert(iName);
	return *this;
	}

RelHead RelHead::operator|(const ZTName& iName) const
	{
	RelHead result = *this;
	result |= iName;
	return result;
	}

RelHead& RelHead::operator-=(const RelHead& iOther)
	{
	set<ZTName> result;

	set_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

RelHead RelHead::operator-(const RelHead& iOther) const
	{
	set<ZTName> result;

	set_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return RelHead(result, true);
	}

RelHead& RelHead::operator-=(const ZTName& iName)
	{
	ZUtil_STL::sEraseIfContains(fNames, iName);
	return *this;
	}

RelHead RelHead::operator-(const ZTName& iName) const
	{
	RelHead result = *this;
	result -= iName;
	return result;
	}

RelHead& RelHead::operator^=(const RelHead& iOther)
	{
	set<ZTName> result;

	set_symmetric_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

RelHead RelHead::operator^(const RelHead& iOther) const
	{
	set<ZTName> result;

	set_symmetric_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return RelHead(result, true);
	}

bool RelHead::Contains(const RelHead& iOther) const
	{
	return includes(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end());
	}

bool RelHead::Contains(const ZTName& iName) const
	{ return fNames.end() != fNames.find(iName); }

void RelHead::Add(const ZTName& iName)
	{
	fNames.insert(iName);
	}

void RelHead::GetNames(set<ZTName>& oNames) const
	{
	oNames = fNames;
	}

RelHead operator|(const ZTName& iName, const RelHead& iRelHead)
	{
	return iRelHead | iName;
	}

RelHead operator&(const ZTName& iName, const RelHead& iRelHead)
	{
	return iRelHead & iName;
	}

ZTQL::RelHead operator|(const ZTName& iLHS, const ZTName& iRHS)
	{
	return ZTQL::RelHead(iLHS) | iRHS;
	}

} // namespace ZTQL

NAMESPACE_ZOOLIB_END
