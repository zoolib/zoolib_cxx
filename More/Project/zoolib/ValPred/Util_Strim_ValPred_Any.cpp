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

#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ValPred/Util_Strim_ValPred_Any.h"
#include "zoolib/ValPred/ValPred_Any.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

namespace ZooLib {
namespace Util_Strim_ValPred_Any {

using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark Helper functions (anonymous)

namespace { // anonymous

void spWrite_PropName(const string& iName, const ChanW_UTF& s)
	{
	s << "@";
	Yad_JSON::sWrite_PropName(iName, s);
	}

void spToStrim(const ZRef<ValComparand>& iComparand, const ChanW_UTF& s)
	{
	if (not iComparand)
		{
		s << "/*Null Comparand*/";
		}
	else if (ZRef<ValComparand_Name> cr = iComparand.DynamicCast<ValComparand_Name>())
		{
		spWrite_PropName(cr->GetName(), s);
		}
	else if (ZRef<ValComparand_Const_Any> cr = iComparand.DynamicCast<ValComparand_Const_Any>())
		{
		Yad_JSON::sToChan(sYadR(cr->GetVal()), s);
		}
	else
		{
		s << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ZRef<ValComparator>& iComparator, const ChanW_UTF& s)
	{
	if (not iComparator)
		{
		s << "/*Null Comparator*/";
		}
	else if (ZRef<ValComparator_Simple> asSimple =
		iComparator.DynamicCast<ValComparator_Simple>())
		{
		switch (asSimple->GetEComparator())
			{
			case ValComparator_Simple::eLT:
				{
				s << " < ";
				break;
				}
			case ValComparator_Simple::eLE:
				{
				s << " <= ";
				break;
				}
			case ValComparator_Simple::eEQ:
				{
				s << " == ";
				break;
				}
			case ValComparator_Simple::eNE:
				{
				s << " != ";
				break;
				}
			case ValComparator_Simple::eGE:
				{
				s << " >= ";
				break;
				}
			case ValComparator_Simple::eGT:
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
	else if (ZRef<ValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ValComparator_StringContains>())
		{
		s << " contains ";
		}
	else
		{
		s << "/*Unknown Comparator*/";
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Util_Strim_ValPred_Any

void sToStrim(const ValPred& iValPred, const ChanW_UTF& s)
	{
	spToStrim(iValPred.GetLHS(), s);
	spToStrim(iValPred.GetComparator(), s);
	spToStrim(iValPred.GetRHS(), s);
	}

} // namespace Util_Strim_ValPred_Any

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ValPred& iValPred)
	{
	Util_Strim_ValPred_Any::sToStrim(iValPred, w);
	return w;
	}

} // namespace ZooLib
