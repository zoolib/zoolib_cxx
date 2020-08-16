// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Expr/Visitor_Expr_Bool_Do_Eval.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Do_Eval

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iRep)
	{ this->pSetResult(true); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iRep)
	{ this->pSetResult(false); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iRep)
	{ this->pSetResult(not this->Do(iRep)); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) && this->Do(iRep->GetOp1())); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) || this->Do(iRep->GetOp1())); }

} // namespace ZooLib
