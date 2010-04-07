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

#include "zoolib/zqe/ZQE_Iterator.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

Iterator::Iterator()
	{}

Iterator::~Iterator()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Intersect

Iterator_Intersect::Iterator_Intersect(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Result> Iterator_Intersect::ReadInc()
	{
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
						fIterator_RHS->Rewind();
						return theLHS;
						}
					}
				else
					{
					fIterator_RHS->Rewind();
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

void Iterator_Intersect::Rewind()
	{
	fIterator_LHS->Rewind();
	fIterator_RHS->Rewind();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Join

Iterator_Join::Iterator_Join(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Result> Iterator_Join::ReadInc()
	{
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
				if (ZRef<Result> joinedResult = fResult_LHS->JoinedWith(theRHS_Result))
					return joinedResult;
				continue;
				}
			fIterator_RHS->Rewind();
			fResult_LHS.Clear();
			break;
			}
		}
	}

void Iterator_Join::Rewind()
	{
	fIterator_LHS->Rewind();
	fIterator_RHS->Rewind();
	fResult_LHS.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Union

Iterator_Union::Iterator_Union(ZRef<Iterator> iIterator_LHS, ZRef<Iterator> iIterator_RHS)
:	fIterator_LHS(iIterator_LHS)
,	fIterator_RHS(iIterator_RHS)
	{}

ZRef<Result> Iterator_Union::ReadInc()
	{
	if (ZRef<Result> result = fIterator_LHS->ReadInc())
		return result;

	if (ZRef<Result> result = fIterator_LHS->ReadInc())
		return result;

	return ZRef<Result>();
	}

void Iterator_Union::Rewind()
	{
	fIterator_LHS->Rewind();
	fIterator_RHS->Rewind();
	}

} // namespace ZQE
NAMESPACE_ZOOLIB_END
