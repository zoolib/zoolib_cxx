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

#include "zoolib/tql/ZTQL_LogOp.h"
#include "zoolib/tql/ZUtil_Strim_TQL_Spec.h"

#include "zoolib/ZUtil_Strim_Tuple.h"
#include "zoolib/ZYad_ZooLibStrim.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL_Spec {

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void spToStrim(ZRef<ComparandRep> iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparand!!";
		}
	else if (ZRef<ComparandRep_Name> cr = ZRefDynamicCast<ComparandRep_Name>(iCR))
		{
		s << "@";
		ZYad_ZooLibStrim::sWrite_PropName(cr->GetName(), s);
		}
	else if (ZRef<ComparandRep_Value> cr = ZRefDynamicCast<ComparandRep_Value>(iCR))
		{
		s << cr->GetValue();
		}
	else
		{
		s << "!!Unknown Comparand!!";
		}
	}

static void spToStrim(ZRef<ComparatorRep> iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparator!!";
		}
	else if (ZRef<ComparatorRep_Simple> cr = ZRefDynamicCast<ComparatorRep_Simple>(iCR))
		{
		switch (cr->GetEComparator())
			{
			case ComparatorRep_Simple::eLT:
				{
				s << " < ";
				break;
				}
			case ComparatorRep_Simple::eLE:
				{
				s << " <= ";
				break;
				}
			case ComparatorRep_Simple::eEQ:
				{
				s << " == ";
				break;
				}
			case ComparatorRep_Simple::eGE:
				{
				s << " >= ";
				break;
				}
			case ComparatorRep_Simple::eGT:
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

class Writer : public LogOpVisitor
	{
public:
	Writer(const ZStrimW& iStrimW);

	virtual bool Visit_True(ZRef<LogOp_True> iLogOp);
	virtual bool Visit_False(ZRef<LogOp_False> iLogOp);
	virtual bool Visit_And(ZRef<LogOp_And> iLogOp);
	virtual bool Visit_Or(ZRef<LogOp_Or> iLogOp);
	virtual bool Visit_Condition(ZRef<LogOp_Condition> iLogOp);

private:
	const ZStrimW& fStrimW;
	};

} // anonymous namespace

Writer::Writer(const ZStrimW& iStrimW)
:	fStrimW(iStrimW)
	{}

bool Writer::Visit_True(ZRef<LogOp_True> iLogOp)
	{
	fStrimW << "any";
	return true;
	}

bool Writer::Visit_False(ZRef<LogOp_False> iLogOp)
	{
	fStrimW << "none";
	return true;
	}

bool Writer::Visit_And(ZRef<LogOp_And> iLogOp)
	{
	fStrimW << "(";
	sToStrim(iLogOp->GetLHS(), fStrimW);
	fStrimW << " & ";
	sToStrim(iLogOp->GetRHS(), fStrimW);
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Or(ZRef<LogOp_Or> iLogOp)
	{
	fStrimW << "(";
	sToStrim(iLogOp->GetLHS(), fStrimW);
	fStrimW << " | ";
	sToStrim(iLogOp->GetRHS(), fStrimW);
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Condition(ZRef<LogOp_Condition> iLogOp)
	{
	ZUtil_Strim_TQL_Spec::sToStrim(iLogOp->GetCondition(), fStrimW);
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL_Spec

void sToStrim(const Condition& iCondition, const ZStrimW& s)
	{
	spToStrim(iCondition.GetLHS().GetRep(), s);
	spToStrim(iCondition.GetComparator().GetRep(), s);
	spToStrim(iCondition.GetRHS().GetRep(), s);
	}

void sToStrim(const Spec& iSpec, const ZStrimW& s)
	{ sToStrim(iSpec.GetLogOp(), s); }

void sToStrim(ZRef<LogOp> iLogOp, const ZStrimW& s)
	{
	if (iLogOp)
		{
		Writer theWriter(s);
		iLogOp->Accept(theWriter);
		}
	else
		{
		s << "none";
		}
	}

} // namespace ZUtil_Strim_TQL_Spec

NAMESPACE_ZOOLIB_END
