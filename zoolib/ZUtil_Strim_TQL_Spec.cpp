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
#include "zoolib/ZStrimW_Escapify.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

NAMESPACE_ZOOLIB_BEGIN

using namespace ZTQL;

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
#pragma mark * ZUtil_Strim_TQL_Spec

void ZUtil_Strim_TQL_Spec::sToStrim(const ZStrimW& s, const Condition& iCondition)
	{
	sToStrim(s, iCondition.GetLHS().GetRep());
	sToStrim(s, iCondition.GetComparator().GetRep());
	sToStrim(s, iCondition.GetRHS().GetRep());
	}

void ZUtil_Strim_TQL_Spec::sToStrim(const ZStrimW& s, const ZTQL::Spec& iSpec)
	{
	sToStrim(s, iSpec.GetLogOp());
	}

void ZUtil_Strim_TQL_Spec::sToStrim(const ZStrimW& s, ZRef<ZTQL::LogOp> iLogOp)
	{
	if (!iLogOp)
		{
		s << "none";
		}
	else if (ZRef<LogOp_True> lo = ZRefDynamicCast<LogOp_True>(iLogOp))
		{
		s << "any";
		}
	else if (ZRef<LogOp_False> lo = ZRefDynamicCast<LogOp_False>(iLogOp))
		{
		s << "none";
		}
	else if (ZRef<LogOp_And> lo = ZRefDynamicCast<LogOp_And>(iLogOp))
		{
		s << "(";
		sToStrim(s, lo->GetLHS());
		s << " & ";
		sToStrim(s, lo->GetRHS());
		s << ")";
		}
	else if (ZRef<LogOp_Or> lo = ZRefDynamicCast<LogOp_Or>(iLogOp))
		{
		s << "(";
		sToStrim(s, lo->GetLHS());
		s << " | ";
		sToStrim(s, lo->GetRHS());
		s << ")";
		}
	else if (ZRef<LogOp_Condition> lo = ZRefDynamicCast<LogOp_Condition>(iLogOp))
		{
		ZUtil_Strim_TQL_Spec::sToStrim(s, lo->GetCondition());
		}	
	else
		{
		s << "!!Unknown LogOp!!";
		}
	}

NAMESPACE_ZOOLIB_END
