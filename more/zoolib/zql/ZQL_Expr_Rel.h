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

#ifndef __ZQL_Expr_Rel__
#define __ZQL_Expr_Rel__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

// Although Expr_Rel is anemic, we need it as the base for Rel_Binary, Rel_Concrete
// and Rel_Unary, so there is a common type against which relation operators can match.
// That might not be the case forever -- we might blur the distinction between Rel and Logic
// expressions, but for now it's useful for catching incompatible operations.

class Expr_Rel : public virtual ZExpr
	{
protected:
	Expr_Rel();

public:
	};

// =================================================================================================
#pragma mark -
#pragma mark * Rel

// A useful typedef.
typedef ZRef<Expr_Rel> Rel;

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel__
