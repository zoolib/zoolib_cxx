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

#ifndef __ZQL_Expr_Select__
#define __ZQL_Expr_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logical.h"
#include "zoolib/zql/ZQL_Expr_Relation.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_ExprRep_Select;

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Select

class ExprRep_Select : public ExprRep_Relation
	{
public:
	ExprRep_Select(
		const ZRef<ZExprRep_Logical>& iExprRep_Logical,
		const ZRef<ExprRep_Relation>& iExprRep_Relation);

	virtual ~ExprRep_Select();

// From ZExprRep_Relation
	virtual bool Accept(Visitor_ExprRep_Relation& iVisitor);

	virtual ZRelHead GetRelHead();

// Our protocol
	virtual bool Accept(Visitor_ExprRep_Select& iVisitor);

	ZRef<ZExprRep_Logical> GetExprRep_Logical();
	ZRef<ExprRep_Relation> GetExprRep_Relation();

private:
	const ZRef<ZExprRep_Logical> fExprRep_Logical;
	const ZRef<ExprRep_Relation> fExprRep_Relation;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Select

class Visitor_ExprRep_Select : public virtual Visitor_ExprRep_Relation
	{
public:
	virtual bool Visit_Select(ZRef<ExprRep_Select> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Select

class Expr_Select : public Expr_Relation
	{
	typedef Expr_Relation inherited;

	Expr_Select operator=(const Expr_Relation&);
	Expr_Select operator=(const ZRef<ExprRep_Relation>&);

public:
	Expr_Select();
	Expr_Select(const Expr_Select& iOther);
	~Expr_Select();
	Expr_Select& operator=(const Expr_Select& iOther);

	Expr_Select(const ZRef<ExprRep_Select>& iRep);

	operator ZRef<ExprRep_Select>() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Query operators

Expr_Select sSelect(const ZExpr_Logical& iExpr_Logical, const Expr_Relation& iExpr_Relation);

Expr_Select operator&(const ZExpr_Logical& iExpr_Logical, const Expr_Relation& iExpr_Relation);

Expr_Select operator&(const Expr_Relation& iExpr_Relation, const ZExpr_Logical& iExpr_Logical);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Select__
