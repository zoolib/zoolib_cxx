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

#include "zoolib/ZUtil_Expr_Bool_CNF.h"
#include "zoolib/ZVisitor_Do_T.h"

using std::set;

namespace ZooLib {
namespace Util_Expr_Bool {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous

CNF spCrossMultiply(const CNF& iCNF0, const CNF& iCNF1)
	{
	CNF result;
	for (CNF::const_iterator iter0 = iCNF0.begin();
		iter0 != iCNF0.end(); ++iter0)
		{
		for (CNF::const_iterator iter1 = iCNF1.begin();
			iter1 != iCNF1.end(); ++iter1)
			{
			Disjunction theDis = *iter0;
			theDis.insert(iter1->begin(), iter1->end());
			result.insert(theDis);
			}
		}
	return result;
	}

CNF spTrue()
	{
	Disjunction theDis;
	theDis.insert(sTrue());
	CNF result;
	result.insert(theDis);
	return result;
	}

CNF spFalse()
	{
	Disjunction theDis;
	CNF result;
	result.insert(theDis);
	return result;
	}

bool spIsFalse(const CNF& iCNF)
	{
	if (iCNF.size() == 1 && iCNF.begin()->size() == 0)
		return true;
	return false;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_AsCNF (anonymous)

namespace { // anonymous

class Visitor_AsCNF
:	public virtual ZVisitor_Do_T<CNF>
,	public virtual ZVisitor_Expr_Bool_True
,	public virtual ZVisitor_Expr_Bool_False
,	public virtual ZVisitor_Expr_Bool_Not
,	public virtual ZVisitor_Expr_Bool_And
,	public virtual ZVisitor_Expr_Bool_Or
	{
public:
	Visitor_AsCNF()
	:	fNegating(false)
		{}

	virtual void Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<ZExpr_Bool> >& iExpr)
		{
		Disjunction theDis;
		if (fNegating)
			theDis.insert(sNot(iExpr->Self()));
		else
			theDis.insert(iExpr->Self());

		CNF result;
		result.insert(theDis);
		this->pSetResult(result);
		}

	virtual void Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>& iRep)
		{
		if (fNegating)
			this->pSetResult(spFalse());
		else
			this->pSetResult(spTrue());
		}

	virtual void Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>& iRep)
		{
		if (fNegating)
			this->pSetResult(spTrue());
		else
			this->pSetResult(spFalse());
		}

	virtual void Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>& iRep)
		{
		fNegating = !fNegating;
		ZVisitor_Expr_Bool_Not::Visit_Expr_Bool_Not(iRep);
		fNegating = !fNegating;
		}

	virtual void Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iRep)
		{
		const CNF theCNF0 = this->Do(iRep->GetOp0());
		CNF theCNF1 = this->Do(iRep->GetOp1());
		if (fNegating)
			{
			this->pSetResult(spCrossMultiply(theCNF0, theCNF1));
			}
		else
			{
			if (spIsFalse(theCNF0) || spIsFalse(theCNF1))
				{
				this->pSetResult(spFalse());
				}
			else
				{
				theCNF1.insert(theCNF0.begin(), theCNF0.end());
				this->pSetResult(theCNF1);
				}
			}
		}

	virtual void Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iRep)
		{
		const CNF theCNF0 = this->Do(iRep->GetOp0());
		CNF theCNF1 = this->Do(iRep->GetOp1());
		if (fNegating)
			{
			if (spIsFalse(theCNF0) || spIsFalse(theCNF1))
				{
				this->pSetResult(spFalse());
				}
			else
				{
				theCNF1.insert(theCNF0.begin(), theCNF0.end());
				this->pSetResult(theCNF1);
				}
			}
		else
			{
			this->pSetResult(spCrossMultiply(theCNF0, theCNF1));
			}		
		}

protected:
	bool fNegating;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Util_Expr_Bool

static ZRef<ZExpr_Bool> spFromDisjunction(
	const Disjunction& iDisjunction,
	const ZRef<ZExpr_Bool>& iTrue,
	const ZRef<ZExpr_Bool>& iFalse)
	{
	ZRef<ZExpr_Bool> result = iFalse;
	for (Disjunction::const_iterator iter = iDisjunction.begin();
		iter != iDisjunction.end(); ++iter)
		{
		ZRef<ZExpr_Bool> theExpr = iter->Get();
		if (theExpr == iTrue)
			{
			return iTrue;
			}
		else if (theExpr && theExpr != iFalse)
			{
			if (result != iFalse)
				result |= theExpr;
			else
				result = theExpr;
			}
		}
	return result;
	}

ZRef<ZExpr_Bool> sFromCNF(const CNF& iCNF)
	{
	ZRef<ZExpr_Bool> theTrue = sTrue();
	ZRef<ZExpr_Bool> theFalse = sFalse();
	ZRef<ZExpr_Bool> result = theTrue;
	for (CNF::const_iterator iter = iCNF.begin();
		iter != iCNF.end(); ++iter)
		{
		ZRef<ZExpr_Bool> theExpr = spFromDisjunction(*iter, theTrue, theFalse);
		if (!theExpr || theExpr == theFalse)
			return theFalse;
		else if (result == theTrue)
			result = theExpr;
		else
			result &= theExpr;
		}
	return result;
	}

CNF sAsCNF(const ZRef<ZExpr_Bool>& iExpr)
	{ return Visitor_AsCNF().Do(iExpr); }

} // namespace Util_Expr_Bool
} // namespace ZooLib
