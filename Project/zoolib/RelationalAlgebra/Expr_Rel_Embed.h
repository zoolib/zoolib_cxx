// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Embed_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Embed_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Embed;

// =================================================================================================
#pragma mark - Expr_Rel_Embed

class Expr_Rel_Embed
:	public virtual Expr_Rel
,	public virtual Expr_Op2_T<Expr_Rel>
	{
	typedef Expr_Op2_T<Expr_Rel> inherited;
public:
	Expr_Rel_Embed(const ZP<Expr_Rel>& iOp0, const RelHead& iBoundNames,
		const ColName& iColName, const ZP<Expr_Rel>& iEmbedee);

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr
	virtual int Compare(const ZP<Expr>& iOther);

// From Expr_Op2_T
	virtual void Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1);

// Our protocol
	virtual void Accept_Expr_Rel_Embed(Visitor_Expr_Rel_Embed& iVisitor);

	const RelHead& GetBoundNames() const;
	const ColName& GetColName() const;

private:
	const RelHead fBoundNames;
	const ColName fColName;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Embed

class Visitor_Expr_Rel_Embed
:	public virtual Visitor_Expr_Op2_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sEmbed(const ZP<Expr_Rel>& iOp0, const RelHead& iBoundNames,
	const ColName& iColName, const ZP<Expr_Rel>& iEmbedee);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_h__
