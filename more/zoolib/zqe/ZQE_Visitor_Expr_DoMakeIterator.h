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

#ifndef __ZQE_Visitor_Expr_DoMakeIterator__
#define __ZQE_Visitor_Expr_DoMakeIterator__ 1
#include "zconfig.h"

#include "zoolib/zqe/ZQE_Iterator.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Difference.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Intersect.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Join.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Union.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Rename.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_DoMakeIterator

class Visitor_Expr_DoMakeIterator
:	public virtual ZQL::Visitor_Expr_Rel_Binary_Difference
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Intersect
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Join
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Union
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Project
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Rename
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Select
	{
public:
	virtual void Visit_Expr_Rel_Binary_Difference(ZRef<ZQL::Expr_Rel_Binary_Difference> iRep);
	virtual void Visit_Expr_Rel_Binary_Intersect(ZRef<ZQL::Expr_Rel_Binary_Intersect> iRep);
	virtual void Visit_Expr_Rel_Binary_Join(ZRef<ZQL::Expr_Rel_Binary_Join> iRep);
	virtual void Visit_Expr_Rel_Binary_Union(ZRef<ZQL::Expr_Rel_Binary_Union> iRep);

	virtual void Visit_Expr_Rel_Unary_Project(ZRef<ZQL::Expr_Rel_Unary_Project> iRep);
	virtual void Visit_Expr_Rel_Unary_Rename(ZRef<ZQL::Expr_Rel_Unary_Rename> iRep);
	virtual void Visit_Expr_Rel_Unary_Select(ZRef<ZQL::Expr_Rel_Unary_Select> iRep);

// Our protocol
	ZRef<Iterator> DoMakeIterator(ZRef<ZExpr> iExpr);

protected:
	ZRef<Iterator> fIterator;
	};

} // namespace ZQE
NAMESPACE_ZOOLIB_END

#endif // __ZQE_Visitor_Expr_DoMakeIterator__
