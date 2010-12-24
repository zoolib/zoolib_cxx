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

#ifndef __ZRA_Compare_Rel__
#define __ZRA_Compare_Rel__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {

template <>
int sCompare_T(const ZRef<ZRA::Expr_Rel>& iLHS, const ZRef<ZRA::Expr_Rel>& iRHS);

namespace ZRA {

struct Comparator_Rel
	{
	bool operator()(const ZRef<ZRA::Expr_Rel>& iLeft, const ZRef<ZRA::Expr_Rel>& iRight) const
		{ return sCompare_T(iLeft, iRight) < 0; }

	typedef ZRef<ZRA::Expr_Rel> first_argument_type;
	typedef ZRef<ZRA::Expr_Rel> second_argument_type;
	typedef bool result_type;
	};

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Compare_Rel__
