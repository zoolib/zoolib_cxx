// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Util_Strim_Result.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_ZZ_JSON.h"

#include "zoolib/Dataspace/Types.h" // For AbsentOptional_t
#include "zoolib/Dataspace/Util_Strim_Daton.h"

#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {
namespace QueryEngine {

using RelationalAlgebra::RelHead;
using Util_Chan_JSON::PushTextOptions_JSON;

// =================================================================================================
#pragma mark - sToStrim

static void spToStrim(const ChanW_UTF& iChanW, const ZP<Result>& iResult)
	{
	const size_t theIndent = ThreadVal_PushTextIndent::sGet()+1;
	ThreadVal_PushTextIndent tv_NewIndent(theIndent);
	const PushTextOptions_JSON& theOptions = ThreadVal<PushTextOptions_JSON>::sGet();

	const bool doIndentation = sDoIndentation(theOptions);

	const size_t theCount = iResult->Count();
	const RelHead& theRH = iResult->GetRelHead();

	iChanW << "⫷Count: " << theCount << ", RelHead: " << theRH;

	for (size_t yy = 0; yy < theCount; ++yy)
		{
		if (doIndentation && theCount > 1)
			Util_Chan_JSON::sWrite_LFIndent(iChanW, theIndent, theOptions);

		const Val_DB* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			{
			if (xx)
				iChanW << ", ";

			Util_ZZ_JSON::sWrite(iChanW, theRow[xx].As<Val_ZZ>());
			}
		}
	iChanW << "⫸";
	}

static bool spWriteFilter(
	const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_UTF& iChanW,
	const ZP<Callable_JSON_WriteFilter>& iPrior)
	{
	if (auto theP = sPGet<DataspaceTypes::AbsentOptional_t>(iPPT))
		{
		iChanW << "⨂";
		// iChanW << "!absent¡";
		return true;
		}

	if (auto theP = sPGet<Dataspace::Daton>(iPPT))
		{
		iChanW << *theP;
		return true;
		}

	if (auto theP = sPGet<ZP<Result>>(iPPT))
		{
		spToStrim(iChanW, *theP);
		return true;
		}

	if (iPrior)
		{
		if (sCall(iPrior, iPPT, iChanR, iChanW))
			return true;
		}

	return false;
	}

ZP<Callable_JSON_WriteFilter> sCallable_JSON_Write_Filter()
	{
	auto priorFilter = ThreadVal<ZP<Callable_JSON_WriteFilter>>::sGet();
	return sBindR(sCallable(spWriteFilter), priorFilter);
	}

void sToStrim(const ChanW_UTF& iChanW, const ZP<Result>& iResult)
	{
	ThreadVal<ZP<Callable_JSON_WriteFilter>> tv_Filter(sCallable_JSON_Write_Filter());

	spToStrim(iChanW, iResult);
	}

} // namespace QueryEngine

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const ZP<QueryEngine::Result>& iResult)
	{
	QueryEngine::sToStrim(iChanW, iResult);
	return iChanW;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<QueryEngine::Result>& iResult)
	{
	ThreadVal<Util_Chan_JSON::PushTextOptions_JSON> tv_Options(true);

	sToStrim(StdIO::sChanW_UTF_Err, iResult);
	StdIO::sChanW_UTF_Err << "\n";
	}

#endif // defined(ZMACRO_pdesc)
