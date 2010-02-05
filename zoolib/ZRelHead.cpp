/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZDebug.h"
#include "zoolib/ZRelHead.h"

using std::set;
using std::string;
using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRelHead

ZRelHead::ZRelHead()
:	fUniversal(false)
	{}

ZRelHead::ZRelHead(const ZRelHead& iOther)
:	fUniversal(iOther.fUniversal)
,	fNames(iOther.fNames)
	{}

ZRelHead::~ZRelHead()
	{}

ZRelHead& ZRelHead::operator=(const ZRelHead& iOther)
	{
	fUniversal = iOther.fUniversal;
	fNames = iOther.fNames;
	return *this;
	}

ZRelHead::ZRelHead(bool iUniversal)
:	fUniversal(iUniversal)
	{
	// ZAssert(!fUniversal);
	// Universal not done yet.
	}

ZRelHead::ZRelHead(const char* iName)
:	fUniversal(false)
	{
	fNames.insert(iName);
	}

ZRelHead::ZRelHead(const string& iName)
:	fUniversal(false)
,	fNames(&iName, &iName + 1)
	{}

ZRelHead::ZRelHead(const string& iName1, const string& iName2)
:	fUniversal(false)
	{
	fNames.insert(iName1);
	fNames.insert(iName2);
	}

ZRelHead::ZRelHead(const set<string>& iNames)
:	fUniversal(false)
,	fNames(iNames)
	{}

ZRelHead::ZRelHead(set<string>& ioNames, bool iKnowWhatImDoing)
:	fUniversal(false)
	{
	ZAssert(iKnowWhatImDoing);
	fNames.swap(ioNames);
	}

ZRelHead::ZRelHead(const vector<string>& iNames)
:	fUniversal(false)
,	fNames(iNames.begin(), iNames.end())
	{}

ZRelHead::ZRelHead(const string* iNames, size_t iCount)
:	fUniversal(false)
,	fNames(iNames, iNames + iCount)
	{}

bool ZRelHead::operator==(const ZRelHead& iOther) const
	{ return fUniversal == iOther.fUniversal && fNames == iOther.fNames; }

bool ZRelHead::operator!=(const ZRelHead& iOther) const
	{ return fUniversal != iOther.fUniversal || fNames != iOther.fNames; }

ZRelHead& ZRelHead::operator&=(const ZRelHead& iOther)
	{
	set<string> result;

	set_intersection(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

ZRelHead ZRelHead::operator&(const ZRelHead& iOther) const
	{
	set<string> result;

	set_intersection(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return ZRelHead(result, true);
	}

ZRelHead& ZRelHead::operator|=(const ZRelHead& iOther)
	{
	set<string> result;

	set_union(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

ZRelHead ZRelHead::operator|(const ZRelHead& iOther) const
	{
	set<string> result;

	set_union(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return ZRelHead(result, true);
	}

ZRelHead& ZRelHead::operator-=(const ZRelHead& iOther)
	{
	set<string> result;

	set_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

ZRelHead ZRelHead::operator-(const ZRelHead& iOther) const
	{
	set<string> result;

	set_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return ZRelHead(result, true);
	}

ZRelHead& ZRelHead::operator^=(const ZRelHead& iOther)
	{
	set<string> result;

	set_symmetric_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	fNames.swap(result);

	return *this;
	}

ZRelHead ZRelHead::operator^(const ZRelHead& iOther) const
	{
	set<string> result;

	set_symmetric_difference(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end(),
		inserter(result, result.end()));

	return ZRelHead(result, true);
	}

bool ZRelHead::Contains(const ZRelHead& iOther) const
	{
	return includes(fNames.begin(), fNames.end(),
		iOther.fNames.begin(), iOther.fNames.end());
	}

bool ZRelHead::Contains(const string& iName) const
	{ return fNames.end() != fNames.find(iName); }

void ZRelHead::Add(const string& iName)
	{
	if (fUniversal)
		fNames.insert(iName);
	else
		fNames.erase(iName);
	}

void ZRelHead::Remove(const string& iName)
	{
	if (fUniversal)
		fNames.erase(iName);
	else
		fNames.insert(iName);
	}

void ZRelHead::GetNames(bool& oUniversal, std::set<string>& oNames) const
	{
	oUniversal = fUniversal;
	oNames = fNames;
	}

ZRelHead operator|(const string& iName, const ZRelHead& iRelHead)
	{
	return iRelHead | iName;
	}

ZRelHead operator&(const string& iName, const ZRelHead& iRelHead)
	{
	return iRelHead & iName;
	}

NAMESPACE_ZOOLIB_END
