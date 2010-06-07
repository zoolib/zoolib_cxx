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

#include "zoolib/ZExpr_Logic_ValPred.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Project

Iterator_Any_Project::Iterator_Any_Project(
	ZRef<Iterator> iIterator, const ZRA::RelHead& iRelHead)
:	fIterator(iIterator)
,	fRelHead(iRelHead)
	{}

ZRef<Iterator> Iterator_Any_Project::Clone()
	{ return new Iterator_Any_Project(fIterator->Clone(), fRelHead); }

ZRef<Result> Iterator_Any_Project::ReadInc()
	{
	if (ZRef<Result> theResult = fIterator->ReadInc())
		{
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			const ZAny theAny = theResult_Any->GetVal();
			if (const ZMap_Any* theMap = theAny.PGet_T<ZMap_Any>())
				{
				ZRA::RelHead sourceRelHead;
				ZMap_Any newMap;
				for (ZMap_Any::Index_t i = theMap->Begin(); i != theMap->End(); ++i)
					{
					const string8 theName = theMap->NameOf(i);
					sourceRelHead |= theName;
					if (fRelHead.Contains(theName))
						newMap.Set(theName, theMap->Get(i));
					}
				if (sourceRelHead.Contains(fRelHead))
					return new Result_Any(newMap, theResult_Any);
				}
			else
				{
				return theResult_Any;
				}
			}
		}
	return nullref;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Rename

Iterator_Any_Rename::Iterator_Any_Rename(
	ZRef<Iterator> iIterator, const ZRA::RelName& iNew, const ZRA::RelName& iOld)
:	fIterator(iIterator)
,	fNew(iNew)
,	fOld(iOld)
	{}

ZRef<Iterator> Iterator_Any_Rename::Clone()
	{ return new Iterator_Any_Rename(fIterator->Clone(), fNew, fOld); }

ZRef<Result> Iterator_Any_Rename::ReadInc()
	{
	if (ZRef<Result> theResult = fIterator->ReadInc())
		{
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			const ZAny& theAny = theResult_Any->GetVal();
			if (const ZMap_Any* theMap = theAny.PGet_T<ZMap_Any>())
				{
				ZMap_Any newMap = *theMap;
				ZMap_Any::Index_t theIndex = newMap.IndexOf(fOld);
				if (theIndex != newMap.End())
					{
					const ZVal_Any theVal = newMap.Get(theIndex);
					newMap.Erase(theIndex);
					newMap.Set(fNew, theVal);

					return new Result_Any(newMap, theResult_Any);
					}
				}
			return theResult_Any;
			}
		}
	return nullref;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Restrict

Iterator_Any_Restrict::Iterator_Any_Restrict(
	ZRef<Iterator> iIterator, const ZValPred& iValPred)
:	fIterator(iIterator)
,	fValPred(iValPred)
	{}

ZRef<Iterator> Iterator_Any_Restrict::Clone()
	{ return new Iterator_Any_Restrict(fIterator->Clone(), fValPred); }

ZRef<Result> Iterator_Any_Restrict::ReadInc()
	{
	for (;;)
		{
		ZRef<Result> theResult = fIterator->ReadInc();
		if (!theResult)
			return nullref;
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			ZValContext theContext;
			if (fValPred.Matches(theContext, theResult_Any->GetVal()))
				return theResult;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

Iterator_Any_Select::Iterator_Any_Select(ZRef<Iterator> iIterator, ZRef<ZExpr_Logic> iExpr_Logic)
:	fIterator(iIterator)
,	fExpr_Logic(iExpr_Logic)
	{}

ZRef<Iterator> Iterator_Any_Select::Clone()
	{ return new Iterator_Any_Select(fIterator->Clone(), fExpr_Logic); }

ZRef<Result> Iterator_Any_Select::ReadInc()
	{
	for (;;)
		{
		ZRef<Result> theResult = fIterator->ReadInc();
		if (!theResult)
			return nullref;
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			if (sMatches(fExpr_Logic, theResult_Any->GetVal()))
				return theResult;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator_Any

void Visitor_DoMakeIterator_Any::Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		this->pSetResult(new ZQE::Iterator_Any_Project(theIterator, iExpr->GetProjectRelHead()));
	}

void Visitor_DoMakeIterator_Any::Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		{
		this->pSetResult(
			new ZQE::Iterator_Any_Rename(theIterator, iExpr->GetNew(), iExpr->GetOld()));
		}
	}

void Visitor_DoMakeIterator_Any::Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		this->pSetResult(new ZQE::Iterator_Any_Restrict(theIterator, iExpr->GetValPred()));
	}

void Visitor_DoMakeIterator_Any::Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->Do(iExpr->GetOp0()))
		this->pSetResult(new ZQE::Iterator_Any_Select(theIterator, iExpr->GetExpr_Logic()));
	}

} // namespace ZQE
} // namespace ZooLib
