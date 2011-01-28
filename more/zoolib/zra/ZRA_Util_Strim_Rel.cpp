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

#include "zoolib/ZUtil_Strim_ValPred_Any.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_ToStrim.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Dee.h"
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_GetRelHead.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZRA {
namespace Util_Strim_Rel {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ToStrim

namespace { // anonymous

class Visitor_ToStrim
:	public virtual ZVisitor_Expr_Bool_ValPred_Any_ToStrim
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Union
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
	{
public:
	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr);
	virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr);

	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr);

	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr);

private:
	void pWriteBinary(const string& iFunctionName, const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr);
	};

} // anonymous namespace

void Visitor_ToStrim::Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor_ToStrim::Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor_ToStrim::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{ this->pWriteBinary("Product", iExpr); }

void Visitor_ToStrim::Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor_ToStrim::Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Embed(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << ", " << iExpr->GetBindings() << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Project(" << iExpr->GetProjectRelHead() << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Rename(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetNew(), w);
	w << "<--";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetOld(), w);
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Restrict(";
	this->pToStrim(iExpr->GetExpr_Bool());
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Calc(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << " = /*Some function of*/ " << iExpr->GetBindings();
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{ pStrimW() << "Concrete(" << iExpr->GetConcreteRelHead() << ")"; }

void Visitor_ToStrim::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Const(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << ",";
	ZYad_ZooLibStrim::sToStrim(sMakeYadR(iExpr->GetVal()), w);
	w << ")";
	}

void Visitor_ToStrim::Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr)
	{ pStrimW() << "Dee()"; }

void Visitor_ToStrim::pWriteBinary(
	const string& iFunctionName, const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << iFunctionName << "(";

	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp1());
	w << ")";
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZRA_Util_Strim_Rel

void sToStrim(const ZRef<ZRA::Expr_Rel>& iRel, const ZStrimW& iStrimW)
	{
	Options theOptions;
	theOptions.fEOLString = "\n";
	theOptions.fIndentString = "|\t";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim(const ZRef<ZRA::Expr_Rel>& iRel, const Options& iOptions, const ZStrimW& iStrimW)
	{ Visitor_ToStrim().ToStrim(iOptions, iStrimW, iRel); }

} // namespace Util_Strim_Rel
} // namespace ZRA
} // namespace ZooLib
