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

#ifndef __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__
#define __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__ 1
#include "zconfig.h"

#include "zoolib/ZVisitor_Do_T.h"

#include "zoolib/QueryEngine/Walker.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
// MARK: - Visitor_DoMakeWalker

class Visitor_DoMakeWalker
:	public virtual ZVisitor_Do_T<ZRef<Walker> >
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Calc
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Const
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Dee
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Dum
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Embed
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Product
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Project
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Rename
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Restrict
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Union
	{
public:
// From ZVisitor
	virtual void Visit(const ZRef<ZVisitee>& iRep);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Calc(const ZRef<RelationalAlgebra::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<RelationalAlgebra::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZRef<RelationalAlgebra::Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Dum(const ZRef<RelationalAlgebra::Expr_Rel_Dum>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZRef<RelationalAlgebra::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<RelationalAlgebra::Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<RelationalAlgebra::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<RelationalAlgebra::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<RelationalAlgebra::Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZRef<RelationalAlgebra::Expr_Rel_Union>& iExpr);
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__
