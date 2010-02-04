/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLib.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/tql/ZUtil_Strim_TQL.h"
#include "zoolib/tql/ZUtil_Strim_TQL_Spec.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL {

typedef ZVal_Any Val;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void spToStrim(const ZRef<ZValComparandRep<ZVal_Expr> >& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparand!!";
		}
	else if (ZRef<ZValComparandRep_Trail<ZVal_Expr> > cr = ZRefDynamicCast<ZValComparandRep_Trail<ZVal_Expr> >(iCR))
		{
		const ZTrail& theTrail = cr->GetTrail();
		if (theTrail.Count() == 1)
			ZUtil_Strim_TQL::sWrite_PropName(theTrail.At(0), s);
		else
			ZUtil_Strim_TQL::sWrite_PropName("/" + theTrail.AsString(), s);
		}
	else if (ZRef<ZValComparandRep_Var<ZVal_Expr> > cr = ZRefDynamicCast<ZValComparandRep_Var<ZVal_Expr> >(iCR))
		{
		s << "$";
		ZYad_ZooLibStrim::sWrite_PropName(cr->GetVarName(), s);
		}
	else if (ZRef<ZValComparandRep_Const<ZVal_Expr> > cr = ZRefDynamicCast<ZValComparandRep_Const<ZVal_Expr> >(iCR))
		{
		ZUtil_Strim_TQL::sWrite(cr->GetVal(), s);
		}
	else
		{
		s << "!!Unknown Comparand!!";
		}
	}

static void spToStrim(const ZRef<ZValComparatorRep<ZVal_Expr> >& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparator!!";
		}
	else if (ZRef<ZValComparatorRep_Simple<ZVal_Expr> > cr = ZRefDynamicCast<ZValComparatorRep_Simple<ZVal_Expr> >(iCR))
		{
		switch (cr->GetEComparator())
			{
			case ZValComparatorRep_Simple<ZVal_Expr>::eLT:
				{
				s << " < ";
				break;
				}
			case ZValComparatorRep_Simple<ZVal_Expr>::eLE:
				{
				s << " <= ";
				break;
				}
			case ZValComparatorRep_Simple<ZVal_Expr>::eEQ:
				{
				s << " == ";
				break;
				}
			case ZValComparatorRep_Simple<ZVal_Expr>::eGE:
				{
				s << " >= ";
				break;
				}
			case ZValComparatorRep_Simple<ZVal_Expr>::eGT:
				{
				s << " > ";
				break;
				}
			}
		}
	else
		{
		s << "!!Unknown Comparator!!";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Writer

namespace ZANONYMOUS {

class Writer : public ZVisitor_ExprRep_ValCondition_T<ZVal_Expr>
	{
public:
	Writer(const ZStrimW& iStrimW);

// From ZVisitor_ExprRep via ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit(ZRef<ZExprRep> iRep);


// From ZVisitor_ExprRep_Logical via ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit_Logical_True(ZRef<ZExprRep_Logical_True> iRep);
	virtual bool Visit_Logical_False(ZRef<ZExprRep_Logical_False> iRep);
	virtual bool Visit_Logical_Not(ZRef<ZExprRep_Logical_Not> iRep);
	virtual bool Visit_Logical_And(ZRef<ZExprRep_Logical_And> iRep);
	virtual bool Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep);

// From ZVisitor_ExprRep_ValCondition_T
	virtual bool Visit_ValCondition(ZRef<ZExprRep_ValCondition> iRep);

private:
	const ZStrimW& fStrimW;
	};

} // anonymous namespace

Writer::Writer(const ZStrimW& iStrimW)
:	fStrimW(iStrimW)
	{}

bool Writer::Visit(ZRef<ZExprRep> iRep)
	{
	fStrimW << "/*unknown LogOp*/";
	return true;
	}

bool Writer::Visit_Logical_True(ZRef<ZExprRep_Logical_True> iRep)
	{
	fStrimW << "any";
	return true;
	}

bool Writer::Visit_Logical_False(ZRef<ZExprRep_Logical_False> iRep)
	{
	fStrimW << "none";
	return true;
	}

bool Writer::Visit_Logical_Not(ZRef<ZExprRep_Logical_Not> iRep)
	{
	fStrimW << "!(";
	sToStrim(iRep->GetOperand(), fStrimW);
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Logical_And(ZRef<ZExprRep_Logical_And> iRep)
	{
	fStrimW << "(";
	sToStrim(iRep->GetLHS(), fStrimW);
	fStrimW << " & ";
	sToStrim(iRep->GetRHS(), fStrimW);
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Logical_Or(ZRef<ZExprRep_Logical_Or> iRep)
	{
	fStrimW << "(";
	sToStrim(iRep->GetLHS(), fStrimW);
	fStrimW << " | ";
	sToStrim(iRep->GetRHS(), fStrimW);
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_ValCondition(ZRef<ZExprRep_ValCondition> iRep)
	{
	ZUtil_Strim_TQL::sToStrim(iRep->GetValCondition(), fStrimW);
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZValCondition& iValCondition, const ZStrimW& s)
	{
	spToStrim(iValCondition.GetLHS(), s);
	spToStrim(iValCondition.GetComparator(), s);
	spToStrim(iValCondition.GetRHS(), s);
	}

void sToStrim(const ZRef<ZExprRep_Logical>& iRep, const ZStrimW& s)
	{
	if (iRep)
		{
		Writer theWriter(s);
		iRep->Accept(theWriter);
		}
	else
		{
		s << "none";
		}
	}

void sWrite(const ZVal_Any& iVal, const ZStrimW& s)
	{ ZYad_ZooLibStrim::sToStrim(sMakeYadR(iVal), s); }

} // namespace ZUtil_Strim_TQL

NAMESPACE_ZOOLIB_END
