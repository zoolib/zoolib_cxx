// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Expr/Visitor_Expr_Bool_ToStrim.h"

#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ToStrim

void Visitor_Expr_Bool_ToStrim::Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iRep)
	{ this->pStrimW() << "TRUE"; }

void Visitor_Expr_Bool_ToStrim::Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iRep)
	{ this->pStrimW() << "FALSE"; }

void Visitor_Expr_Bool_ToStrim::Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iRep)
	{
	this->pStrimW() << "~(";
	this->pToStrim(iRep->GetOp0());
	this->pStrimW() << ")";
	}

void Visitor_Expr_Bool_ToStrim::Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iRep)
	{
	this->pStrimW() << "(";
	this->pToStrim(iRep->GetOp0());
	this->pStrimW() << " & ";
	this->pToStrim(iRep->GetOp1());
	this->pStrimW() << ")";
	}

void Visitor_Expr_Bool_ToStrim::Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iRep)
	{
	this->pStrimW() << "(";
	this->pToStrim(iRep->GetOp0());
	this->pStrimW() << " | ";
	this->pToStrim(iRep->GetOp1());
	this->pStrimW() << ")";
	}

} // namespace ZooLib
