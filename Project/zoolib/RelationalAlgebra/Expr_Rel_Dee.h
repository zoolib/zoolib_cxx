// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Dee_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Dee_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Dee;

// =================================================================================================
#pragma mark - Expr_Rel_Dee

// No columns, single row.

class Expr_Rel_Dee
:	public virtual Expr_Rel
,	public virtual Expr_Op0_T<Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Dee();

	virtual ~Expr_Rel_Dee();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr
	virtual int Compare(const ZP<Expr>& iOther);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Dee(Visitor_Expr_Rel_Dee& iVisitor);
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Dee

class Visitor_Expr_Rel_Dee
:	public virtual Visitor_Expr_Op0_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sDee();

} // namespace RelationalAlgebra

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Dee_h__
