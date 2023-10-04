// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Dum_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Dum_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Dum;

// =================================================================================================
#pragma mark - Expr_Rel_Dum

// No columns, no rows.

class Expr_Rel_Dum
:	public virtual Expr_Rel
,	public virtual Expr_Op0_T<Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Dum();

	virtual ~Expr_Rel_Dum();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr
	virtual int Compare(const ZP<Expr>& iOther);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Dum(Visitor_Expr_Rel_Dum& iVisitor);
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Dum

class Visitor_Expr_Rel_Dum
:	public virtual Visitor_Expr_Op0_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sDum();

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Dum_h__
