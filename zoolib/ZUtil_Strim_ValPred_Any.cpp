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

#include "zoolib/ZUtil_Strim_Operators.h"
#include "zoolib/ZUtil_Strim_ValPred_Any.h"
#include "zoolib/ZValPred_Any.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {
namespace ZUtil_Strim_ValPred_Any {

using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void spWrite_PropName(const string& iName, const ZStrimW& s)
	{
	s.Write("@");
	ZYad_ZooLibStrim::sWrite_PropName(iName, s);
	}

static void spToStrim(const ZRef<ZValComparand>& iComparand, const ZStrimW& s)
	{
	if (!iComparand)
		{
		s << "/*Null Comparand*/";
		}
	else if (ZRef<ZValComparand_Name> cr = iComparand.DynamicCast<ZValComparand_Name>())
		{
		spWrite_PropName(cr->GetName(), s);
		}
	else if (ZRef<ZValComparand_Const_Any> cr = iComparand.DynamicCast<ZValComparand_Const_Any>())
		{
		ZYad_ZooLibStrim::sToStrim(sMakeYadR(cr->GetVal()), s);
		}
	else
		{
		s << "/*Unknown Comparand*/";
		}
	}

static void spToStrim(const ZRef<ZValComparator>& iComparator, const ZStrimW& s)
	{
	if (!iComparator)
		{
		s << "/*Null Comparator*/";
		}
	else if (ZRef<ZValComparator_Simple> asSimple =
		iComparator.DynamicCast<ZValComparator_Simple>())
		{
		switch (asSimple->GetEComparator())
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
			case ZValComparator_Simple::eNE:
				{
				s << " != ";
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
			default:
				{
				s << "/*Unknown Simple Comparator*/";
				break;
				}
			}
		}
	else if (ZRef<ZValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ZValComparator_StringContains>())
		{
		s << " contains ";
		}
	else
		{
		s << "/*Unknown Comparator*/";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_ValPred_Any

void sToStrim(const ZValPred& iValPred, const ZStrimW& s)
	{
	spToStrim(iValPred.GetLHS(), s);
	spToStrim(iValPred.GetComparator(), s);
	spToStrim(iValPred.GetRHS(), s);
	}

} // namespace ZUtil_Strim_ValPred_Any

const ZStrimW& operator<<(const ZStrimW& w, const ZValPred& iValPred)
	{
	ZUtil_Strim_ValPred_Any::sToStrim(iValPred, w);
	return w;
	}

} // namespace ZooLib
