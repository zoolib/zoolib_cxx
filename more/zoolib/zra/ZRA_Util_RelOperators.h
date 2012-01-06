/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZRA_Util_RelOperators_h__
#define __ZRA_Util_RelOperators_h__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool_ValPred.h" // For ValPred/Bool operators
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dum.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"

namespace ZooLib {
namespace ZRA {

ZRef<Expr_Rel> sConst(const ZMap_Any& iMap);

ZRef<Expr_Rel> operator*(const ZMap_Any& iMap, const ZRef<Expr_Rel>& iRel);
ZRef<Expr_Rel> operator*(const ZNameVal& iNameVal, const ZRef<Expr_Rel>& iRel);
ZRef<Expr_Rel> operator*(const ZRef<Expr_Rel>& iRel, const ZMap_Any& iMap);
ZRef<Expr_Rel>& operator*=(ZRef<Expr_Rel>& ioRel, const ZMap_Any& iMap);

ZRef<Expr_Rel> operator&(const ZRef<Expr_Rel>& iExpr_Rel, const ZValPred& iValPred);

ZRef<Expr_Rel> operator&(const ZValPred& iValPred, const ZRef<Expr_Rel>& iExpr_Rel);

ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr_Rel, const ZValPred& iValPred);

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Util_RelOperators_h__
