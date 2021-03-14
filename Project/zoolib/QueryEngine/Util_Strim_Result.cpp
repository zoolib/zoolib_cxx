// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Util_Strim_Result.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_ZZ_JSON.h"

#include "zoolib/Dataspace/Types.h" // For AbsentOptional_t

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

static void spToStrim(const ChanW_UTF& ww, const ZP<Result>& iResult)
	{
	const size_t theCount = iResult->Count();

	const RelHead& theRH = iResult->GetRelHead();

	ww << "Count: " << theCount << ", RelHead: " << theRH;

	for (size_t yy = 0; yy < theCount; ++yy)
		{
		ww << "\n";
		const Val_DB* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			{
			if (xx)
				ww << ", ";

			// Forceably disable pretty print for the row -- too verbose.
			ThreadVal<PushTextOptions_JSON> tv_Options(ThreadVal<PushTextOptions_JSON>::sGet());
			tv_Options.Mut().fIndentStringQ.Clear();

			Util_ZZ_JSON::sWrite(ww, theRow[xx].As<Val_ZZ>());
			}
		}
	}

static bool spWriteFilter(
	const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_UTF& iChanW,
	const ZP<Callable_JSON_WriteFilter>& iPrior)
	{
	if (auto theP = sPGet<DataspaceTypes::AbsentOptional_t>(iPPT))
		{
		iChanW << "!absent!";
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

void sToStrim(const ChanW_UTF& ww, const ZP<Result>& iResult)
	{
	auto priorFilter = ThreadVal<ZP<Callable_JSON_WriteFilter>>::sGet();
	ThreadVal<ZP<Callable_JSON_WriteFilter>> tv_Filter(sBindR(sCallable(spWriteFilter), priorFilter));

	spToStrim(ww, iResult);
	}

} // namespace QueryEngine

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ZP<QueryEngine::Result>& iResult)
	{
	QueryEngine::sToStrim(ww, iResult);
	return ww;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<QueryEngine::Result>& iResult)
	{
	StdIO::sChanW_UTF_Err << iResult << "\n";
	}

#endif // defined(ZMACRO_pdesc)
