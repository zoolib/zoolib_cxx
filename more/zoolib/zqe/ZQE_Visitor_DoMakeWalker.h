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

#ifndef __ZQE_Visitor_DoMakeWalker__
#define __ZQE_Visitor_DoMakeWalker__ 1
#include "zconfig.h"

#include "zoolib/ZVisitor_Do_T.h"

#include "zoolib/zqe/ZQE_Walker.h"

#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

class Visitor_DoMakeWalker
:	public virtual ZVisitor_Do_T<ZRef<Walker> >
,	public virtual ZRA::Visitor_Expr_Rel_Product
,	public virtual ZRA::Visitor_Expr_Rel_Embed
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Calc
,	public virtual ZRA::Visitor_Expr_Rel_Const
,	public virtual ZRA::Visitor_Expr_Rel_Dee
	{
public:
// From ZVisitor
	virtual void Visit(const ZRef<ZVisitee>& iRep);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr);

	virtual void Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<ZRA::Expr_Rel_Restrict>& iExpr);

	virtual void Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZRef<ZRA::Expr_Rel_Dee>& iExpr);
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Visitor_DoMakeWalker__
