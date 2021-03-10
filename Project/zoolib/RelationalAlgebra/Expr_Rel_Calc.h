// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Calc_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Calc_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Val_DB.h"

#include "zoolib/Expr/Expr_Op_T.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/PseudoMap.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Calc;

// =================================================================================================
#pragma mark - Expr_Rel_Calc

class Expr_Rel_Calc
:	public virtual Expr_Rel
,	public virtual Expr_Op1_T<Expr_Rel>
	{
	typedef Expr_Op1_T<Expr_Rel> inherited;
public:
	typedef RelationalAlgebra::PseudoMap PseudoMap;

	typedef Callable<Val_DB(const PseudoMap&)> Callable_t;

	Expr_Rel_Calc(const ZP<Expr_Rel>& iOp0,
		const ColName& iColName,
		const ZP<Callable_t>& iCallable);

	virtual ~Expr_Rel_Calc();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone(const ZP<Expr_Rel>& iOp0);

// From Expr_Rel
	virtual int Compare(const Expr_Rel& iOther);

// Our protocol
	virtual void Accept_Expr_Rel_Calc(Visitor_Expr_Rel_Calc& iVisitor);

	const ColName& GetColName() const;
	const ZP<Callable_t>& GetCallable() const;

private:
	const ColName fColName;
	const ZP<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Calc

class Visitor_Expr_Rel_Calc
:	public virtual Visitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sCalc(const ZP<Expr_Rel>& iOp0,
	const ColName& iColName,
	const ZP<Expr_Rel_Calc::Callable_t>& iCallable);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Calc_h__
