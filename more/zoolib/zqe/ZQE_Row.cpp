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

#include "zoolib/zqe/ZQE_Row.h"

namespace ZooLib {
namespace ZQE {

using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Row

Row::Row()
	{}

Row::~Row()
	{}

void Row::GetAnnotations(std::set<ZRef<ZCounted> >& ioAnnotations)
	{}

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
#pragma mark * Row_Vector

Row_Vector::Row_Vector(const vector<ZVal_Any>& iVals)
:	fVals(iVals)
	{}

Row_Vector::Row_Vector(vector<ZVal_Any>* ioVals)
	{
	ioVals->swap(fVals);
	}

Row_Vector::~Row_Vector()
	{}

size_t Row_Vector::Count()
	{ return fVals.size(); }

ZVal_Any Row_Vector::Get(size_t iIndex)
	{
	if (iIndex < fVals.size())
		return fVals[iIndex];
	return ZVal_Any();
	}

void Row_Vector::GetAnnotations(set<ZRef<ZCounted> >& ioAnnotations)
	{ ioAnnotations.insert(fAnnotations.begin(), fAnnotations.end()); }

void Row_Vector::AddAnnotation(ZRef<ZCounted> iCounted)
	{ fAnnotations.insert(iCounted); }

void Row_Vector::AddAnnotations(const std::set<ZRef<ZCounted> >& iAnnotations)
	{ fAnnotations.insert(iAnnotations.begin(), iAnnotations.end()); }

} // namespace ZQE
} // namespace ZooLib
