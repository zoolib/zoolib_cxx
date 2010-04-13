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

#ifndef __ZQL_Visitor_Expr_Rel_DoGetRelHead__
#define __ZQL_Visitor_Expr_Rel_DoGetRelHead__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/ZVisitor_Expr_Do_T.h"
#include "zoolib/zql/ZQL_Expr_Rel_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Rel_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Rename.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_DoGetRelHead

class Visitor_Expr_Rel_DoGetRelHead
:	public virtual ZVisitor_Expr_Do_T<RelHead>
,	public virtual ZVisitor_Expr_Op1_T<Expr_Rel>
,	public virtual ZVisitor_Expr_Op2_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
	{
public:
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<Expr_Rel> > iExpr);
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);
	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	};

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Visitor_Expr_Rel_DoGetRelHead__
