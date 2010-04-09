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

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Restrict

Iterator_Any_Restrict::Iterator_Any_Restrict(
	const ZValCondition& iValCondition, ZRef<Iterator> iIterator)
:	fValCondition(iValCondition)
,	fIterator(iIterator)
	{}

ZRef<ZQE::Iterator> Iterator_Any_Restrict::Clone()
	{ return new Iterator_Any_Restrict(fValCondition, fIterator->Clone()); }

ZRef<ZQE::Result> Iterator_Any_Restrict::ReadInc()
	{
	for (;;)
		{
		if (ZRef<ZQE::Result> theZQEResult = fIterator->ReadInc())
			{
			if (ZRef<ZQE::Result_Any> theResult =
				theZQEResult.DynamicCast<ZQE::Result_Any>())
				{
				ZValContext theContext;
				if (fValCondition.Matches(theContext, theResult->GetVal()))
					return theResult;
				}
			}
		else
			{
			return ZRef<ZQE::Result>();
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator_Any_Select

Iterator_Any_Select::Iterator_Any_Select(
	ZRef<ZExpr_Logic> iExpr_Logic, ZRef<ZQE::Iterator> iIterator)
:	fExpr_Logic(iExpr_Logic)
,	fIterator(iIterator)
	{}

ZRef<ZQE::Iterator> Iterator_Any_Select::Clone()
	{ return new Iterator_Any_Select(fExpr_Logic, fIterator->Clone()); }

ZRef<ZQE::Result> Iterator_Any_Select::ReadInc()
	{
	for (;;)
		{
		if (ZRef<ZQE::Result> theZQEResult = fIterator->ReadInc())
			{
			if (ZRef<ZQE::Result_Any> theResult =
				theZQEResult.DynamicCast<ZQE::Result_Any>())
				{
				if (sMatches(fExpr_Logic, theResult->GetVal()))
					return theResult;
				}
			}
		else
			{
			return ZRef<ZQE::Result>();
			}
		}
	}

} // namespace ZQE
NAMESPACE_ZOOLIB_END
