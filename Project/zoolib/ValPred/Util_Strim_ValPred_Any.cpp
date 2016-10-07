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

#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

#include "zoolib/ValPred/Util_Strim_ValPred_Any.h"
#include "zoolib/ValPred/ValPred_Any.h"

namespace ZooLib {
namespace Util_Strim_ValPred_Any {

using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark Helper functions (anonymous)

namespace { // anonymous

void spWrite_PropName(const string& iName, const ChanW_UTF& iChanW)
	{
	iChanW << "@";
	Yad_JSON::sWrite_PropName(iName, iChanW);
	}

void spToStrim(const ZRef<ValComparand>& iComparand, const ChanW_UTF& iChanW)
	{
	if (not iComparand)
		{
		iChanW << "/*Null Comparand*/";
		}
	else if (ZRef<ValComparand_Name> cr = iComparand.DynamicCast<ValComparand_Name>())
		{
		spWrite_PropName(cr->GetName(), iChanW);
		}
	else if (ZRef<ValComparand_Const_Any> cr = iComparand.DynamicCast<ValComparand_Const_Any>())
		{
		Yad_JSON::sToChan(sYadR(cr->GetVal()), iChanW);
		}
	else
		{
		iChanW << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ZRef<ValComparator>& iComparator, const ChanW_UTF& iChanW)
	{
	if (not iComparator)
		{
		iChanW << "/*Null Comparator*/";
		}
	else if (ZRef<ValComparator_Simple> asSimple =
		iComparator.DynamicCast<ValComparator_Simple>())
		{
		switch (asSimple->GetEComparator())
			{
			case ValComparator_Simple::eLT:
				{
				iChanW << " < ";
				break;
				}
			case ValComparator_Simple::eLE:
				{
				iChanW << " <= ";
				break;
				}
			case ValComparator_Simple::eEQ:
				{
				iChanW << " == ";
				break;
				}
			case ValComparator_Simple::eNE:
				{
				iChanW << " != ";
				break;
				}
			case ValComparator_Simple::eGE:
				{
				iChanW << " >= ";
				break;
				}
			case ValComparator_Simple::eGT:
				{
				iChanW << " > ";
				break;
				}
			default:
				{
				iChanW << "/*Unknown Simple Comparator*/";
				break;
				}
			}
		}
	else if (ZRef<ValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ValComparator_StringContains>())
		{
		iChanW << " contains ";
		}
	else
		{
		iChanW << "/*Unknown Comparator*/";
		}
	}

ZRef<ValComparand> spQRead_ValComparand(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;
	if (sTryRead_CP('@', iChanR, iChanU))
		{
		if (ZQ<string8> theQ = Yad_JSON::sQRead_PropName(iChanR, iChanU))
			return new ValComparand_Name(*theQ);
		throw ParseException("Expected Name after @");
		}

	if (ZQ<Val_Any> theQ = Util_Any_JSON::sQRead(sChanner_Chan(iChanR), sChanner_Chan(iChanU)))
		return new ValComparand_Const_Any(*theQ);

	return null;
	}

ZRef<ValComparator> spQRead_ValComparator(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using Util_Chan::sTryRead_CP;

	if (NotQ<UTF32> theQ = sQRead(iChanR))
		{
		return null;
		}
	else if (*theQ == '<')
		{
		if (sTryRead_CP('=', iChanR, iChanU))
			return new ValComparator_Simple(ValComparator_Simple::eLE);
		return new ValComparator_Simple(ValComparator_Simple::eLT);
		}
	else if (*theQ == '>')
		{
		if (sTryRead_CP('=', iChanR, iChanU))
			return new ValComparator_Simple(ValComparator_Simple::eGE);
		return new ValComparator_Simple(ValComparator_Simple::eGT);
		}
	else if (*theQ == '=' && sTryRead_CP('=', iChanR, iChanU))
		{
		return new ValComparator_Simple(ValComparator_Simple::eEQ);
		}
	else if (*theQ == '!' && sTryRead_CP('=', iChanR, iChanU))
		{
		return new ValComparator_Simple(ValComparator_Simple::eNE);
		}
	else
		{
		sUnread(*theQ, iChanU);
		return null;
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Util_Strim_ValPred_Any

void sToStrim(const ValPred& iValPred, const ChanW_UTF& iChanW)
	{
	spToStrim(iValPred.GetLHS(), iChanW);
	spToStrim(iValPred.GetComparator(), iChanW);
	spToStrim(iValPred.GetRHS(), iChanW);
	}

ZQ<ValPred> sQFromStrim(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	using namespace Util_Chan;

	if (ZRef<ValComparand,false> theComparandL = spQRead_ValComparand(iChanR, iChanU))
		{
		return null;
		}
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (ZRef<ValComparator,false> theComparator = spQRead_ValComparator(iChanR, iChanU))
			{
			throw ParseException("Expected Comparator after Comparand");
			}
		else
			{
			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
			if (ZRef<ValComparand,false> theComparandR = spQRead_ValComparand(iChanR, iChanU))
				{
				throw ParseException("Expected Comparand after Comparator");
				}
			else
				{
				return ValPred(theComparandL, theComparator, theComparandR);
				}
			}
		}
	}

} // namespace Util_Strim_ValPred_Any

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const ValPred& iValPred)
	{
	Util_Strim_ValPred_Any::sToStrim(iValPred, iChanW);
	return iChanW;
	}

} // namespace ZooLib
