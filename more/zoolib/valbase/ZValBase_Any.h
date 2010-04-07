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

#ifndef __ZValBase_Any__
#define __ZValBase_Any__ 1
#include "zconfig.h"

#include "zoolib/ZVal_Any.h"
#include "zoolib/zqe/ZQE_Visitor_ExprRep_MakeIterator.h"
#include "zoolib/zql/ZQL_Expr_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZValBase_Any {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Concrete

class ExprRep_Concrete : public ZQL::ExprRep_Concrete
	{
public:
	ExprRep_Concrete(const ZSeq_Any& iSeq);

// From ExprRep_Relation via ExprRep_Concrete
	virtual ZRelHead GetRelHead();

// Our protocol
	const ZSeq_Any& GetSeq();

private:
	const ZSeq_Any fSeq;
	};

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

} // namespace ZValBase_Any
NAMESPACE_ZOOLIB_END

#endif // __ZValBase_Any__
