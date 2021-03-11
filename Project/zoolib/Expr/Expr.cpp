// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Expr/Expr.h"

#include "zoolib/Stringf.h"
#include "zoolib/TypeIdName.h"

#include <cstring> // For strcmp

namespace ZooLib {

// =================================================================================================
#pragma mark - Expr

void Expr::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr* theVisitor = sDynNonConst<Visitor_Expr>(&iVisitor))
		this->Accept_Expr(*theVisitor);
	else
		Visitee::Accept(iVisitor);
	}

void Expr::Accept_Expr(Visitor_Expr& iVisitor)
	{ iVisitor.Visit_Expr(this); }

int Expr::Compare(const ZP<Expr>& iOther)
	{
	if (int compare = strcmp(sTypeIdName(*this), sTypeIdName(*iOther.Get())))
		return compare;
	return 0;
	}

std::string Expr::DebugDescription()
	{ return sStringf("%p/", this) + sTypeIdName(*this); }

// =================================================================================================
#pragma mark - Visitor_Expr

void Visitor_Expr::Visit_Expr(const ZP<Expr>& iExpr)
	{ this->Visit(iExpr); }

} // namespace ZooLib
