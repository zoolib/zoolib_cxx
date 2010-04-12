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

#include "zoolib/zql/ZQL_RelHead.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * RelHead, extra operators

RelHead operator|(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead | std::string(iElem); }

RelHead operator|(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead | std::string(iElem); }

RelHead operator&(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead & std::string(iElem); }

RelHead operator&(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead & std::string(iElem); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * RelHead, tests

#if ! ZCONFIG_ExcludeTests

#include "zoolib/ZStrim.h"
#include "zoolib/zql/ZQL_Util_Strim_RelHead.h"

NAMESPACE_ZOOLIB_USING
using namespace ZQL;

const ZStrimW& operator<<(const ZStrimW& iStrimW, const RelHead& iRelHead)
	{
	Util_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW);
	return iStrimW;
	}

#define show(a) s << #a << ": " << (a) << "\n";

#define verify(a,b) s << #a << ": " << (a); \
	check(s, a,b);

static void check(const ZStrimW& w, const RelHead& sample, const RelHead& expected)
	{
	if (sample != expected)
		w << ", Should be: " << expected;
	else
		w << ", OK";
	w << "\n";
	}

void sTestRelHead(const ZStrimW& s)
	{
	const RelHead none(false);
	const RelHead univ(true);
	RelHead a = RelHead().Insert("A");
	RelHead b = RelHead().Insert("B");
	RelHead c = RelHead().Insert("C");
	RelHead ab = RelHead().Insert("A").Insert("B");
	RelHead ac = RelHead().Insert("A").Insert("C");
	RelHead bc = RelHead().Insert("B").Insert("C");
	RelHead abc = RelHead().Insert("A").Insert("B").Insert("C");

	RelHead _a = RelHead(true).Erase("A");
	RelHead _b = RelHead(true).Erase("B");
	RelHead _c = RelHead(true).Erase("C");
	RelHead _ab = RelHead(true).Erase("A").Erase("B");
	RelHead _ac = RelHead(true).Erase("A").Erase("C");
	RelHead _bc = RelHead(true).Erase("B").Erase("C");
	RelHead _abc = RelHead(true).Erase("A").Erase("B").Erase("C");

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
