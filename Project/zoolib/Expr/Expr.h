// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Expr_Expr_h__
#define __ZooLib_Expr_Expr_h__ 1
#include "zconfig.h"

#include "zoolib/Visitor.h"

#include <string>

namespace ZooLib {

class Visitor_Expr;

// =================================================================================================
#pragma mark - Expr

class Expr : public Visitee
	{
public:
// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// Our protocol
	virtual void Accept_Expr(Visitor_Expr& iVisitor);

	virtual std::string DebugDescription();
	};

// =================================================================================================
#pragma mark - Visitor_Expr

class Visitor_Expr
:	public virtual Visitor
	{
public:
// Our protocol
	virtual void Visit_Expr(const ZP<Expr>& iExpr);
	};

} // namespace ZooLib

#endif // __ZooLib_Expr_Expr_h__
