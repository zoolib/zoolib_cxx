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

#include "zoolib/ZUtil_Strim_ValCondition.h"
#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoToStrim.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Join.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"
#include "zoolib/zra/ZRA_Visitor_Expr_Rel_DoGetRelHead.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZRA {
namespace Util_Strim_Rel {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

namespace ZANONYMOUS {

RelHead spGetRelHead(ZRef<ZExpr> iExpr)
	{ return Visitor_Expr_Rel_DoGetRelHead().Do(iExpr); }

void spWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite_RelHead(const RelHead& iRelHead, const ZStrimW& iStrimW)
	{ Util_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW); }

void spWrite_PropName(const string& iPropName, const ZStrimW& iStrimW)
	{ Util_Strim_RelHead::sWrite_PropName(iPropName, iStrimW); }

void spWrite_EffectiveRelHeadComment(ZRef<ZExpr> iExpr, const ZStrimW& iStrimW)
	{
	iStrimW.Write(" // ");
	Util_Strim_RelHead::sWrite_RelHead(spGetRelHead(iExpr), iStrimW);
	}

} // anonymous namespace


// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoToStrim

namespace ZANONYMOUS {

class Visitor_DoToStrim
:	public virtual ZVisitor_Expr_Logic_ValCondition_DoToStrim
,	public virtual ZRA::Visitor_Expr_Rel_Difference
,	public virtual ZRA::Visitor_Expr_Rel_Intersect
,	public virtual ZRA::Visitor_Expr_Rel_Join
,	public virtual ZRA::Visitor_Expr_Rel_Union
,	public virtual ZRA::Visitor_Expr_Rel_Concrete
,	public virtual ZRA::Visitor_Expr_Rel_Project
,	public virtual ZRA::Visitor_Expr_Rel_Rename
,	public virtual ZRA::Visitor_Expr_Rel_Restrict
,	public virtual ZRA::Visitor_Expr_Rel_Select
	{
public:
	virtual void Visit_Expr_Rel_Difference(ZRef<Expr_Rel_Difference> iExpr);
	virtual void Visit_Expr_Rel_Intersect(ZRef<Expr_Rel_Intersect> iExpr);
	virtual void Visit_Expr_Rel_Join(ZRef<Expr_Rel_Join> iExpr);
	virtual void Visit_Expr_Rel_Union(ZRef<Expr_Rel_Union> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);

private:
	void pWriteBinary(const string& iFunctionName, ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr);
	};

} // anonymous namespace

void Visitor_DoToStrim::Visit_Expr_Rel_Difference(ZRef<Expr_Rel_Difference> iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Intersect(ZRef<Expr_Rel_Intersect> iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Join(ZRef<Expr_Rel_Join> iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Join";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	w << "(";

	if (pOptions().fDebuggingOutput)
		{
		w << " // Joining on: ";
		const RelHead joinOn = spGetRelHead(iExpr->GetOp0()) & spGetRelHead(iExpr->GetOp1());
		spWrite_RelHead(joinOn, w);
		}

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());
	w << ", ";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp1());

	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Union(ZRef<Expr_Rel_Union> iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor_DoToStrim::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{
	const ZStrimW& w = pStrimW();

	w << "Concrete";
	// We always include the relhead
	spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	w << "(" << iExpr->GetName() << "/*" << iExpr->GetDescription() << "*/";
	w << "/*" << typeid(*iExpr.Get()).name() << "*/ )";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Project", w);

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	spWrite("(", w);

	this->pWriteLFIndent();
	spWrite_RelHead(iExpr->GetRelHead(), w);
	spWrite(",", w);

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

	this->pWriteLFIndent();
	spWrite(")", w);
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Rename", w);

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	spWrite("(", w);

	this->pWriteLFIndent();
	spWrite_PropName(iExpr->GetNew(), w);
	spWrite("<--", w);
	spWrite_PropName(iExpr->GetOld(), w);
	spWrite(",", w);

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

	this->pWriteLFIndent();
	spWrite(")", w);
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Restrict";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	w << "(";
	this->pWriteLFIndent();
	ZUtil_Strim_ValCondition::sToStrim(iExpr->GetValCondition(), w);
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

	this->pWriteLFIndent();
	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << "Select";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	w << "(";
	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetExpr_Logic());
	w << ",";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());

	this->pWriteLFIndent();
	w << ")";
	}


void Visitor_DoToStrim::pWriteBinary(
	const string& iFunctionName, ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr)
	{
	const ZStrimW& w = pStrimW();
	w << iFunctionName;

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iExpr, w);

	this->pWriteLFIndent();
	w << "(";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp0());
	w << ", ";

	this->pWriteLFIndent();
	this->pDoToStrim(iExpr->GetOp1());

	this->pWriteLFIndent();
	w << ")";
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const Rel& iRel, const ZStrimW& iStrimW)
	{ sToStrim(iRel, Options(), iStrimW); }

void sToStrim(const Rel& iRel, const Options& iOptions, const ZStrimW& iStrimW)
	{ Visitor_DoToStrim().DoToStrim(iOptions, iStrimW, iRel); }

} // namespace Util_Strim_Rel
} // namespace ZRA
NAMESPACE_ZOOLIB_END
