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

#ifndef __ZQE_Visitor_DoMakeWalker_Any__
#define __ZQE_Visitor_DoMakeWalker_Any__ 1
#include "zconfig.h"

#include "zoolib/ZVisitor_Do_T.h"

#include "zoolib/zqe/ZQE_Walker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker_Any

class Visitor_DoMakeWalker_Any
:	public virtual ZVisitor_Do_T<ZRef<Walker> >
,	public virtual ZRA::Visitor_Expr_Rel_Difference
,	public virtual ZRA::Visitor_Expr_Rel_Intersect
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Union
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Select
	{
public:
	virtual void Visit_Expr_Rel_Difference(ZRef<ZRA::Expr_Rel_Difference> iExpr);
	virtual void Visit_Expr_Rel_Intersect(ZRef<ZRA::Expr_Rel_Intersect> iExpr);
	virtual void Visit_Expr_Rel_Product(ZRef<ZRA::Expr_Rel_Product> iExpr);
	virtual void Visit_Expr_Rel_Union(ZRef<ZRA::Expr_Rel_Union> iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr);
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Visitor_DoMakeWalker_Any__
