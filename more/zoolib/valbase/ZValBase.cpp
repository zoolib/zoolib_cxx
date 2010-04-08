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
#include "zoolib/zqe/ZQE_Visitor_ExprRep_DoMakeIterator.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Concrete.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Restrict.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Concrete

ExprRep_Relation_Concrete::ExprRep_Relation_Concrete()
	{}

ZRelHead ExprRep_Relation_Concrete::GetRelHead()
	{ return ZRelHead(true); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeIterator

namespace ZANONYMOUS {

class Visitor_DoMakeIterator
:	public virtual ZQL::Visitor_ExprRep_Relation_Concrete
,	public virtual ZQL::Visitor_ExprRep_Relation_Restrict
,	public virtual ZQL::Visitor_ExprRep_Relation_Select
,	public virtual ZQE::Visitor_ExprRep_DoMakeIterator
	{
public:
// From Visitor_ExprRep_Relation_Concrete
	virtual bool Visit_ExprRep_Relation_Concrete(ZRef<ZQL::ExprRep_Relation_Concrete> iRep);	

// From Visitor_ExprRep_Relation_Restrict
	virtual bool Visit_ExprRep_Relation_Restrict(ZRef<ZQL::ExprRep_Relation_Restrict> iRep);

// From Visitor_ExprRep_Relation_Select
	virtual bool Visit_ExprRep_Relation_Select(ZRef<ZQL::ExprRep_Relation_Select> iRep);
	};

bool Visitor_DoMakeIterator::Visit_ExprRep_Relation_Concrete(
	ZRef<ZQL::ExprRep_Relation_Concrete> iRep)
	{
	if (ZRef<ExprRep_Relation_Concrete> theRep = iRep.DynamicCast<ExprRep_Relation_Concrete>())
		{
		fIterator = theRep->MakeIterator();
		return true;
		}
	return Visitor_ExprRep_Relation_Concrete::Visit_ExprRep_Relation_Concrete(iRep);
	}

bool Visitor_DoMakeIterator::Visit_ExprRep_Relation_Restrict(
	ZRef<ZQL::ExprRep_Relation_Restrict> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->DoMakeIterator(iRep->GetExprRep()))
		fIterator = new ZQE::Iterator_Any_Restrict(iRep->GetValCondition(), theIterator);

	return true;	
	}

bool Visitor_DoMakeIterator::Visit_ExprRep_Relation_Select(ZRef<ZQL::ExprRep_Relation_Select> iRep)
	{
	if (ZRef<ZQE::Iterator> theIterator = this->DoMakeIterator(iRep->GetExprRep_Relation()))
		fIterator = new ZQE::Iterator_Any_Select(iRep->GetExprRep_Logic(), theIterator);

	return true;	
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Concrete_Dummy

class ExprRep_Relation_Concrete_Dummy : public ZQL::ExprRep_Relation_Concrete
	{
public:
	ExprRep_Relation_Concrete_Dummy();

// From ExprRep_Relation via ExprRep_Relation_Concrete
	virtual ZRelHead GetRelHead();
	};

ExprRep_Relation_Concrete_Dummy::ExprRep_Relation_Concrete_Dummy()
	{}

ZRelHead ExprRep_Relation_Concrete_Dummy::GetRelHead()
	{ return ZRelHead(true); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValBase pseudo constructors

ZRef<ZQL::ExprRep_Relation> sConcrete()
	{
	// Take a ZRelHead as a param?
	// Put this outside of ZValBase? Perhaps over in ZQL?
	// Need to return an Expr_Relation_Concrete that can be
	// handled by ZQL::Util_Strim_Query::Visitor_Query_DoToStrim
	return new ExprRep_Relation_Concrete_Dummy;
	}

ZRef<ZQE::Iterator> sIterator(ZRef<ZQL::ExprRep_Relation> iExprRep)
	{ return Visitor_DoMakeIterator().DoMakeIterator(iExprRep); }

} // namespace ZValBase_YadSeqR
NAMESPACE_ZOOLIB_END
