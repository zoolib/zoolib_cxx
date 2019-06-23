/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_Expr_Util_Strim_Expr_Bool_h__
#define __ZooLib_Expr_Util_Strim_Expr_Bool_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZQ.h"

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
