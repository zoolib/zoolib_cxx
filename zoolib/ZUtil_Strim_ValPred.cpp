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

#include "zoolib/ZUtil_Strim_ValPred.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {

namespace ZUtil_Strim_ValPred {

using std::string;

typedef ZValComparator_Simple_T<ZVal_Expr> ZValComparator_Simple;
typedef ZValComparand_Trail_T<ZVal_Expr> ZValComparand_Trail;
typedef ZValComparand_Var_T<ZVal_Expr> ZValComparand_Var;
typedef ZValComparand_Const_T<ZVal_Expr> ZValComparand_Const;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void spWrite_PropName(const string& iName, const ZStrimW& s)
	{
	s.Write("@");
	ZYad_ZooLibStrim::sWrite_PropName(iName, s);
	}

static void spToStrim(const ZRef<ZValPred::Comparand>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparand!!";
		}
	else if (ZRef<ZValComparand_Trail> cr = iCR.DynamicCast<ZValComparand_Trail>())
		{
		const ZTrail& theTrail = cr->GetTrail();
		if (theTrail.Count() == 1)
			spWrite_PropName(theTrail.At(0), s);
		else
			spWrite_PropName("/" + theTrail.AsString(), s);
		}
	else if (ZRef<ZValComparand_Var> cr = iCR.DynamicCast<ZValComparand_Var>())
		{
		s << "$";
		ZYad_ZooLibStrim::sWrite_PropName(cr->GetVarName(), s);
		}
	else if (ZRef<ZValComparand_Const> cr = iCR.DynamicCast<ZValComparand_Const>())
		{
		ZYad_ZooLibStrim::sToStrim(sMakeYadR(cr->GetVal()), s);
		}
	else
		{
		s << "!!Unknown Comparand!!";
		}
	}

static void spToStrim(const ZRef<ZValPred::Comparator>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparator!!";
		}
	else if (ZRef<ZValComparator_Simple> cr = iCR.DynamicCast<ZValComparator_Simple>())
		{
		switch (cr->GetEComparator())
			{
			case ZValComparator_Simple::eLT:
				{
				s << " < ";
				break;
				}
			case ZValComparator_Simple::eLE:
				{
				s << " <= ";
				break;
				}
			case ZValComparator_Simple::eEQ:
				{
				s << " == ";
				break;
				}
			case ZValComparator_Simple::eGE:
				{
				s << " >= ";
				break;
				}
			case ZValComparator_Simple::eGT:
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
#pragma mark * ZUtil_Strim_ValPred

void sToStrim(const ZValPred& iValPred, const ZStrimW& s)
	{
	spToStrim(iValPred.GetLHS(), s);
	spToStrim(iValPred.GetComparator(), s);
	spToStrim(iValPred.GetRHS(), s);
	}

} // namespace ZUtil_Strim_ValPred

} // namespace ZooLib
