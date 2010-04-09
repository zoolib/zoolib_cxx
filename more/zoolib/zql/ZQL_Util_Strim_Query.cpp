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

#include "zoolib/ZUtil_Strim_RelHead.h"
#include "zoolib/ZUtil_Strim_ValCondition.h"
#include "zoolib/ZVisitor_Expr_Logic_ValCondition_DoToStrim.h"
#include "zoolib/zql/ZQL_Util_Strim_Query.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Difference.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Intersect.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Join.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Union.h"
#include "zoolib/zql/ZQL_Expr_Rel_Concrete.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Rename.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Select.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace Util_Strim_Query {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

namespace ZANONYMOUS {

void spWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite_RelHead(const ZRelHead& iRelHead, const ZStrimW& iStrimW)
	{ ZUtil_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW); }

void spWrite_PropName(const string& iPropName, const ZStrimW& iStrimW)
	{ ZUtil_Strim_RelHead::sWrite_PropName(iPropName, iStrimW); }

void spWrite_EffectiveRelHeadComment(ZRef<Expr_Rel> iRep, const ZStrimW& iStrimW)
	{
	iStrimW.Write(" // ");
	ZUtil_Strim_RelHead::sWrite_RelHead(iRep->GetRelHead(), iStrimW);
	}

} // anonymous namespace


// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoToStrim

namespace ZANONYMOUS {

class Visitor_DoToStrim
:	public virtual ZVisitor_Expr_Logic_ValCondition_DoToStrim
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Difference
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Intersect
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Join
,	public virtual ZQL::Visitor_Expr_Rel_Binary_Union
,	public virtual ZQL::Visitor_Expr_Rel_Concrete
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Project
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Rename
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Restrict
,	public virtual ZQL::Visitor_Expr_Rel_Unary_Select
	{
public:
	virtual void Visit_Expr_Rel_Binary_Difference(
		ZRef<Expr_Rel_Binary_Difference> iRep);
	virtual void Visit_Expr_Rel_Binary_Intersect(
		ZRef<Expr_Rel_Binary_Intersect> iRep);
	virtual void Visit_Expr_Rel_Binary_Join(ZRef<Expr_Rel_Binary_Join> iRep);
	virtual void Visit_Expr_Rel_Binary_Union(ZRef<Expr_Rel_Binary_Union> iRep);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iRep);

	virtual void Visit_Expr_Rel_Unary_Project(ZRef<Expr_Rel_Unary_Project> iRep);
	virtual void Visit_Expr_Rel_Unary_Rename(ZRef<Expr_Rel_Unary_Rename> iRep);
	virtual void Visit_Expr_Rel_Unary_Restrict(ZRef<Expr_Rel_Unary_Restrict> iRep);
	virtual void Visit_Expr_Rel_Unary_Select(ZRef<Expr_Rel_Unary_Select> iRep);

private:
	void pWriteBinary(const std::string& iFunctionName, ZRef<Expr_Rel_Binary> iRep);
	};

} // anonymous namespace

void Visitor_DoToStrim::Visit_Expr_Rel_Binary_Difference(
	ZRef<Expr_Rel_Binary_Difference> iRep)
	{ this->pWriteBinary("Difference", iRep); }

void Visitor_DoToStrim::Visit_Expr_Rel_Binary_Intersect(
	ZRef<Expr_Rel_Binary_Intersect> iRep)
	{ this->pWriteBinary("Intersect", iRep); }

void Visitor_DoToStrim::Visit_Expr_Rel_Binary_Join(
	ZRef<Expr_Rel_Binary_Join> iRep)
	{
	const ZStrimW& w = pStrimW();
	w << "Join";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	w << "(";

	if (pOptions().fDebuggingOutput)
		{
		w << " // Joining on: ";
		const ZRelHead joinOn = iRep->GetLHS()->GetRelHead()
			& iRep->GetRHS()->GetRelHead();
		spWrite_RelHead(joinOn, w);
		}

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetLHS());
	w << ", ";

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetRHS());
	this->pWriteLFIndent();

	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Binary_Union(
	ZRef<Expr_Rel_Binary_Union> iRep)
	{ this->pWriteBinary("Union", iRep); }

void Visitor_DoToStrim::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iRep)
	{
	const ZStrimW& w = pStrimW();

	w << "/* Relation_Concrete: " << typeid(*iRep.Get()).name() << " */";

	if (pOptions().fDebuggingOutput)
		{
		this->pWriteLFIndent();
		spWrite_EffectiveRelHeadComment(iRep, w);
		}
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Unary_Project(
	ZRef<Expr_Rel_Unary_Project> iRep)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Project", w);

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	spWrite("(", w);

	this->pWriteLFIndent();
	spWrite_RelHead(iRep->GetProjectRelHead(), w);
	spWrite(",", w);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExpr_Rel());
	this->pWriteLFIndent();

	spWrite(")", w);
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Unary_Rename(
	ZRef<Expr_Rel_Unary_Rename> iRep)
	{
	const ZStrimW& w = pStrimW();
	spWrite("Rename", w);

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	spWrite("(", w);

	this->pWriteLFIndent();
	spWrite_PropName(iRep->GetNew(), w);
	spWrite("<--", w);
	spWrite_PropName(iRep->GetOld(), w);
	spWrite(",", w);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExpr_Rel());
	this->pWriteLFIndent();

	spWrite(")", w);
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Unary_Restrict(
	ZRef<Expr_Rel_Unary_Restrict> iRep)
	{
	const ZStrimW& w = pStrimW();
	w << "Restrict";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	w << "(";
	this->pWriteLFIndent();
	ZUtil_Strim_ValCondition::sToStrim(iRep->GetValCondition(), w);
	w << ",";

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExpr_Rel());
	this->pWriteLFIndent();

	w << ")";
	}

void Visitor_DoToStrim::Visit_Expr_Rel_Unary_Select(
	ZRef<Expr_Rel_Unary_Select> iRep)
	{
	const ZStrimW& w = pStrimW();
	w << "Select";

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	w << "(";
	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExpr_Logic());
	w << ",";

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExpr_Rel());
	this->pWriteLFIndent();

	w << ")";
	}


void Visitor_DoToStrim::pWriteBinary(
	const std::string& iFunctionName, ZRef<Expr_Rel_Binary> iRep)
	{
	const ZStrimW& w = pStrimW();
	w << iFunctionName;

	if (pOptions().fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, w);

	this->pWriteLFIndent();
	w << "(";

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetLHS());
	w << ", ";

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetRHS());
	this->pWriteLFIndent();

	w << ")";
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZRef<ZExpr>& iRep, const ZStrimW& iStrimW)
	{ sToStrim(iRep, ZVisitor_Expr_DoToStrim::Options(), iStrimW); }

void sToStrim(const ZRef<ZExpr>& iRep,
	const ZVisitor_Expr_DoToStrim::Options& iOptions,
	const ZStrimW& iStrimW)
	{ Visitor_DoToStrim().StartToStrim(iOptions, iStrimW, iRep); }

} // namespace Util_Strim_Query
} // namespace ZQL
NAMESPACE_ZOOLIB_END
