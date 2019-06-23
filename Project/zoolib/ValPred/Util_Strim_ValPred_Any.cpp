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

#include "zoolib/ValPred/Util_Strim_ValPred_Any.h"

#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ValPred/ValPred_Any.h"

namespace ZooLib {
namespace Util_Strim_ValPred_Any {

using std::set;
using std::string;

// =================================================================================================
#pragma mark - Helper functions (anonymous)

namespace { // anonymous

void spWrite_PropName(const string& iName, const ChanW_UTF& iChanW)
	{
	iChanW << "@";
	Util_Chan_JSON::sWrite_PropName(iName, iChanW);
	}

void spToStrim(const ZP<ValComparand>& iComparand, const ChanW_UTF& iChanW)
	{
	if (not iComparand)
		{
		iChanW << "/*Null Comparand*/";
		}
	else if (ZP<ValComparand_Name> cmprnd = iComparand.DynamicCast<ValComparand_Name>())
		{
		spWrite_PropName(cmprnd->GetName(), iChanW);
		}
	else if (ZP<ValComparand_Const_Any> cmprnd = iComparand.DynamicCast<ValComparand_Const_Any>())
		{
		Util_Any_JSON::sWrite(cmprnd->GetVal(), iChanW);
		}
	else
		{
		iChanW << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ZP<ValComparator>& iComparator, const ChanW_UTF& iChanW)
	{
	if (not iComparator)
		{
		iChanW << "/*Null Comparator*/";
		}
	else if (ZP<ValComparator_Simple> asSimple =
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
	else if (ZP<ValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ValComparator_StringContains>())
		{
		iChanW << " contains ";
		}
	else
		{
		iChanW << "/*Unknown Comparator*/";
		}
	}

ZP<ValComparand> spQRead_ValComparand(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;
	if (sTryRead_CP('@', iChanRU))
		{
		if (ZQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
			return new ValComparand_Name(*theQ);
		throw ParseException("Expected Name after @");
		}

	if (ZQ<Any> theQ = Util_Any_JSON::sQRead(iChanRU))
		return new ValComparand_Const_Any(*theQ);

	return null;
	}

ZP<ValComparator> spQRead_ValComparator(const ChanRU_UTF& iChanRU)
	{
	using Util_Chan::sTryRead_CP;

	if (NotQ<UTF32> theQ = sQRead(iChanRU))
		{
		return null;
		}
	else if (*theQ == '<')
		{
		if (sTryRead_CP('=', iChanRU))
			return new ValComparator_Simple(ValComparator_Simple::eLE);
		return new ValComparator_Simple(ValComparator_Simple::eLT);
		}
	else if (*theQ == '>')
		{
		if (sTryRead_CP('=', iChanRU))
			return new ValComparator_Simple(ValComparator_Simple::eGE);
		return new ValComparator_Simple(ValComparator_Simple::eGT);
		}
	else if (*theQ == '=' && sTryRead_CP('=', iChanRU))
		{
		return new ValComparator_Simple(ValComparator_Simple::eEQ);
		}
	else if (*theQ == '!' && sTryRead_CP('=', iChanRU))
		{
		return new ValComparator_Simple(ValComparator_Simple::eNE);
		}
	else
		{
		sUnread(iChanRU, *theQ);
		return null;
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Util_Strim_ValPred_Any

void sToStrim(const ValPred& iValPred, const ChanW_UTF& iChanW)
	{
	spToStrim(iValPred.GetLHS(), iChanW);
	spToStrim(iValPred.GetComparator(), iChanW);
	spToStrim(iValPred.GetRHS(), iChanW);
	}

ZQ<ValPred> sQFromStrim(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	if (NotP<ValComparand> theComparandL = spQRead_ValComparand(iChanRU))
		{
		return null;
		}
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (NotP<ValComparator> theComparator = spQRead_ValComparator(iChanRU))
			{
			throw ParseException("Expected Comparator after Comparand");
			}
		else
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (NotP<ValComparand> theComparandR = spQRead_ValComparand(iChanRU))
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
