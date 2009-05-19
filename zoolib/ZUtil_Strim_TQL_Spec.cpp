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

#include "zoolib/ZUtil_Strim_TQL_Spec.h"

#include "zoolib/ZTQL_LogOp.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL_Spec {

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void sToStrim(const ZStrimW& s, ZRef<ComparandRep> iCR)
	{
	if (!iCR)
		{
		s << "!!Null Comparand!!";
		}
	else if (ZRef<ComparandRep_Name> cr = ZRefDynamicCast<ComparandRep_Name>(iCR))
		{
		s << "@";
		ZUtil_Strim_Tuple::sWrite_PropName(s, cr->GetName());
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

static void sToStrim(const ZStrimW& s, ZRef<ComparatorRep> iCR)
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
	sToStrim(fStrimW, iLogOp->GetLHS());
	fStrimW << " & ";
	sToStrim(fStrimW, iLogOp->GetRHS());
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Or(ZRef<LogOp_Or> iLogOp)
	{
	fStrimW << "(";
	sToStrim(fStrimW, iLogOp->GetLHS());
	fStrimW << " | ";
	sToStrim(fStrimW, iLogOp->GetRHS());
	fStrimW << ")";
	return true;
	}

bool Writer::Visit_Condition(ZRef<LogOp_Condition> iLogOp)
	{
	ZUtil_Strim_TQL_Spec::sToStrim(fStrimW, iLogOp->GetCondition());
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL_Spec

void sToStrim(const ZStrimW& s, const Condition& iCondition)
	{
	sToStrim(s, iCondition.GetLHS().GetRep());
	sToStrim(s, iCondition.GetComparator().GetRep());
	sToStrim(s, iCondition.GetRHS().GetRep());
	}

void sToStrim(const ZStrimW& s, const Spec& iSpec)
	{
	sToStrim(s, iSpec.GetLogOp());
	}

void sToStrim(const ZStrimW& s, ZRef<LogOp> iLogOp)
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
