// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_Util_Expr_Bool_ValPred_Rename_h__
#define __ZooLib_ValPred_Util_Expr_Bool_ValPred_Rename_h__ 1
#include "zconfig.h"

#include "zoolib/Expr/Expr_Bool.h"

#include <map>
#include <string>

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark - Util_Expr_Bool

ZP<Expr_Bool> sRenamed(const std::map<std::string,std::string>& iRename,
	const ZP<Expr_Bool>& iExpr);

} // namespace Util_Expr_Bool
} // namespace ZooLib

#endif // __ZooLib_ValPred_Util_Expr_Bool_ValPred_Rename_h__
