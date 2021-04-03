// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/Util_Strim_ValPred_DB.h"

#include "zoolib/Not.h"
#include "zoolib/ParseException.h"
#include "zoolib/Util_ZZ_JSON.h"
#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/ValPred/ValPred_DB.h"

namespace ZooLib {
namespace Util_Strim_ValPred_DB {

using std::set;
using std::string;

// =================================================================================================
#pragma mark - Helper functions (anonymous)

namespace { // anonymous

void spWrite_PropName(const ChanW_UTF& iChanW, const string& iName)
	{
	iChanW << "@";
	Util_Chan_JSON::sWrite_PropName(iChanW, iName);
	}

void spToStrim(const ChanW_UTF& iChanW, const ZP<ValComparand>& iComparand)
	{
	if (not iComparand)
		{
		iChanW << "/*Null Comparand*/";
		}
	else if (ZP<ValComparand_Name> cmprnd = iComparand.DynamicCast<ValComparand_Name>())
		{
		spWrite_PropName(iChanW, cmprnd->GetName());
		}
	else if (ZP<ValComparand_Const_DB> cmprnd = iComparand.DynamicCast<ValComparand_Const_DB>())
		{
		Util_ZZ_JSON::sWrite(iChanW, cmprnd->GetVal().As<Val_ZZ>());
		}
	else
		{
		iChanW << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ChanW_UTF& iChanW, const ZP<ValComparator>& iComparator)
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
	if (sTryRead_CP(iChanRU, '@'))
		{
		if (ZQ<string8> theQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
			return new ValComparand_Name(*theQ);
		throw ParseException("Expected Name after @");
		}

	if (ZQ<Val_ZZ> theQ = Util_ZZ_JSON::sQRead(iChanRU))
		return new ValComparand_Const_DB(theQ->As<Val_DB>());

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
		if (sTryRead_CP(iChanRU, '='))
			return new ValComparator_Simple(ValComparator_Simple::eLE);
		return new ValComparator_Simple(ValComparator_Simple::eLT);
		}
	else if (*theQ == '>')
		{
		if (sTryRead_CP(iChanRU, '='))
			return new ValComparator_Simple(ValComparator_Simple::eGE);
		return new ValComparator_Simple(ValComparator_Simple::eGT);
		}
	else if (*theQ == '=' && sTryRead_CP(iChanRU, '='))
		{
		return new ValComparator_Simple(ValComparator_Simple::eEQ);
		}
	else if (*theQ == '!' && sTryRead_CP(iChanRU, '='))
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
#pragma mark - Util_Strim_ValPred_DB

void sToStrim(const ChanW_UTF& iChanW, const ValPred& iValPred)
	{
	spToStrim(iChanW, iValPred.GetLHS());
	spToStrim(iChanW, iValPred.GetComparator());
	spToStrim(iChanW, iValPred.GetRHS());
	}

ZQ<ValPred> sQFromStrim(const ChanRU_UTF& iChanRU)
	{
	using namespace Util_Chan;

	ZP<ValComparand> theComparandL = spQRead_ValComparand(iChanRU);
	if (not theComparandL)
		return null;

	sSkip_WSAndCPlusPlusComments(iChanRU);

	ZP<ValComparator> theComparator = spQRead_ValComparator(iChanRU);
	if (not theComparator)
		throw ParseException("Expected Comparator after Comparand");

	sSkip_WSAndCPlusPlusComments(iChanRU);

	ZP<ValComparand> theComparandR = spQRead_ValComparand(iChanRU);
	if (not theComparandR)
		throw ParseException("Expected Comparand after Comparator");

	return ValPred(theComparandL, theComparator, theComparandR);
	}

} // namespace Util_Strim_ValPred_DB

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const ValPred& iValPred)
	{
	Util_Strim_ValPred_DB::sToStrim(iChanW, iValPred);
	return iChanW;
	}

} // namespace ZooLib
