// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Expr_Rel_Comment_h__
#define __ZooLib_RelationalAlgebra_Expr_Rel_Comment_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Op_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

class Visitor_Expr_Rel_Comment;

// =================================================================================================
#pragma mark - Expr_Rel_Comment

class Expr_Rel_Comment
:	public virtual Expr_Rel
,	public virtual Expr_Op1_T<Expr_Rel>
	{
	typedef Expr_Op1_T<Expr_Rel> inherited;
public:
	Expr_Rel_Comment(const ZP<Expr_Rel>& iOp0,
		const std::string& iComment,
		const ZP<Callable_Void>& iCallable);

	virtual ~Expr_Rel_Comment();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZP<Expr_Rel> Self();
	virtual ZP<Expr_Rel> Clone(const ZP<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Comment(Visitor_Expr_Rel_Comment& iVisitor);

	const std::string& GetComment() const;

	ZP<Callable_Void> GetCallable() const;

private:
	const std::string fComment;
	const ZP<Callable_Void> fCallable;
	};

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Comment

class Visitor_Expr_Rel_Comment
:	public virtual Visitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr);
	};

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Comment> sComment(const ZP<Expr_Rel>& iExpr_Rel, const std::string& iComment);

ZP<Expr_Rel_Comment> sComment(const ZP<Expr_Rel>& iExpr_Rel,
	const std::string& iComment,
	const ZP<Callable_Void>& iCallable);

} // namespace RelationalAlgebra

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Comment& iL,
	const RelationalAlgebra::Expr_Rel_Comment& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Expr_Rel_Comment_h__
