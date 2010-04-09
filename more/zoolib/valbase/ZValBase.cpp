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

#include "zoolib/valbase/ZValBase.h"
#include "zoolib/zqe/ZQE_Iterator_Any.h"
#include "zoolib/zqe/ZQE_Result_Any.h"
#include "zoolib/zqe/ZQE_Visitor_Expr_DoMakeIterator.h"
#include "zoolib/zql/ZQL_Expr_Relation_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Relation_Unary_Restrict.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Concrete

Expr_Relation_Concrete::Expr_Relation_Concrete()
	{}

ZRelHead Expr_Relation_Concrete::GetRelHead()
	{ return ZRelHead(true); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator

namespace ZANONYMOUS {

class Visitor_DoMakeIterator
:	public virtual ZQE::Visitor_Expr_DoMakeIterator
,	public virtual ZQL::Visitor_Expr_Relation_Unary_Restrict
,	public virtual ZQL::Visitor_Expr_Relation_Concrete
	{
public:
// Via ZQE::Visitor_Expr_DoMakeIterator
	virtual void Visit_Expr_Relation_Unary_Select(
		ZRef<ZQL::Expr_Relation_Unary_Select> iRep);

// From ZQL::Visitor_Expr_Relation_Unary_Restrict
	virtual void Visit_Expr_Relation_Unary_Restrict(
		ZRef<ZQL::Expr_Relation_Unary_Restrict> iRep);

// From ZQL::Visitor_Expr_Relation_Concrete
	virtual void Visit_Expr_Relation_Concrete(ZRef<ZQL::Expr_Relation_Concrete> iRep);	
	};

void Visitor_DoMakeIterator::Visit_Expr_Relation_Unary_Select(
	ZRef<ZQL::Expr_Relation_Unary_Select> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->DoMakeIterator(iRep->GetExpr_Relation()))
		fIterator = new ZQE::Iterator_Any_Select(iRep->GetExpr_Logic(), theIterator);
	}

void Visitor_DoMakeIterator::Visit_Expr_Relation_Unary_Restrict(
	ZRef<ZQL::Expr_Relation_Unary_Restrict> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->DoMakeIterator(iRep->GetExpr_Relation()))
		fIterator = new ZQE::Iterator_Any_Restrict(iRep->GetValCondition(), theIterator);
	}

void Visitor_DoMakeIterator::Visit_Expr_Relation_Concrete(
	ZRef<ZQL::Expr_Relation_Concrete> iRep)
	{
	if (ZRef<Expr_Relation_Concrete> theRep = iRep.DynamicCast<Expr_Relation_Concrete>())
		fIterator = theRep->MakeIterator();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Concrete_Dummy

class Expr_Relation_Concrete_Dummy : public ZQL::Expr_Relation_Concrete
	{
public:
	Expr_Relation_Concrete_Dummy(const ZRelHead& iRelHead);

// From Expr_Relation via Expr_Relation_Concrete
	virtual ZRelHead GetRelHead();

private:
	ZRelHead fRelHead;
	};

Expr_Relation_Concrete_Dummy::Expr_Relation_Concrete_Dummy(const ZRelHead& iRelHead)
:	fRelHead(iRelHead)
	{}

ZRelHead Expr_Relation_Concrete_Dummy::GetRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase pseudo constructors

ZRef<ZQL::Expr_Relation> sConcrete()
	{
	// Put this outside of ZValBase? Perhaps over in ZQL?
	return sConcrete(ZRelHead(true));
	}

ZRef<ZQL::Expr_Relation> sConcrete(const ZRelHead& iRelHead)
	{ return new Expr_Relation_Concrete_Dummy(iRelHead); }

ZRef<ZQE::Iterator> sIterator(ZRef<ZQL::Expr_Relation> iExpr)
	{ return Visitor_DoMakeIterator().DoMakeIterator(iExpr); }

} // namespace ZValBase_YadSeqR
NAMESPACE_ZOOLIB_END
