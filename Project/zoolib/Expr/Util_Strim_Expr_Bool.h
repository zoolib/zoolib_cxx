// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Expr_Util_Strim_Expr_Bool_h__
#define __ZooLib_Expr_Util_Strim_Expr_Bool_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Chan_UTF.h"

#include "zoolib/Expr/Expr_Bool.h"

namespace ZooLib {
namespace Util_Strim_Expr_Bool {

// =================================================================================================
#pragma mark - Util_Strim_Expr_Bool

typedef Callable<ZP<Expr_Bool>(const ChanRU_UTF& iChanRU)>
	Callable_Terminal;

ZP<Expr_Bool> sQFromStrim(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU);

} // namespace Util_Strim_Expr_Bool
} // namespace ZooLib

#endif // __ZooLib_Expr_Util_Strim_Expr_Bool_h__
