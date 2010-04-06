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

#ifndef __ZQL_Util_Strim_Query__
#define __ZQL_Util_Strim_Query__
#include "zconfig.h"

#include "zoolib/ZExpr.h"
#include "zoolib/ZVisitor_ExprRep_ToStrim.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace Util_Strim_Query {

// =================================================================================================
#pragma mark -
#pragma mark * ZQL_Util_Strim_Query

void sToStrim(const ZRef<ZExprRep>& iRep, const ZStrimW& iStrimW);

void sToStrim(const ZRef<ZExprRep>& iRep,
	const ZVisitor_ExprRep_ToStrim::Options& iOptions,
	const ZStrimW& iStrimW);

} // namespace Util_Strim_Query
} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Util_Strim_Query__
