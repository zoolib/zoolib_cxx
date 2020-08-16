// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Util_Strim_Expr_Bool_ValPred_h__
#define __ZooLib_ValPred_Util_Strim_Expr_Bool_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"

#include "zoolib/Expr/Expr_Bool.h"

namespace ZooLib {
namespace Util_Strim_Expr_Bool_ValPred {

// =================================================================================================
#pragma mark - Util_Strim_Expr_Bool_ValPred

ZP<Expr_Bool> sQFromStrim(const ChanRU_UTF& iChanRU);

} // namespace Util_Strim_Expr_Bool_ValPred
} // namespace ZooLib

#endif // __ZooLib_ValPred_Util_Strim_Expr_Bool_ValPred_h__
