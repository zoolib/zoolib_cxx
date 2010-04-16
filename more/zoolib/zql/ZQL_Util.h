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

#ifndef __ZQL_Util__
#define __ZQL_Util__
#include "zconfig.h"

#include "zoolib/zql/ZQL_Expr_Rel.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace Util {

// =================================================================================================
#pragma mark -
#pragma mark * Util

class Problem : public ZCounted
	{
public:
	Problem(ZRef<Expr_Rel> iRel, std::string iDescription);

	ZRef<Expr_Rel> GetRel() const;
	std::string GetDescription() const;

private:
	ZRef<Expr_Rel> fRel;
	std::string fDescription;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Util

bool sValidate(std::vector<Problem>& oProblems, ZRef<Expr_Rel> iRel);

} // namespace Util
} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Util__
