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

#include "zoolib/zra/ZRA_Util_RelOperators.h"

namespace ZooLib {
namespace ZRA {

ZRef<Expr_Rel> operator*(const ZMap_Any& iMap, const ZRef<Expr_Rel>& iRel)
	{
	ZRef<Expr_Rel> result = iRel;
	for (ZMap_Any::Index_t i = iMap.Begin(); i != iMap.End(); ++i)
		result = sConst(result, iMap.NameOf(i), iMap.Get(i));
	return result;
	}

ZRef<Expr_Rel> operator*(const ZRef<Expr_Rel>& iRel, const ZMap_Any& iMap)
	{ return iMap * iRel; }

ZRef<Expr_Rel>& operator*=(ZRef<Expr_Rel>& ioRel, const ZMap_Any& iMap)
	{ return ioRel = ioRel * iMap; }

} // namespace ZRA
} // namespace ZooLib
