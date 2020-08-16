// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Do_GetNames_h__
#define __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Do_GetNames_h__ 1
#include "zconfig.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/Visitor_Do_T.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_Do_GetNames

class Visitor_Expr_Bool_ValPred_Do_GetNames
:	public virtual Visitor_Do_T<std::set<std::string>>
,	public virtual Visitor_Expr_Bool_ValPred
,	public virtual Visitor_Expr_Op1_T<Expr_Bool>
,	public virtual Visitor_Expr_Op2_T<Expr_Bool>
	{
public:
// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr);

// From Visitor_Expr_Op1_T
	virtual void Visit_Expr_Op1(const ZP<Expr_Op1_T<Expr_Bool>>& iExpr);

// From Visitor_Expr_Op2_T
	virtual void Visit_Expr_Op2(const ZP<Expr_Op2_T<Expr_Bool>>& iExpr);
	};

std::set<std::string> sGetNames(const ZP<Expr_Bool>& iExpr);

} // namespace ZooLib

#endif // __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_Do_GetNames_h__
