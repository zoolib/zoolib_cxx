#if 0
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

#ifndef __ZDataspace_SearchSpec__
#define __ZDataspace_SearchSpec__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZValPredCompound.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"
#include "zoolib/zra/ZRA_NameMap.h"

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * SearchSpec

class SearchSpec
	{
public:
	std::vector<ZNameVal> fNameVals;
	std::vector<ZRA::NameMap> fNameMaps;
	ZValPredCompound fPredCompound;
	};

bool operator==(const SearchSpec& l, const SearchSpec& r);
bool operator<(const SearchSpec& l, const SearchSpec& r);

SearchSpec sAsSearchSpec(ZRef<ZRA::Expr_Rel> iRel);

} // namespace ZDataspace
} // namespace ZooLib

namespace ZooLib {
template <> int sCompare_T(const ZDataspace::SearchSpec& iL, const ZDataspace::SearchSpec& iR);
} // namespace ZooLib

#endif // __ZDataspace_SearchSpec__
#endif
