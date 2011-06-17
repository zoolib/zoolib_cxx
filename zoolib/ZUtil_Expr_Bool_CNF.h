/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZUtil_Expr_Bool_CNF__
#define __ZUtil_Expr_Bool_CNF__
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZTag.h"

#include <set>

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark -
#pragma mark * Util_Expr_Bool

typedef std::set<ZTag<struct Disjunction_t, ZRef<ZExpr_Bool> > > Disjunction;
typedef std::set<Disjunction> CNF;

ZRef<ZExpr_Bool> sFromCNF(const CNF& iCNF);
CNF sAsCNF(const ZRef<ZExpr_Bool>& iExpr);

} // namespace Util_Expr_Bool
} // namespace ZooLib

#endif // __ZUtil_Expr_Bool_CNF__
