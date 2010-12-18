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
#include "zoolib/zqe/ZQE_ResultSet.h"

using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCompare

template <> int sCompare_T(const ZRef<ZQE::ResultSet>& iL, const ZRef<ZQE::ResultSet>& iR)
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

ZMACRO_CompareRegistration_T(ZRef<ZQE::ResultSet>)

namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * ResultSet

ResultSet::ResultSet(const std::vector<string8>& iRowHead, ZRef<RowVector> iRowVector)
:	fRowHead(iRowHead)
,	fRowVector(iRowVector)
	{
	ZAssert(fRowVector);
	}

ResultSet::ResultSet(std::vector<string8>* ioRowHead, ZRef<RowVector> iRowVector)
:	fRowVector(iRowVector)
	{
	ZAssert(fRowVector);
	ioRowHead->swap(fRowHead);
	}

ResultSet::ResultSet(std::vector<string8>* ioRowHead)
	{
	ioRowHead->swap(fRowHead);
	fRowVector = new RowVector;
	}

const std::vector<string8>& ResultSet::GetRowHead()
	{ return fRowHead; }

ZRef<RowVector> ResultSet::GetRowVector()
	{ return fRowVector; }

int ResultSet::Compare(const ZRef<ResultSet>& iOther)
	{
	if (int compare = sCompare_T(fRowHead, iOther->fRowHead))
		return compare;

	return sCompare_T(fRowVector, iOther->fRowVector);
	}

} // namespace ZQE
} // namespace ZooLib
