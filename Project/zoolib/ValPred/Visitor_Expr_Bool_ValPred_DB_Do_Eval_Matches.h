// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches_h__
#define __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches_h__ 1
#include "zconfig.h"

#include "zoolib/Val_DB.h"

#include "zoolib/Expr/Visitor_Expr_Bool_Do_Eval.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches

class Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches
:	public virtual Visitor_Expr_Bool_Do_Eval
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
	Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches(const Val_DB& iVal);

// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr);

private:
	const Val_DB& fVal;
	};

bool sMatches(const ZP<Expr_Bool>& iExpr, const Val_DB& iVal);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_Do_Eval_Matches_h__
