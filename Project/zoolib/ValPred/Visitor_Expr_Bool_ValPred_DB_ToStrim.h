// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_ToStrim_h__
#define __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_ToStrim_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Visitor_Expr_Bool_ToStrim.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_ValPred_DB_ToStrim

class Visitor_Expr_Bool_ValPred_DB_ToStrim
:	public virtual Visitor_Expr_Bool_ToStrim
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iRep);
	};

} // namespace ZooLib

#endif // __ZooLib_ValPred_Visitor_Expr_Bool_ValPred_DB_ToStrim_h__
