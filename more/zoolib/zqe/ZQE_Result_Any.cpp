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

#include "zoolib/ZUtil_STL.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

using std::string;
using std::vector;

static ZMap_Any spJoin(const ZMap_Any& l, const ZMap_Any& r)
	{
	typedef ZMap_Any::Index_t Index_t;

	ZMap_Any result;
	vector<string> rVisited;
	Index_t rEnd = r.End();

	for (Index_t lIter = l.Begin(), lEnd = l.End();
		lIter != lEnd; ++lIter)
		{
		const string& lName = l.NameOf(lIter);
		const ZVal_Any lVal = l.Get(lIter);
		Index_t rIter = r.IndexOf(lName);
		if (rIter == rEnd)
			{
			// It's in l, not in r. Include it in result.
			result.Set(lName, lVal);			
			}
		else
			{
			if (0 == lVal.Compare(r.Get(rIter)))
				{
				// They're the same.
				result.Set(lName, lVal);
				rVisited.push_back(lName);
				}
			else
				{
				// They're different, and we should bail;
				return ZMap_Any();
				}
			}
		}
	// We've got all the values in l that are not in r, or are in r with
	// the same value as that in l. Now include all the remaining values from r.
	for (Index_t rIter = r.Begin(); rIter != rEnd; ++rIter)
		{
		const string rName = r.NameOf(rIter);
		if (!ZUtil_STL::sContains(rVisited, rName))
			result.Set(rName, r.Get(rIter));
		}

	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Result_Any

Result_Any::Result_Any(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

bool Result_Any::SameAs(ZRef<Result> iOther)
	{
	if (ZRef<Result_Any> other = iOther.DynamicCast<Result_Any>())
		return 0 == sCompare_T(fVal, other->GetVal());

	ZUnimplemented();
	return false;
	}

ZRef<Result> Result_Any::JoinedWith(ZRef<Result> iOther)
	{
	if (ZRef<Result_Any> other = iOther.DynamicCast<Result_Any>())
		{
		if (const ZMap_Any* mapThis = fVal.PGet_T<ZMap_Any>())
			{
			if (const ZMap_Any* mapOther = other->GetVal().PGet_T<ZMap_Any>())
				{
				if (const ZMap_Any result = spJoin(*mapThis, *mapOther))
					return new Result_Any(result);
				}
			}
		}
	return nullref;
	}

const ZVal_Any& Result_Any::GetVal()
	{ return fVal; }

} // namespace ZQE
NAMESPACE_ZOOLIB_END
