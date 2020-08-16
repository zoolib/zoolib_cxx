// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Project_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Project_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Project;

// =================================================================================================
#pragma mark - Expr_Rel_Project

class Expr_Rel_Project
:	public virtual Expr_Rel
,	public virtual Expr_Op1_T<Expr_Rel>
	{
	typedef Expr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Project(const ZP<Expr_Rel>& iOp0, const RelHead& iRelHead);
	virtual ~Expr_Rel_Project();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone(const ZP<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Project(Visitor_Expr_Rel_Project& iVisitor);

	const RelHead& GetProjectRelHead() const;

private:
	const RelHead fRelHead;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Project

class Visitor_Expr_Rel_Project
:	public virtual Visitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Project> sProject(const ZP<Expr_Rel>& iExpr, const RelHead& iRelHead);

ZP<Expr_Rel> operator&(const ZP<Expr_Rel>& iExpr, const RelHead& iRelHead);
ZP<Expr_Rel> operator&(const RelHead& iRelHead, const ZP<Expr_Rel>& iExpr);
ZP<Expr_Rel>& operator&=(ZP<Expr_Rel>& ioExpr, const RelHead& iRelHead);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Project& iL,
	const RelationalAlgebra::Expr_Rel_Project& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Project_h__
