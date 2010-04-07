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

#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/valbase/ZValBase_Any.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_Any {

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(const ZSeq_Any& iSeq);
	Iterator(const ZSeq_Any& iSeq, size_t iIndex);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

private:
	const ZSeq_Any fSeq;
	size_t fIndex;
	};

Iterator::Iterator(const ZSeq_Any& iSeq)
:	fSeq(iSeq)
,	fIndex(0)
	{}

Iterator::Iterator(const ZSeq_Any& iSeq, size_t iIndex)
:	fSeq(iSeq)
,	fIndex(iIndex)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fSeq, fIndex); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{
	if (fIndex < fSeq.Count())
		return new ZQE::Result_Any(fSeq.Get(fIndex++));
	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete

ExprRep_Concrete::ExprRep_Concrete(const ZSeq_Any& iSeq)
:	fSeq(iSeq)
	{}

ZRelHead ExprRep_Concrete::GetRelHead()
	{ return ZRelHead(true); }

const ZSeq_Any& ExprRep_Concrete::GetSeq()
	{ return fSeq; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Concrete_MakeIterator

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Concrete(ZRef<ZQL::ExprRep_Concrete> iRep)
	{
	if (ZRef<ExprRep_Concrete> theRep = iRep.DynamicCast<ExprRep_Concrete>())
		{
		fIterator = new Iterator(theRep->GetSeq());
		return true;
		}
	return Visitor_ExprRep_Concrete::Visit_Concrete(iRep);
	}

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Restrict(ZRef<ZQL::ExprRep_Restrict> iRep)
	{
	// Could specialize here if we discover that theIterator is ours.
	if (ZRef<ZQE::Iterator> theIterator = this->MakeIterator(iRep->GetExprRep()))
		fIterator = new ZQE::Iterator_Any_Restrict(iRep->GetValCondition(), theIterator);

	return true;	
	}

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Select(ZRef<ZQL::ExprRep_Select> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->MakeIterator(iRep->GetExprRep_Relation()))
		fIterator = new ZQE::Iterator_Any_Select(iRep->GetExprRep_Logic(), theIterator);

	return true;	
	}

} // namespace ZValBase_Seq
NAMESPACE_ZOOLIB_END
