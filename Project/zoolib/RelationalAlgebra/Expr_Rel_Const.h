// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__ 1
#include "zconfig.h"

#include "zoolib/Val_DB.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Const;

// =================================================================================================
#pragma mark - Expr_Rel_Const

class Expr_Rel_Const
:	public virtual Expr_Rel
,	public virtual Expr_Op0_T<Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Expr_Rel_Const(const ColName& iColName, const Val_DB& iVal);

	virtual ~Expr_Rel_Const();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Expr_Rel_Const(Visitor_Expr_Rel_Const& iVisitor);

	const ColName& GetColName() const;
	const Val_DB& GetVal() const;

private:
	const ColName fColName;
	const Val_DB fVal;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Const

class Visitor_Expr_Rel_Const
:	public virtual Visitor_Expr_Op0_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sConst(const ColName& iColName, const Val_DB& iVal);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Const& iL,
	const RelationalAlgebra::Expr_Rel_Const& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Const_h__
