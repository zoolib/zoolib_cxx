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

#include "zoolib/ZRelHead.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRelHead, extra operators

ZRelHead operator|(const char* iElem, const ZRelHead& iRelHead)
	{ return iRelHead | std::string(iElem); }

ZRelHead operator|(const ZRelHead& iRelHead, const char* iElem)
	{ return iRelHead | std::string(iElem); }

ZRelHead operator&(const ZRelHead& iRelHead, const char* iElem)
	{ return iRelHead & std::string(iElem); }

ZRelHead operator&(const char* iElem, const ZRelHead& iRelHead)
	{ return iRelHead & std::string(iElem); }

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * ZRelHead, tests

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
