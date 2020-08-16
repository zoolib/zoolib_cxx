// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Util_Rel_Operators_h__
#define __ZooLib_RelationalAlgebra_Util_Rel_Operators_h__ 1
#include "zconfig.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h" // For ValPred/Bool operators

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Intersect.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"

namespace ZooLib {
namespace RelationalAlgebra {

ZP<Expr_Rel> sConst(const NameVal& iNameVal);

ZP<Expr_Rel> operator*(const NameVal& iNameVal, const ZP<Expr_Rel>& iRel);
ZP<Expr_Rel> operator*(const ZP<Expr_Rel>& iRel, const NameVal& iNameVal);
ZP<Expr_Rel>& operator*=(ZP<Expr_Rel>& ioRel, const NameVal& iNameVal);

ZP<Expr_Rel> sConst(const Map_ZZ& iMap);

ZP<Expr_Rel> operator*(const Map_ZZ& iMap, const ZP<Expr_Rel>& iRel);
ZP<Expr_Rel> operator*(const ZP<Expr_Rel>& iRel, const Map_ZZ& iMap);
ZP<Expr_Rel>& operator*=(ZP<Expr_Rel>& ioRel, const Map_ZZ& iMap);

ZP<Expr_Rel> operator&(const ZP<Expr_Rel>& iExpr_Rel, const ValPred& iValPred);
ZP<Expr_Rel> operator&(const ValPred& iValPred, const ZP<Expr_Rel>& iExpr_Rel);
ZP<Expr_Rel>& operator&=(ZP<Expr_Rel>& ioExpr_Rel, const ValPred& iValPred);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Util_Rel_Operators_h__
