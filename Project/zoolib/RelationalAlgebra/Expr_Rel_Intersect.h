// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Intersect_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Intersect_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Intersect;

// =================================================================================================
#pragma mark - Expr_Rel_Intersect

class Expr_Rel_Intersect
:	public virtual Expr_Rel
,	public virtual Expr_Op2_T<Expr_Rel>
	{
	typedef Expr_Op2_T<Expr_Rel> inherited;
public:
	Expr_Rel_Intersect(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1);

// From Expr
	virtual int Compare(const ZP<Expr>& iOther);

// From Expr_Op2_T
	virtual void Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1);

// Our protocol
	virtual void Accept_Expr_Rel_Intersect(Visitor_Expr_Rel_Intersect& iVisitor);
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Intersect

class Visitor_Expr_Rel_Intersect
:	public virtual Visitor_Expr_Op2_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Intersect> sIntersect(
	const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS);

ZP<Expr_Rel> operator&(
	const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_h__
