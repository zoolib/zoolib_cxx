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

#include "zoolib/ZThread.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/valbase/ZValBase_YadSeqR.h"
#include "zoolib/zqe/ZQE_Visitor_ExprRep_MakeIterator.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zql/ZQL_Expr_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_YadSeqR {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete

class ExprRep_Concrete : public ZQL::ExprRep_Concrete
	{
public:
	ExprRep_Concrete(ZRef<ZYadSeqR> iYadSeqR);

// From ExprRep_Relation via ExprRep_Concrete
	virtual ZRelHead GetRelHead();

// Our protocol
	ZRef<ZQE::Result> ReadInc(size_t& ioIndex);

private:
	ZMtx fMtx;
	ZRef<ZYadSeqR> fYadSeqR;
	ZSeq_Any fSeq;
	};

ExprRep_Concrete::ExprRep_Concrete(ZRef<ZYadSeqR> iYadSeqR)
:	fYadSeqR(iYadSeqR)
	{}

ZRelHead ExprRep_Concrete::GetRelHead()
	{ return ZRelHead(true); }

ZRef<ZQE::Result> ExprRep_Concrete::ReadInc(size_t& ioIndex)
	{
	ZAcqMtx acq(fMtx);

	if (ioIndex < fSeq.Count())
		return new ZQE::Result_Any(fSeq.Get(ioIndex++));

	if (ZRef<ZYadR> theYadR = fYadSeqR->ReadInc())
		{
		const ZVal_Any theVal = sFromYadR(ZVal_Any(), theYadR);
		++ioIndex;
		if (this->GetRefCount() > 1)
			{
			fSeq.Append(theVal);
			ZAssert(ioIndex == fSeq.Count());
			}
		return new ZQE::Result_Any(theVal);
		}

	return ZRef<ZQE::Result>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Iterator

class Iterator : public ZQE::Iterator
	{
public:
	Iterator(ZRef<ExprRep_Concrete> iExprRep, size_t iIndex);

	virtual ~Iterator();
	
	virtual ZRef<ZQE::Iterator> Clone();
	virtual ZRef<ZQE::Result> ReadInc();

protected:
	ZRef<ExprRep_Concrete> fExprRep;
	size_t fIndex;
	};

Iterator::Iterator(ZRef<ExprRep_Concrete> iExprRep, size_t iIndex)
:	fExprRep(iExprRep)
,	fIndex(0)
	{}

Iterator::~Iterator()
	{}

ZRef<ZQE::Iterator> Iterator::Clone()
	{ return new Iterator(fExprRep, fIndex); }

ZRef<ZQE::Result> Iterator::ReadInc()
	{ return fExprRep->ReadInc(fIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Concrete_MakeIterator

class Visitor_ExprRep_Concrete_MakeIterator
:	public virtual ZQL::Visitor_ExprRep_Concrete
,	public virtual ZQL::Visitor_ExprRep_Restrict
,	public virtual ZQL::Visitor_ExprRep_Select
,	public virtual ZQE::Visitor_ExprRep_MakeIterator
	{
public:
// From Visitor_ExprRep_Concrete
	virtual bool Visit_Concrete(ZRef<ZQL::ExprRep_Concrete> iRep);	

// From Visitor_ExprRep_Restrict
	virtual bool Visit_Restrict(ZRef<ZQL::ExprRep_Restrict> iRep);

// From Visitor_ExprRep_Select
	virtual bool Visit_Select(ZRef<ZQL::ExprRep_Select> iRep);
	};

bool Visitor_ExprRep_Concrete_MakeIterator::Visit_Concrete(ZRef<ZQL::ExprRep_Concrete> iRep)
	{
	if (ZRef<ExprRep_Concrete> theRep = iRep.DynamicCast<ExprRep_Concrete>())
		{
		fIterator = new Iterator(theRep, 0);
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

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase_YadSeqR pseudo constructors

ZQL::Expr_Concrete sConcrete(ZRef<ZYadSeqR> iYadSeqR)
	{
	// Could do a dynamic cast on iYadSeqR to see if it's really a ZYadSeqRPos,
	// in which case returning a ZValBase_YadSeqRPos::Iterator would be a win.
	return ZQL::Expr_Concrete(new ExprRep_Concrete(iYadSeqR));
	}

ZRef<ZQE::Iterator> sIterator(ZRef<ZQL::ExprRep_Relation> iExprRep)
	{ return Visitor_ExprRep_Concrete_MakeIterator().MakeIterator(iExprRep); }

} // namespace ZValBase_YadSeqR
NAMESPACE_ZOOLIB_END
