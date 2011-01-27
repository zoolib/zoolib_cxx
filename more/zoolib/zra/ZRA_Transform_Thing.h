/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZRA_Transform_Thing__
#define __ZRA_Transform_Thing__
#include "zconfig.h"

#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::Transform_Thing

class Transform_Thing
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
	{
	typedef ZVisitor_Expr_Op_Do_Transform_T<Expr_Rel> inherited;
public:
	Transform_Thing();
	
// From ZVisitor_Expr_OpX_T via ZVisitor_Expr_Op_DoTransform_T
	virtual void Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<Expr_Rel> >& iExpr);
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr);

	virtual void Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr);

	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr);

// Our protocol
	ZRef<Expr_Rel> TopLevelDo(ZRef<Expr_Rel> iExpr);

	int fNextInt;
	Rename fRename_Concrete2Temp;
	Rename fRename_Temp2Concrete;
	RelHead fResultingRelHead;
	};

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_Transform_Thing__
