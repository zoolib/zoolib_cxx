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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompare_Vector.h"
#include "zoolib/zqe/ZQE_Result.h"

using std::set;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCompare

template <> int sCompare_T(const ZRef<ZQE::Result>& iL, const ZRef<ZQE::Result>& iR)
	{
	if (iL)
		{
		if (iR)
			return iL->Compare(iR);
		return 1;
		}
	else if (iR)
		{
		return -1;
		}
	return 0;
	}

ZMACRO_CompareRegistration_T(ZRef<ZQE::Result>)

// =================================================================================================
#pragma mark -
#pragma mark * ZQE::Result

namespace ZQE {

Result::Result(const ZRA::RelHead& iRelHead,
	vector<ZVal_Any>* ioPackedRows,
	vector<vector<ZRef<ZCounted> > >* ioAnnotations)
:	fRelHead(iRelHead)
	{
	ioPackedRows->swap(fPackedRows);
	const size_t theCount = fPackedRows.size() / fRelHead.size();
	if (ioAnnotations)
		{
		ioAnnotations->swap(fAnnotations);
		ZAssert(fAnnotations.size() == theCount);
		}
	}

Result::~Result()
	{}

const ZRA::RelHead& Result::GetRelHead()
	{ return fRelHead; }

size_t Result::Count()
	{ return fPackedRows.size() / fRelHead.size(); }

const ZVal_Any* Result::GetValsAt(size_t iIndex)
	{ return &fPackedRows[fRelHead.size() * iIndex]; }

void Result::GetAnnotationsAt(size_t iIndex, set<ZRef<ZCounted> >& oAnnotations)
	{
	if (iIndex < fAnnotations.size())
		{
		const vector<ZRef<ZCounted> >& theAnnotations = fAnnotations[iIndex];
		oAnnotations.insert(theAnnotations.begin(), theAnnotations.end());
		}
	}

int Result::Compare(const ZRef<Result>& iOther)
	{
	if (int compare = sCompare_T(fRelHead, iOther->fRelHead))
		return compare;
	return sCompare_T(fPackedRows, iOther->fPackedRows);
	}

} // namespace ZQE
} // namespace ZooLib
