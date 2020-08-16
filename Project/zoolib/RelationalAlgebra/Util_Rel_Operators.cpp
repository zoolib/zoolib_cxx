// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Util_Rel_Operators.h"

namespace ZooLib {
namespace RelationalAlgebra {

ZP<Expr_Rel> sConst(const NameVal& iNameVal)
	{ return sConst(iNameVal.first, iNameVal.second.As<Val_DB>()); }

ZP<Expr_Rel> operator*(const NameVal& iNameVal, const ZP<Expr_Rel>& iRel)
	{ return sConst(iNameVal) * iRel; }

ZP<Expr_Rel> operator*(const ZP<Expr_Rel>& iRel, const NameVal& iNameVal)
	{ return iRel * sConst(iNameVal); }

ZP<Expr_Rel>& operator*=(ZP<Expr_Rel>& ioRel, const NameVal& iNameVal)
	{ return ioRel *= sConst(iNameVal); }

// -----

static ZP<Expr_Rel> spConst(const Map_ZZ& iMap)
	{
	ZP<Expr_Rel> result;
	for (Map_ZZ::Index_t i = iMap.Begin(); i != iMap.End(); ++i)
		{
		ZP<Expr_Rel> cur = sConst(iMap.NameOf(i), iMap.Get(i).As<Val_DB>());
		if (not result)
			result = cur;
		else
			result = result * cur;
		}
	return result;
	}

ZP<Expr_Rel> sConst(const Map_ZZ& iMap)
	{
	if (ZP<Expr_Rel> result = spConst(iMap))
		return result;

	return sDee();
	}

ZP<Expr_Rel> operator*(const Map_ZZ& iMap, const ZP<Expr_Rel>& iRel)
	{
	if (ZP<Expr_Rel> asRel = spConst(iMap))
		return asRel * iRel;
	return iRel;
	}

ZP<Expr_Rel> operator*(const ZP<Expr_Rel>& iRel, const Map_ZZ& iMap)
	{
	if (ZP<Expr_Rel> asRel = spConst(iMap))
		return iRel * asRel;
	return iRel;
	}

ZP<Expr_Rel>& operator*=(ZP<Expr_Rel>& ioRel, const Map_ZZ& iMap)
	{
	if (ZP<Expr_Rel> asRel = spConst(iMap))
		ioRel = ioRel * asRel;
	return ioRel;
	}

// -----

ZP<Expr_Rel> operator&(const ZP<Expr_Rel>& iExpr_Rel, const ValPred& iValPred)
	{ return new Expr_Rel_Restrict(iExpr_Rel, new Expr_Bool_ValPred(iValPred)); }

ZP<Expr_Rel> operator&(const ValPred& iValPred, const ZP<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Restrict(iExpr_Rel, new Expr_Bool_ValPred(iValPred)); }

ZP<Expr_Rel>& operator&=(ZP<Expr_Rel>& ioExpr_Rel, const ValPred& iValPred)
	{ return ioExpr_Rel = ioExpr_Rel & iValPred; }

} // namespace RelationalAlgebra
} // namespace ZooLib
