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

#include "zoolib/ZExpr_Logic_ValPred.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Util.h"

namespace ZooLib {
namespace ZRA {
namespace Util {

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Util

Problem::Problem(ZRef<Expr_Rel> iRel, string iDescription)
:	fRel(iRel)
,	fDescription(iDescription)
	{}

ZRef<Expr_Rel> Problem::GetRel() const
	{ return fRel; }

string Problem::GetDescription() const
	{ return fDescription; }

// =================================================================================================
#pragma mark -
#pragma mark * Anonymous

namespace { // anonymous

class Doer
:	public virtual ZVisitor_Do_T<RelHead>
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Select
	{
public:
	Doer(vector<Problem>& oProblems);

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);

private:
	vector<Problem>& fProblems;
	};

Doer::Doer(vector<Problem>& oProblems)
:	fProblems(oProblems)
	{}

void Doer::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	const RelHead providedRelHead = this->Do(iExpr->GetOp0());
	const RelHead requiredRelHead = iExpr->GetProjectRelHead();

	if (!providedRelHead.Contains(requiredRelHead))
		{
		fProblems.push_back(
			Problem(iExpr, "Project references property(s) not provided by expr"));
		}

	this->pSetResult(providedRelHead & requiredRelHead);
	}

void Doer::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	const RelHead providedRelHead = this->Do(iExpr->GetOp0());
	const RelHead requiredRelHead = iExpr->GetValPred().GetNames();

	if (!providedRelHead.Contains(requiredRelHead))
		{
		fProblems.push_back(
			Problem(iExpr, "Restrict's ValPred requires property(s) not provided by expr"));
		}

	this->pSetResult(providedRelHead);
	}

void Doer::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	const RelHead providedRelHead = this->Do(iExpr->GetOp0());
	const RelHead requiredRelHead = sGetNames(iExpr->GetExpr_Logic());

	if (!providedRelHead.Contains(requiredRelHead))
		{
		fProblems.push_back(
			Problem(iExpr, "Select's Logic requires property(s) not provided by expr"));
		}

	this->pSetResult(providedRelHead);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Util

bool sValidate(vector<Problem>& oProblems, ZRef<Expr_Rel> iRel)
	{
	oProblems.clear();
	Doer(oProblems).Do(iRel);
	return oProblems.empty();
	}

} // namespace Util
} // namespace ZRA
} // namespace ZooLib
