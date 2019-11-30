// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"

#include "zoolib/Callable_Function.h"

#include "zoolib/Expr/Util_Strim_Expr_Bool.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/Util_Strim_ValPred_DB.h"

namespace ZooLib {
namespace Util_Strim_Expr_Bool_ValPred {

// =================================================================================================
#pragma mark - Util_Strim_Expr_Bool_ValPred

static ZP<Expr_Bool> spQReadValPred(const ChanRU_UTF& iChanRU)
	{
	if (ZQ<ValPred> theQ = Util_Strim_ValPred_DB::sQFromStrim(iChanRU))
		return sExpr_Bool(*theQ);
	return null;
	}

ZP<Expr_Bool> sQFromStrim(const ChanRU_UTF& iChanRU)
	{
	return Util_Strim_Expr_Bool::sQFromStrim(sCallable(spQReadValPred), iChanRU);
	}

} // namespace Util_Strim_Expr_Bool_ValPred
} // namespace ZooLib
