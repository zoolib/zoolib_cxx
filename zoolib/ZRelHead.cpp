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

namespace ZANONYMOUS {

static void spOr(const set<string>& iLHS, const set<string>& iRHS, set<string>& oResult)
	{
	set_union(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

static void spAnd(const set<string>& iLHS, const set<string>& iRHS, set<string>& oResult)
	{
	set_intersection(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

static void spMinus(const set<string>& iLHS, const set<string>& iRHS, set<string>& oResult)
	{
	set_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

static void spXor(const set<string>& iLHS, const set<string>& iRHS, set<string>& oResult)
	{
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

static bool spIncludes(const set<string>& iLHS, const set<string>& iRHS)
	{
	return includes(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end());
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRelHead

ZRelHead::ZRelHead(bool iUniversal, std::set<std::string>& ioNames, bool iKnowWhatImDoing)
:	fUniversal(iUniversal)
	{
	fNames.swap(ioNames);
	ZAssert(iKnowWhatImDoing);
	}

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
	{}

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
	*this = *this & iOther;
	return *this;
	}

ZRelHead ZRelHead::operator&(const ZRelHead& iOther) const
	{
	set<string> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			spOr(fNames, iOther.fNames, result);
			}
		else
			{
			resultUniversal = false;
			spMinus(iOther.fNames, fNames, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			spMinus(fNames, iOther.fNames, result);
			}
		else
			{
			resultUniversal = false;
			spAnd(iOther.fNames, fNames, result);
			}
		}
	return ZRelHead(resultUniversal, result, true);
	}

ZRelHead& ZRelHead::operator|=(const ZRelHead& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

ZRelHead ZRelHead::operator|(const ZRelHead& iOther) const
	{
	set<string> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			spAnd(fNames, iOther.fNames, result);
			}
		else
			{
			resultUniversal = true;
			spMinus(fNames, iOther.fNames, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			spMinus(iOther.fNames, fNames, result);
			}
		else
			{
			resultUniversal = false;
			spOr(fNames, iOther.fNames, result);
			}
		}
	return ZRelHead(resultUniversal, result, true);
	}

ZRelHead& ZRelHead::operator-=(const ZRelHead& iOther)
	{
	*this = *this - iOther;
	return *this;
	}

ZRelHead ZRelHead::operator-(const ZRelHead& iOther) const
	{
	set<string> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			spMinus(iOther.fNames, fNames, result);
			}
		else
			{
			resultUniversal = true;
			spOr(fNames, iOther.fNames, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			spAnd(fNames, iOther.fNames, result);
			}
		else
			{
			resultUniversal = false;
			spMinus(fNames, iOther.fNames, result);
			}
		}
	return ZRelHead(resultUniversal, result, true);
	}

ZRelHead& ZRelHead::operator^=(const ZRelHead& iOther)
	{
	*this = *this ^ iOther;
	return *this;
	}

ZRelHead ZRelHead::operator^(const ZRelHead& iOther) const
	{
	set<string> result;
	bool resultUniversal = fUniversal ^ iOther.fUniversal;
	spXor(fNames, iOther.fNames, result);
	return ZRelHead(resultUniversal, result, true);
	}

bool ZRelHead::Contains(const ZRelHead& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			return spIncludes(iOther.fNames, fNames);
			}
		else
			{
			set<string> sect;
			spAnd(fNames, iOther.fNames, sect);
			return sect.empty();
			}
		}
	else
		{
		if (iOther.fUniversal)
			return false;
		else
			return spIncludes(fNames, iOther.fNames);
		}
	}

bool ZRelHead::Contains(const string& iName) const
	{ return fUniversal != (fNames.end() != fNames.find(iName)); }

ZRelHead& ZRelHead::Add(const string& iName)
	{
	if (fUniversal)
		fNames.erase(iName);
	else
		fNames.insert(iName);
	return *this;
	}

ZRelHead& ZRelHead::Remove(const string& iName)
	{
	if (fUniversal)
		fNames.insert(iName);
	else
		fNames.erase(iName);
	return *this;
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

#if ! ZCONFIG_ExcludeTests

#include "zoolib/ZStrim.h"
#include "zoolib/ZUtil_Strim_RelHead.h"

NAMESPACE_ZOOLIB_USING

const ZStrimW& operator<<(const ZStrimW& iStrimW, const ZRelHead& iRelHead)
	{
	ZUtil_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW);
	return iStrimW;
	}

#define show(a) s << #a << ": " << (a) << "\n";

#define verify(a,b) s << #a << ": " << (a); \
	check(s, a,b);

static void check(const ZStrimW& w, const ZRelHead& sample, const ZRelHead& expected)
	{
	if (sample != expected)
		w << ", Should be: " << expected;
	else
		w << ", OK";
	w << "\n";
	}

void sTestRelHead(const ZStrimW& s)
	{
	const ZRelHead none(false);
	const ZRelHead univ(true);
	ZRelHead a = ZRelHead().Add("A");
	ZRelHead b = ZRelHead().Add("B");
	ZRelHead c = ZRelHead().Add("C");
	ZRelHead ab = ZRelHead().Add("A").Add("B");
	ZRelHead ac = ZRelHead().Add("A").Add("C");
	ZRelHead bc = ZRelHead().Add("B").Add("C");
	ZRelHead abc = ZRelHead().Add("A").Add("B").Add("C");

	ZRelHead _a = ZRelHead(true).Remove("A");
	ZRelHead _b = ZRelHead(true).Remove("B");
	ZRelHead _c = ZRelHead(true).Remove("C");
	ZRelHead _ab = ZRelHead(true).Remove("A").Remove("B");
	ZRelHead _ac = ZRelHead(true).Remove("A").Remove("C");
	ZRelHead _bc = ZRelHead(true).Remove("B").Remove("C");
	ZRelHead _abc = ZRelHead(true).Remove("A").Remove("B").Remove("C");

	show(none)
	show(univ)
	show(a)
	show(b)
	show(c)
	show(ab)
	show(ac)
	show(bc)
	show(abc)
	show(_a)
	show(_b)
	show(_c)
	show(_ab)
	show(_ac)
	show(_bc)
	show(_abc)


	verify(a | a, a)
	verify(a & a, a)
	verify(a - a, none)
	verify(a ^ a, none)

	verify(a | b, ab)
	verify(a & b, none)
	verify(a - b, a)
	verify(a ^ b, ab)

	verify(ab | bc, abc)
	verify(ab & bc, b)
	verify(ab - bc, a)
	verify(ab ^ bc, ac)

	verify(_a | _a, _a)
	verify(_a & _a, _a)
	verify(_a - _a, none)
	verify(_a ^ _a, none)

	verify(_a | b, _a)
	verify(_a & b, b)
	verify(_a - b, _ab)
	verify(_a ^ b, _ab)

	verify(a | _b, _b)
	verify(a & _b, a)
	verify(a - _b, none)
	verify(a ^ _b, _ab)

	verify(_a | _b, univ)
	verify(_a & _b, _ab)
	verify(_a - _b, b)
	verify(_a ^ _b, ab)
	}
#endif // ! ZCONFIG_ExcludeTests
