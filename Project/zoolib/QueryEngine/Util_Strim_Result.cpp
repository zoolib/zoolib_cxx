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

void sToStrim(const ZP<Result>& iResult, const ChanW_UTF& w)
	{
	const size_t theCount = iResult->Count();

	const RelHead& theRH = iResult->GetRelHead();

	w << "Count: " << theCount << ", RelHead: " << theRH;

	for (size_t yy = 0; yy < theCount; ++yy)
		{
		w << "\n";
		const Val_DB* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			{
			if (xx)
				w << ", ";
			if (theRow[xx].PGet<DataspaceTypes::AbsentOptional_t>())
				w << "!absent!";
			else
				Util_ZZ_JSON::sWrite(theRow[xx].As<Val_ZZ>(), false, w);
			}
		}
	}

} // namespace QueryEngine

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZP<QueryEngine::Result>& iResult)
	{
	QueryEngine::sToStrim(iResult, w);
	return w;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<QueryEngine::Result>& iResult)
	{
	StdIO::sChan_UTF_Err << iResult << "\n";
	}

#endif // defined(ZMACRO_pdesc)
