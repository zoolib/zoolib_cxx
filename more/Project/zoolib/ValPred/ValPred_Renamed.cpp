/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/Util_STL_map.h"
#include "zoolib/ZQ.h"

#include "zoolib/ValPred/ValPred_Renamed.h"

namespace ZooLib {

using std::map;
using std::string;

using namespace Util_STL;

ZRef<ValComparand> sRenamed(const map<string,string>& iRename,
	const ZRef<ValComparand>& iVal)
	{
	if (ZRef<ValComparand_Name> asName = iVal.DynamicCast<ValComparand_Name>())
		{
		if (ZQ<string> theQ = sQGet(iRename, asName->GetName()))
			return new ValComparand_Name(*theQ);
		}
	return iVal;
	}

ValPred sRenamed(const map<string,string>& iRename, const ValPred& iValPred)
	{
	return ValPred(
		sRenamed(iRename, iValPred.GetLHS()),
		iValPred.GetComparator(),
		sRenamed(iRename, iValPred.GetRHS()));
	}

} // namespace ZooLib
