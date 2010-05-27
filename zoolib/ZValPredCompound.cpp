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

#include "zoolib/ZValPredCompound.h"
#include "zoolib/ZVisitor_Do_T.h"

namespace ZooLib {

typedef ZValPredCompound::Sect Sect;
typedef ZValPredCompound::SectUnion SectUnion;

// =================================================================================================
#pragma mark -
#pragma mark * spCrossProduct

static void spCrossProduct(const SectUnion& iSourceA,
	const SectUnion& iSourceB,
	SectUnion& oDest)
	{
	for (SectUnion::const_iterator iterSourceA = iSourceA.begin();
		iterSourceA != iSourceA.end(); ++iterSourceA)
		{
		for (SectUnion::const_iterator iterSourceB = iSourceB.begin();
			iterSourceB != iSourceB.end(); ++iterSourceB)
			{
			oDest.push_back(*iterSourceA);
			ZValPredCompound::Sect& temp = oDest.back();
			temp.reserve(temp.size() + (*iterSourceB).size());
			temp.insert(temp.end(), (*iterSourceB).begin(), (*iterSourceB).end());
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValPredCompound

/**\class ZValPredCompound
\brief Represents a compound ZValPred

Implemented as a union of intersections of ZValPred. Matches a target if at least one
of the intersections matches the target. If the union is empty, then no target can
be matched, and the ZValPredComound is our representation of a constant \c false. A
union containing an empty intersection is akin to a constant \c true.
*/

ZValPredCompound ZValPredCompound::sTrue()
	{
	SectUnion theVec(1);
	return ZValPredCompound(&theVec);
	}

ZValPredCompound ZValPredCompound::sFalse()
	{ return ZValPredCompound(); }

ZValPredCompound::ZValPredCompound()
	{}

ZValPredCompound::ZValPredCompound(const ZValPredCompound& iOther)
:	fSectUnion(iOther.fSectUnion)
	{}

ZValPredCompound::~ZValPredCompound()
	{}

ZValPredCompound& ZValPredCompound::operator=(const ZValPredCompound& iOther)
	{
	fSectUnion = iOther.fSectUnion;
	return *this;
	}

ZValPredCompound::ZValPredCompound(SectUnion* ioSectUnion)
	{ ioSectUnion->swap(fSectUnion); }

ZValPredCompound::ZValPredCompound(const ZValPred& iValPred)
:	fSectUnion(1, Sect(1, iValPred))
	{}

ZValPredCompound::ZValPredCompound(const Sect& iSect)
:	fSectUnion(1, iSect)
	{}

ZValPredCompound::ZValPredCompound(const SectUnion& iSectUnion)
:	fSectUnion(iSectUnion)
	{}

bool ZValPredCompound::IsAny() const
	{ return fSectUnion.size() == 1 && fSectUnion.front().empty(); }

bool ZValPredCompound::IsNone() const
	{ return fSectUnion.empty(); }

ZValPredCompound ZValPredCompound::operator&(const ZValPredCompound& iOther) const
	{
	if (this->IsNone() || iOther.IsAny())
		return *this;

	if (this->IsAny() || iOther.IsNone())
		return iOther;

	SectUnion crossProduct;
	spCrossProduct(fSectUnion, iOther.fSectUnion, crossProduct);
	// Here we use our special constructor that swaps its empty list
	// with the passed in list.
	return ZValPredCompound(&crossProduct);
	}

ZValPredCompound& ZValPredCompound::operator&=(const ZValPredCompound& iOther)
	{
	if (this->IsNone() || iOther.IsAny())
		return *this;

	if (this->IsAny() || iOther.IsNone())
		{
		fSectUnion = iOther.fSectUnion;
		return *this;
		}

	SectUnion sourceA;
	fSectUnion.swap(sourceA);
	// sourceA now holds what fSectUnion held, and fSectUnion is empty.
	spCrossProduct(sourceA, iOther.fSectUnion, fSectUnion);
	return *this;
	}

ZValPredCompound ZValPredCompound::operator|(const ZValPredCompound& iOther) const
	{
	if (this->IsAny() || iOther.IsNone())
		return *this;

	if (this->IsNone() || iOther.IsAny())
		return iOther;

	SectUnion theSectUnion = fSectUnion;
	theSectUnion.insert(theSectUnion.end(), iOther.fSectUnion.begin(), iOther.fSectUnion.end());
	return ZValPredCompound(&theSectUnion);
	}

ZValPredCompound& ZValPredCompound::operator|=(const ZValPredCompound& iOther)
	{
	if (this->IsAny() || iOther.IsNone())
		return *this;

	if (this->IsNone() || iOther.IsAny())
		{
		fSectUnion = iOther.fSectUnion;
		return *this;
		}

	fSectUnion.insert(fSectUnion.end(), iOther.fSectUnion.begin(), iOther.fSectUnion.end());
	return *this;
	}

bool ZValPredCompound::Matches(ZValContext& iContext, const ZVal_Any& iVal) const
	{
	for (SectUnion::const_iterator iterSectUnion = fSectUnion.begin();
		iterSectUnion != fSectUnion.end(); ++iterSectUnion)
		{
		bool allOkay = true;
		for (Sect::const_iterator iterSect = (*iterSectUnion).begin();
			allOkay && iterSect != (*iterSectUnion).end(); ++iterSect)
			{
			if (!(*iterSect).Matches(iContext, iVal))
				allOkay = false;
			}
		if (allOkay)
			return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * AsValPredCompound (anonymous)

namespace { // anonymous

class AsValPredCompound
:	public virtual ZVisitor_Do_T<ZValPredCompound>
,	public virtual ZVisitor_Expr_Logic_True
,	public virtual ZVisitor_Expr_Logic_False
,	public virtual ZVisitor_Expr_Logic_Not
,	public virtual ZVisitor_Expr_Logic_And
,	public virtual ZVisitor_Expr_Logic_Or
,	public virtual ZVisitor_Expr_Logic_ValPred
	{
public:
// From ZVisitor_Expr_Logic_XXX
	virtual void Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep);
	virtual void Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep);
	virtual void Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep);
	virtual void Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep);
	virtual void Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep);

	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr);
	};

void AsValPredCompound::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep)
	{ this->pSetResult(ZValPredCompound::sTrue()); }

void AsValPredCompound::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep)
	{ this->pSetResult(ZValPredCompound::sFalse()); }

void AsValPredCompound::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{
	ZUnimplemented();
	}

void AsValPredCompound::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) & this->Do(iRep->GetOp1())); }

void AsValPredCompound::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) | this->Do(iRep->GetOp1())); }

void AsValPredCompound::Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr)
	{ this->pSetResult(iExpr->GetValPred()); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * sAsValPredCompound

ZValPredCompound sAsValPredCompound(ZRef<ZExpr_Logic> iExpr)
	{ return AsValPredCompound().Do(iExpr); }

ZRef<ZExpr_Logic> sAsExpr_Logic(const ZValPredCompound& iVCF)
	{
	ZRef<ZExpr_Logic> result = sFalse();

	for (SectUnion::const_iterator outer = iVCF.fSectUnion.begin();
		outer != iVCF.fSectUnion.end(); ++outer)
		{
		ZRef<ZExpr_Logic> temp = sTrue();
		for (Sect::const_iterator inner = (*outer).begin();
			inner != (*outer).end(); ++inner)
			{
			temp = temp & (*inner);
			}
		result = result | temp;
		}
	return result;
	}

} // namespace ZooLib
