// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Concrete_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Concrete_h__ 1
#include "zconfig.h"

#include "zoolib/UnicodeString.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Concrete;

// =================================================================================================
#pragma mark - Expr_Rel_Concrete

class Expr_Rel_Concrete
:	public virtual Expr_Rel
,	public virtual Expr_Op0_T<Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Concrete(const ConcreteHead& iConcrete);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Concrete(Visitor_Expr_Rel_Concrete& iVisitor);

	const ConcreteHead& GetConcreteHead() const;

private:
	ConcreteHead fConcreteHead;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Concrete

class Visitor_Expr_Rel_Concrete
:	public virtual Visitor_Expr_Op0_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sConcrete(const RelHead& iRelHead);
ZP<Expr_Rel> sConcrete(const RelHead& iRequired, const RelHead& iOptional);
ZP<Expr_Rel> sConcrete(const ConcreteHead& iConcreteHead);

} // namespace RelationalAlgebra

template <>
int sCompareNew_T(const RelationalAlgebra::Expr_Rel_Concrete& iL,
	const RelationalAlgebra::Expr_Rel_Concrete& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Concrete_h__
