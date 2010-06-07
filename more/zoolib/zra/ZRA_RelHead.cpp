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

#include "zoolib/zra/ZRA_RelHead.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Rename_t

/*
Rename_t sInvert(const Rename_t& iRename)
	{
	Rename_t result;
	for (Rename_t::const_iterator i = iRename.begin(); i != iRename.end(); ++i)
		result[(*i).second] = (*i).first;

	return result;
	}
*/

} // namespace ZRA
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * RelHead, tests

#if ! ZCONFIG_ExcludeTests

#include "zoolib/ZStrim.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

using namespace ZooLib;
using namespace ZRA;

const ZStrimW& operator<<(const ZStrimW& iStrimW, const RelHead& iRelHead)
	{
	Util_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW);
	return iStrimW;
	}

#define show(a) s << #a << ": " << (a) << "\n";

#define verify(a,b) s << #a << ": " << (a); \
	check(s, a, b);

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
	const RelHead none;
//	const RelHead univ = RelHead::sUniversal();
	RelHead a = RelHead().Insert("A");
	RelHead b = RelHead().Insert("B");
	RelHead c = RelHead().Insert("C");
	RelHead ab = RelHead().Insert("A").Insert("B");
	RelHead ac = RelHead().Insert("A").Insert("C");
	RelHead bc = RelHead().Insert("B").Insert("C");
	RelHead abc = RelHead().Insert("A").Insert("B").Insert("C");

#if 0
	RelHead _a = RelHead::sUniversal().Erase("A");
	RelHead _b = RelHead::sUniversal().Erase("B");
	RelHead _c = RelHead::sUniversal().Erase("C");
	RelHead _ab = RelHead::sUniversal().Erase("A").Erase("B");
	RelHead _ac = RelHead::sUniversal().Erase("A").Erase("C");
	RelHead _bc = RelHead::sUniversal().Erase("B").Erase("C");
	RelHead _abc = RelHead::sUniversal().Erase("A").Erase("B").Erase("C");
#endif

	show(none)
//	show(univ)
	show(a)
	show(b)
	show(c)
	show(ab)
	show(ac)
	show(bc)
	show(abc)
#if 0
	show(_a)
	show(_b)
	show(_c)
	show(_ab)
	show(_ac)
	show(_bc)
	show(_abc)
#endif

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

#if 0
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
#endif
	}

#endif // ! ZCONFIG_ExcludeTests
