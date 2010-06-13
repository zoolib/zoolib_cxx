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

namespace ZooLib {
namespace ZQE {

using std::string;
using std::vector;

static ZMap_Any spProduct(const ZMap_Any& l, const ZMap_Any& r)
	{
	ZMap_Any result = r;
	for (ZMap_Any::Index_t lIter = l.Begin(), lEnd = l.End();
		lIter != lEnd; ++lIter)
		{
		result.Set(l.NameOf(lIter), l.Get(lIter));
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Result_Any

Result_Any::Result_Any(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

Result_Any::Result_Any(const ZVal_Any& iVal, ZRef<Result_Any> iOther)
:	fVal(iVal)
,	fAnnotations(iOther->GetAnnotations())
	{}

bool Result_Any::SameAs(ZRef<Result> iOther)
	{
	if (ZRef<Result_Any> other = iOther.DynamicCast<Result_Any>())
		return 0 == sCompare_T(fVal, other->GetVal());

	ZUnimplemented();
	return false;
	}

ZRef<Result> Result_Any::ProductWith(ZRef<Result> iOther)
	{
	if (ZRef<Result_Any> other = iOther.DynamicCast<Result_Any>())
		{
		if (const ZMap_Any* mapThis = fVal.PGet_T<ZMap_Any>())
			{
			if (const ZMap_Any* mapOther = other->GetVal().PGet_T<ZMap_Any>())
				{
				ZRef<Result_Any> theResult = new Result_Any(spProduct(*mapThis, *mapOther));
				theResult->AddAnnotations(this->GetAnnotations());
				theResult->AddAnnotations(other->GetAnnotations());
				return theResult;
				}
			}
		}
	return null;
	}

const ZVal_Any& Result_Any::GetVal()
	{ return fVal; }

void Result_Any::AddAnnotation(ZRef<ZCounted> iCounted)
	{ fAnnotations.insert(iCounted); }

void Result_Any::AddAnnotations(const std::set<ZRef<ZCounted> >& iAnnotations)
	{ fAnnotations.insert(iAnnotations.begin(), iAnnotations.end()); }

const std::set<ZRef<ZCounted> >& Result_Any::GetAnnotations()
	{ return fAnnotations; }

} // namespace ZQE
} // namespace ZooLib
