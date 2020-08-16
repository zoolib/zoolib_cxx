// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

#include "zoolib/Util_STL_set.h"

#include "zoolib/ValPred/ValPred_GetNames.h"

namespace ZooLib {

using namespace Util_STL;

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_Do_GetNames

void Visitor_Expr_Bool_ValPred_Do_GetNames::Visit_Expr_Bool_ValPred(
	const ZP<Expr_Bool_ValPred>& iExpr)
	{ this->pSetResult(sGetNames(iExpr->GetValPred())); }

void Visitor_Expr_Bool_ValPred_Do_GetNames::Visit_Expr_Op1(
	const ZP<Expr_Op1_T<Expr_Bool>>& iExpr)
	{ this->pSetResult(this->Do(iExpr->GetOp0())); }

void Visitor_Expr_Bool_ValPred_Do_GetNames::Visit_Expr_Op2(
	const ZP<Expr_Op2_T<Expr_Bool>>& iExpr)
	{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

std::set<std::string> sGetNames(const ZP<Expr_Bool>& iExpr)
	{ return Visitor_Expr_Bool_ValPred_Do_GetNames().Do(iExpr); }

} // namespace ZooLib
