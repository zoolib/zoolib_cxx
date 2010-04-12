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

#ifndef __ZQE_Visitor_Expr_Rel_DoMakeIterator__
#define __ZQE_Visitor_Expr_Rel_DoMakeIterator__ 1
#include "zconfig.h"

#include "zoolib/ZVisitor_Expr_Do_T.h"
#include "zoolib/zqe/ZQE_Iterator.h"
#include "zoolib/zql/ZQL_Expr_Rel_Difference.h"
#include "zoolib/zql/ZQL_Expr_Rel_Intersect.h"
#include "zoolib/zql/ZQL_Expr_Rel_Join.h"
#include "zoolib/zql/ZQL_Expr_Rel_Union.h"
#include "zoolib/zql/ZQL_Expr_Rel_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Rename.h"
#include "zoolib/zql/ZQL_Expr_Rel_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_DoMakeIterator

class Visitor_Expr_Rel_DoMakeIterator
:	public virtual ZVisitor_Expr_Do_T<ZRef<Iterator> >
,	public virtual ZQL::Visitor_Expr_Rel_Difference
,	public virtual ZQL::Visitor_Expr_Rel_Intersect
,	public virtual ZQL::Visitor_Expr_Rel_Join
,	public virtual ZQL::Visitor_Expr_Rel_Union
,	public virtual ZQL::Visitor_Expr_Rel_Project
,	public virtual ZQL::Visitor_Expr_Rel_Rename
,	public virtual ZQL::Visitor_Expr_Rel_Select
	{
public:
	virtual void Visit_Expr_Rel_Difference(ZRef<ZQL::Expr_Rel_Difference> iExpr);
	virtual void Visit_Expr_Rel_Intersect(ZRef<ZQL::Expr_Rel_Intersect> iExpr);
	virtual void Visit_Expr_Rel_Join(ZRef<ZQL::Expr_Rel_Join> iExpr);
	virtual void Visit_Expr_Rel_Union(ZRef<ZQL::Expr_Rel_Union> iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<ZQL::Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<ZQL::Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<ZQL::Expr_Rel_Select> iExpr);
	};

} // namespace ZQE
NAMESPACE_ZOOLIB_END

#endif // __ZQE_Visitor_Expr_Rel_DoMakeIterator__
