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
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_DoToStrim.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_GetRelHead.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZRA {
namespace Util_Strim_Rel {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

namespace { // anonymous

void spWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite_RelHead(const RelHead& iRelHead, const ZStrimW& iStrimW)
	{ Util_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW); }

void spWrite_EffectiveRelHeadComment(ZRef<Expr_Rel> iExpr, const ZStrimW& iStrimW)
	{
	iStrimW.Write(" /*");
	Util_Strim_RelHead::sWrite_RelHead(sGetRelHead(iExpr), iStrimW);
	iStrimW.Write("*/");
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoToStrim

namespace { // anonymous

class Visitor_DoToStrim
:	public virtual ZVisitor_Expr_Bool_ValPred_Any_DoToStrim
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Union
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict_Any
,	public virtual Visitor_Expr_Rel_Select
,	public virtual Visitor_Expr_Rel_Concrete
	{
public:
	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr);

	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>& iExpr);
	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr);

	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr);

private:
	void pWriteBinary(const string& iFunctionName, const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr);
	};

} // anonymous namespace

void Visitor_DoToStrim::Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Embed";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr->Self(), w);

//	this->pWriteLFIndent();
	w << "(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp1());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{ this->pWriteBinary("Product", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Calc", w);

//	if (pOptions().fDebuggingOutput)
//		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";
//	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << ",";

//	this->pWriteLFIndent();
	w << "/*SomeFunction*/";
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Const", w);

//	if (pOptions().fDebuggingOutput)
//		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";
//	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetRelName(), w);
	w << ",";

//	this->pWriteLFIndent();
	ZYad_ZooLibStrim::sToStrim(sMakeYadR(iExpr->GetVal()), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Project", w);

//	if (pOptions().fDebuggingOutput)
//		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";

//	this->pWriteLFIndent();
	spWrite_RelHead(iExpr->GetProjectRelHead(), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Rename", w);

//	if (pOptions().fDebuggingOutput)
//		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";

//	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetNew(), w);
	spWrite("<--", w);
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetOld(), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict_Any>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Restrict";

//	if (pOptions().fDebuggingOutput)
//		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";
//	this->pWriteLFIndent();
	ZUtil_Strim_ValPred_Any::sToStrim(iExpr->GetValPred(), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Select";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w << "(";
//	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetExpr_Bool());
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

//##	this->pWriteLFIndent();
	w << ")";
	}

#if 1
void Visitor_DoToStrim::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	const ZStrimW& w = pStrimW();

	w << "Concrete(";
	Util_Strim_RelHead::sWrite_RelHead(iExpr->GetConcreteRelHead(), w);
	w << ")";
	}
#else
void Visitor_DoToStrim::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	const ZStrimW& w = pStrimW();

	w << "Concrete";
	// We always include the relhead
	spWrite_EffectiveRelHeadComment(iExpr, w);

//	this->pWriteLFIndent();
	w
		<< "("
		<< iExpr->GetName()
		<< "/*" << iExpr->GetDescription() << "*/"
		<< "/*" << typeid(*iExpr.Get()).name() << "*/"
		<< ")";
	}
#endif
void Visitor_DoToStrim::pWriteBinary(
	const string& iFunctionName, const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << iFunctionName;

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr->Self(), w);

//	this->pWriteLFIndent();
	w << "(";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp1());

//##	this->pWriteLFIndent();
	w << ")";
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZRef<ZRA::Expr_Rel>& iRel, const ZStrimW& iStrimW)
	{
	Options theOptions;
	theOptions.fEOLString = "\n";
	theOptions.fIndentString = "|\t";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim(const ZRef<ZRA::Expr_Rel>& iRel, const Options& iOptions, const ZStrimW& iStrimW)
	{ Visitor_DoToStrim().DoToStrim(iOptions, iStrimW, iRel); }

} // namespace Util_Strim_Rel
} // namespace ZRA
} // namespace ZooLib
