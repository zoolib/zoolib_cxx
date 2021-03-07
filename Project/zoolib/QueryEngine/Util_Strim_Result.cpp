// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Util_Strim_Result.h"

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

// =================================================================================================
#pragma mark - sToStrim

void sToStrim(const ChanW_UTF& ww, const ZP<Result>& iResult)
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
			if (theRow[xx].PGet<DataspaceTypes::AbsentOptional_t>())
				ww << "!absent!";
			else
				Util_ZZ_JSON::sWrite(ww, theRow[xx].As<Val_ZZ>(), false);
			}
		}
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
