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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/ZCompare_vector.h"
#include "zoolib/QueryEngine/Result.h"

using std::set;
using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T<QueryEngine::Result>(
	const QueryEngine::Result& iL, const QueryEngine::Result& iR)
	{ return iL.Compare(iR); }

ZMACRO_CompareRegistration_T(QueryEngine::Result)

template <>
int sCompare_T<ZRef<QueryEngine::Result> >(const ZRef<QueryEngine::Result>& iL, const ZRef<QueryEngine::Result>& iR)
	{ return sCompare_Ref_T(iL, iR); }

ZMACRO_CompareRegistration_T(ZRef<QueryEngine::Result>)

// =================================================================================================
// MARK: - QueryEngine::Result

namespace QueryEngine {

Result::Result(const RelationalAlgebra::RelHead& iRelHead,
	vector<ZVal_Any>* ioPackedRows)
:	fRelHead(iRelHead)
	{
	ioPackedRows->swap(fPackedRows);
	}

Result::Result(const ZRef<Result>& iOther, size_t iRow)
:	fRelHead(iOther->GetRelHead())
	{
	if (iRow < iOther->Count())
		{
		const ZVal_Any* theVals = iOther->GetValsAt(iRow);
		fPackedRows.insert(fPackedRows.end(), theVals, theVals + fRelHead.size());
		}
	}

Result::~Result()
	{}

const RelationalAlgebra::RelHead& Result::GetRelHead()
	{ return fRelHead; }

size_t Result::Count()
	{
	if (const size_t theSize = fRelHead.size())
		return fPackedRows.size() / theSize;
	return 0;
	}

const ZVal_Any* Result::GetValsAt(size_t iIndex)
	{ return &fPackedRows[fRelHead.size() * iIndex]; }

int Result::Compare(const Result& iOther) const
	{
	if (int compare = sCompare_T(fRelHead, iOther.fRelHead))
		return compare;
	return sCompare_T(fPackedRows, iOther.fPackedRows);
	}

} // namespace QueryEngine
} // namespace ZooLib
