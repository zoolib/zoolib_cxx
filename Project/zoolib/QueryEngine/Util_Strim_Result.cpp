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

#include "zoolib/QueryEngine/Util_Strim_Result.h"

#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

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
		const Val_Any* theRow = iResult->GetValsAt(yy);
		for (size_t xx = 0; xx < theRH.size(); ++xx)
			{
			if (xx)
				w << ", ";
			if (theRow[xx].PGet<DataspaceTypes::AbsentOptional_t>())
				w << "!absent!";
			else
				Util_Any_JSON::sWrite(theRow[xx], false, w);
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
