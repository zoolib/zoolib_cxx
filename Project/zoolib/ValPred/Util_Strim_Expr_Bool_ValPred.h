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

#ifndef __ZooLib_Util_Strim_Expr_Bool_ValPred_h__
#define __ZooLib_Util_Strim_Expr_Bool_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanU_UTF.h"

#include "zoolib/Expr/Expr_Bool.h"

namespace ZooLib {
namespace Util_Strim_Expr_Bool_ValPred {

// =================================================================================================
#pragma mark - Util_Strim_Expr_Bool_ValPred

ZRef<Expr_Bool> sQFromStrim(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU);

} // namespace Util_Strim_Expr_Bool_ValPred
} // namespace ZooLib

#endif // __ZooLib_Util_Strim_Expr_Bool_ValPred_h__
