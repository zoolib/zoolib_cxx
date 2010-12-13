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

ZRef<ZRA::Expr_Rel> operator*(const ZMap_Any& iMap, const ZRef<ZRA::Expr_Rel>& iRel)
	{ return sProduct(ZRA::sExplicit(iMap), iRel); }

ZRef<ZRA::Expr_Rel> operator*(const ZRef<ZRA::Expr_Rel>& iRel, const ZMap_Any& iMap)
	{ return sProduct(iRel, ZRA::sExplicit(iMap)); }

ZRef<Expr_Rel>& operator*=(ZRef<Expr_Rel>& ioRel, const ZMap_Any& iMap)
	{ return ioRel = ioRel * iMap; }

} // namespace ZRA
} // namespace ZooLib
