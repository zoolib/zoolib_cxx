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

#include "zoolib/zqe/ZQE_Iterator_Std.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Intersect

Iterator_Intersect::Iterator_Intersect(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Iterator> Iterator_Intersect::Clone()
	{
	if (fIterator_RHS_Model)
		{
		// Hmm -- we could/should require that a Clone can
		// happen only prior to first call to ReadInc.
		return new Iterator_Intersect(
			fIterator_LHS->Clone(), fIterator_RHS->Clone(), fIterator_RHS_Model->Clone());
		}
	else
		{
		return new Iterator_Intersect(fIterator_LHS->Clone(), fIterator_RHS->Clone());
		}
	}

ZRef<Result> Iterator_Intersect::ReadInc()
	{
	if (!fIterator_RHS_Model)
		fIterator_RHS_Model = fIterator_RHS->Clone();

	for (;;)
		{
		if (ZRef<Result> theLHS = fIterator_LHS->ReadInc())
			{
			for (;;)
				{
				if (ZRef<Result> theRHS = fIterator_RHS->ReadInc())
					{
					if (theLHS->SameAs(theRHS))
						{
						fIterator_RHS = fIterator_RHS_Model->Clone();
						return theLHS;
						}
					}
				else
					{
					fIterator_RHS = fIterator_RHS_Model->Clone();
					break;
					}
				}
			}
		else
			{
			return ZRef<Result>();
			}
		}
	}

Iterator_Intersect::Iterator_Intersect(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS,
	ZRef<Iterator> iIterator_RHS_Model)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
,	fIterator_RHS_Model(iIterator_RHS_Model)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Product

Iterator_Product::Iterator_Product(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Iterator> Iterator_Product::Clone()
	{
	if (fIterator_RHS_Model)
		{
		return new Iterator_Product(
			fIterator_LHS->Clone(), fIterator_RHS->Clone(), fIterator_RHS_Model->Clone());
		}
	else
		{
		return new Iterator_Product(fIterator_LHS->Clone(), fIterator_RHS->Clone());
		}
	}

ZRef<Result> Iterator_Product::ReadInc()
	{
	if (!fIterator_RHS_Model)
		fIterator_RHS_Model = fIterator_RHS->Clone();

	for (;;)
		{
		if (!fResult_LHS)
			{
			fResult_LHS = fIterator_LHS->ReadInc();
			if (!fResult_LHS)
				return ZRef<Result>();
			}
		
		for (;;)
			{
			if (ZRef<Result> theRHS_Result = fIterator_RHS->ReadInc())
				{
				if (ZRef<Result> joinedResult = fResult_LHS->ProductWith(theRHS_Result))
					return joinedResult;
				continue;
				}
			fIterator_RHS = fIterator_RHS_Model->Clone();
			fResult_LHS.Clear();
			break;
			}
		}
	}

Iterator_Product::Iterator_Product(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS,
	ZRef<Iterator> iIterator_RHS_Model)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
,	fIterator_RHS_Model(iIterator_RHS_Model)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Union

Iterator_Union::Iterator_Union(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Iterator> Iterator_Union::Clone()
	{ return new Iterator_Union(fIterator_LHS->Clone(), fIterator_RHS->Clone()); }

ZRef<Result> Iterator_Union::ReadInc()
	{
	if (ZRef<Result> result = fIterator_LHS->ReadInc())
		return result;

	if (ZRef<Result> result = fIterator_RHS->ReadInc())
		return result;

	return ZRef<Result>();
	}

} // namespace ZQE
} // namespace ZooLib
