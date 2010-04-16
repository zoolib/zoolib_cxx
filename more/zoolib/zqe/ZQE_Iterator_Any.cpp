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

#include "zoolib/ZExpr_Logic_ValCondition.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Project

Iterator_Any_Project::Iterator_Any_Project(
	const ZQL::RelHead& iRelHead, ZRef<Iterator> iIterator)
:	fRelHead(iRelHead)
,	fIterator(iIterator)
	{}

ZRef<Iterator> Iterator_Any_Project::Clone()
	{ return new Iterator_Any_Project(fRelHead, fIterator->Clone()); }

ZRef<Result> Iterator_Any_Project::ReadInc()
	{
	if (ZRef<Result> theResult = fIterator->ReadInc())
		{
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			const ZAny theAny = theResult_Any->GetVal();
			if (const ZMap_Any* theMap = theAny.PGet_T<ZMap_Any>())
				{
				ZMap_Any newMap;
				for (ZMap_Any::Index_t i = theMap->Begin(); i != theMap->End(); ++i)
					{
					const string theName = theMap->NameOf(i);
					if (fRelHead.Contains(theName))
						newMap.Set(theName, theMap->Get(i));
					}
				return new Result_Any(newMap);
				}
			return theResult_Any;
			}
		}
	return nullref;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Rename

Iterator_Any_Rename::Iterator_Any_Rename(
	const std::string& iNew, const std::string& iOld, ZRef<Iterator> iIterator)
:	fNew(iNew)
,	fOld(iOld)
,	fIterator(iIterator)
	{}

ZRef<Iterator> Iterator_Any_Rename::Clone()
	{ return new Iterator_Any_Rename(fNew, fOld, fIterator->Clone()); }

ZRef<Result> Iterator_Any_Rename::ReadInc()
	{
	if (ZRef<Result> theResult = fIterator->ReadInc())
		{
		if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
			{
			const ZAny theAny = theResult_Any->GetVal();
			if (const ZMap_Any* theMap = theAny.PGet_T<ZMap_Any>())
				{
				ZMap_Any newMap = *theMap;
				ZMap_Any::Index_t theIndex = newMap.IndexOf(fOld);
				if (theIndex != newMap.End())
					{
					const ZVal_Any theVal = newMap.Get(theIndex);
					newMap.Erase(theIndex);
					newMap.Set(fNew, theVal);
					return new Result_Any(newMap);
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
	const ZValCondition& iValCondition, ZRef<Iterator> iIterator)
:	fValCondition(iValCondition)
,	fIterator(iIterator)
	{}

ZRef<Iterator> Iterator_Any_Restrict::Clone()
	{ return new Iterator_Any_Restrict(fValCondition, fIterator->Clone()); }

ZRef<Result> Iterator_Any_Restrict::ReadInc()
	{
	for (;;)
		{
		if (ZRef<Result> theResult = fIterator->ReadInc())
			{
			if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
				{
				ZValContext theContext;
				if (fValCondition.Matches(theContext, theResult_Any->GetVal()))
					return theResult;
				}
			}
		else
			{
			return nullref;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

Iterator_Any_Select::Iterator_Any_Select(ZRef<ZExpr_Logic> iExpr_Logic, ZRef<Iterator> iIterator)
:	fExpr_Logic(iExpr_Logic)
,	fIterator(iIterator)
	{}

ZRef<Iterator> Iterator_Any_Select::Clone()
	{ return new Iterator_Any_Select(fExpr_Logic, fIterator->Clone()); }

ZRef<Result> Iterator_Any_Select::ReadInc()
	{
	for (;;)
		{
		if (ZRef<Result> theResult = fIterator->ReadInc())
			{
			if (ZRef<Result_Any> theResult_Any = theResult.DynamicCast<Result_Any>())
				{
				if (sMatches(fExpr_Logic, theResult_Any->GetVal()))
					return theResult;
				}
			}
		else
			{
			return nullref;
			}
		}
	}

} // namespace ZQE
NAMESPACE_ZOOLIB_END
