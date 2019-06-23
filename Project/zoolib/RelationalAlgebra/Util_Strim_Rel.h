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

#ifndef __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
#define __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Any_ToStrim.h"

#include "zoolib/QueryEngine/Expr_Rel_Search.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Difference.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Intersect.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_Rel {

// =================================================================================================
#pragma mark - RelationalAlgebra::Util_Strim_Rel::Visitor

class Visitor
:	public virtual Visitor_Expr_Bool_ValPred_Any_ToStrim
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Comment
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Dum
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Union
,	public virtual QueryEngine::Visitor_Expr_Rel_Search
	{
public:
	virtual void Visit_Expr(const ZP<Expr>& iExpr);
	virtual void Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr);
	virtual void Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr);

	virtual void Visit_Expr_Rel_Search(const ZP<QueryEngine::Expr_Rel_Search>& iExpr);

private:
	void pWriteBinary(const std::string& iFunctionName, const ZP<Expr_Op2_T<Expr_Rel>>& iExpr);
	};

typedef Visitor::Options Options;

void sToStrim(const ZP<Expr_Rel>& iRel, const ChanW_UTF& iStrimW);

void sToStrim_Parseable(const ZP<Expr_Rel>& iRel, const ChanW_UTF& iStrimW);

void sToStrim(const ZP<Expr_Rel>& iRel, const Options& iOptions, const ChanW_UTF& iStrimW);

ZP<Expr_Rel> sFromStrim(const ChanRU_UTF& iChanRU);

ZP<Expr_Rel> sQFromStrim(const ChanRU_UTF& iChanRU);

} // namespace Util_Strim_Rel
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZP<RelationalAlgebra::Expr_Rel>& iRel);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
