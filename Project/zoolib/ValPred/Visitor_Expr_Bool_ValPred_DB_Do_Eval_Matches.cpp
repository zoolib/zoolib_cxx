// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches.h"

#include "zoolib/ValPred/ValPred_DB.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - *

Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches::
Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches(const Val_DB& iVal)
:	fVal(iVal)
	{}

void Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches::Visit_Expr_Bool_ValPred(
	const ZP<Expr_Bool_ValPred>& iExpr)
	{ this->pSetResult(sMatches(iExpr->GetValPred(), fVal)); }

bool sMatches(const ZP<Expr_Bool>& iExpr, const Val_DB& iVal)
	{ return Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches(iVal).Do(iExpr); }

} // namespace ZooLib
