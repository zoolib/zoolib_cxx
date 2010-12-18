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
#include "zoolib/zqe/ZQE_Row.h"

using std::set;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCompare

template <> int sCompare_T(const ZRef<ZQE::Row>& iL, const ZRef<ZQE::Row>& iR)
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

ZMACRO_CompareRegistration_T(ZRef<ZQE::Row>)

template <> int sCompare_T(const ZRef<ZQE::RowVector>& iL, const ZRef<ZQE::RowVector>& iR)
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

ZMACRO_CompareRegistration_T(ZRef<ZQE::RowVector>)

namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Row

Row::Row()
	{}

Row::~Row()
	{}

void Row::GetAnnotations(std::set<ZRef<ZCounted> >& ioAnnotations)
	{}

int Row::Compare(const ZRef<Row>& iOther)
	{
	for (size_t iterThis = 0, iterOther = 0,
		countThis = this->Count(), countOther = iOther->Count();
		/*no test*/; ++iterThis, ++iterOther)
		{
		if (iterThis != countThis)
			{
			// Left is not exhausted.
			if (iterOther != countOther)
				{
				// Right is not exhausted either, so we compare their current values.
				if (int compare = sCompare_T(this->Get(iterThis), iOther->Get(iterOther)))
					{
					// The current values of left and right
					// are different, so we have a result.
					return compare;
					}
				}
			else
				{
				// Exhausted right, but still have left
				// remaining, so left is greater than right.
				return 1;
				}
			}
		else
			{
			// Exhausted left.
			if (iterOther != countOther)
				{
				// Still have right remaining, so left is less than right.
				return -1;
				}
			else
				{
				// Exhausted right. And as left is also
				// exhausted left equals right.
				return 0;
				}
			}
		}
	}


// =================================================================================================
#pragma mark -
#pragma mark * RowVector

RowVector::RowVector()
	{}

RowVector::RowVector(const std::vector<ZRef<Row> >& iRows)
:	fRows(iRows)
	{}

RowVector::RowVector(std::vector<ZRef<Row> >* ioRows)
	{ ioRows->swap(fRows); }
	
RowVector::~RowVector()
	{}

size_t RowVector::Count()
	{ return fRows.size(); }

ZRef<Row> RowVector::Get(size_t iIndex)
	{
	if (iIndex < fRows.size())
		return fRows[iIndex];
	return null;
	}

int RowVector::Compare(const ZRef<RowVector>& iOther)
	{ return sCompare_T(fRows, iOther->fRows); }

// =================================================================================================
#pragma mark -
#pragma mark * Row_Val

Row_Val::Row_Val(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

Row_Val::~Row_Val()
	{}

size_t Row_Val::Count()
	{ return 1; }

ZVal_Any Row_Val::Get(size_t iIndex)
	{
	if (iIndex == 0)
		return fVal;
	return ZVal_Any();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Row_Pair

Row_Pair::Row_Pair(const ZRef<Row>& iLeft, const ZRef<Row>& iRight)
:	fLeft(iLeft)
,	fRight(iRight)
	{}

Row_Pair::~Row_Pair()
	{}

size_t Row_Pair::Count()
	{ return fLeft->Count() + fRight->Count(); }

ZVal_Any Row_Pair::Get(size_t iIndex)
	{
	const size_t leftCount = fLeft->Count();
	if (iIndex >= leftCount)
		return fRight->Get(iIndex - leftCount);
	else
		return fLeft->Get(iIndex);
	}

void Row_Pair::GetAnnotations(set<ZRef<ZCounted> >& ioAnnotations)
	{
	fLeft->GetAnnotations(ioAnnotations);
	fRight->GetAnnotations(ioAnnotations);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Row_ValVector

Row_ValVector::Row_ValVector(const vector<ZVal_Any>& iVals)
:	fVals(iVals)
	{}

Row_ValVector::Row_ValVector(vector<ZVal_Any>* ioVals)
	{
	ioVals->swap(fVals);
	}

Row_ValVector::~Row_ValVector()
	{}

size_t Row_ValVector::Count()
	{ return fVals.size(); }

ZVal_Any Row_ValVector::Get(size_t iIndex)
	{
	if (iIndex < fVals.size())
		return fVals[iIndex];
	return ZVal_Any();
	}

void Row_ValVector::GetAnnotations(set<ZRef<ZCounted> >& ioAnnotations)
	{ ioAnnotations.insert(fAnnotations.begin(), fAnnotations.end()); }

void Row_ValVector::AddAnnotation(ZRef<ZCounted> iCounted)
	{ fAnnotations.insert(iCounted); }

void Row_ValVector::AddAnnotations(const std::set<ZRef<ZCounted> >& iAnnotations)
	{ fAnnotations.insert(iAnnotations.begin(), iAnnotations.end()); }

} // namespace ZQE
} // namespace ZooLib
