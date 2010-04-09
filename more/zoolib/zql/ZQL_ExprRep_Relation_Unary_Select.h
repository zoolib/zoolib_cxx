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

#ifndef __ZQL_ExprRep_Relation_Unary_Select__
#define __ZQL_ExprRep_Relation_Unary_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExprRep_Logic.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Relation_Unary_Select;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Select

class ExprRep_Relation_Unary_Select : public ExprRep_Relation_Unary
	{
public:
	ExprRep_Relation_Unary_Select(
		const ZRef<ExprRep_Relation>& iExprRep_Relation,
		const ZRef<ZExprRep_Logic>& iExprRep_Logic);

	virtual ~ExprRep_Relation_Unary_Select();

// From ExprRep_Relation via ExprRep_Relation_Unary
	virtual ZRelHead GetRelHead();

// From ExprRep_Relation_Unary
	virtual bool Accept_ExprRep_Relation_Unary(Visitor_ExprRep_Relation_Unary& iVisitor);

	virtual ZRef<ExprRep_Relation_Unary> Clone(ZRef<ExprRep_Relation> iExprRep_Relation);

// Our protocol
	virtual bool Accept_ExprRep_Relation_Unary_Select(
		Visitor_ExprRep_Relation_Unary_Select& iVisitor);

	ZRef<ZExprRep_Logic> GetExprRep_Logic();

private:
	const ZRef<ZExprRep_Logic> fExprRep_Logic;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Select

class Visitor_ExprRep_Relation_Unary_Select : public virtual Visitor_ExprRep_Relation_Unary
	{
public:
	virtual bool Visit_ExprRep_Relation_Unary_Select(ZRef<ExprRep_Relation_Unary_Select> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

ZRef<ExprRep_Relation_Unary_Select> sSelect(
	const ZRef<ZExprRep_Logic>& iExprRep_Logic, const ZRef<ExprRep_Relation>& iExprRep_Relation);

ZRef<ExprRep_Relation_Unary_Select> operator&(
	const ZRef<ZExprRep_Logic>& iExprRep_Logic, const ZRef<ExprRep_Relation>& iExprRep_Relation);

ZRef<ExprRep_Relation_Unary_Select> operator&(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZRef<ZExprRep_Logic>& iExprRep_Logic);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_ExprRep_Relation_Unary_Select__
