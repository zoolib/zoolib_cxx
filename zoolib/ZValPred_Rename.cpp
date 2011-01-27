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

#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZValPred_Rename.h"

namespace ZooLib {

using std::map;
using std::set;
using std::string;

ZRef<ZValComparand> sRenamed(
	const map<string,string>& iRename, const ZRef<ZValComparand>& iVal)
	{
	if (ZRef<ZValComparand_Name> as = iVal.DynamicCast<ZValComparand_Name>())
		{
		if (ZQ<string> theQ = ZUtil_STL::sGetIfContains(iRename, as->GetName()))
			return new ZValComparand_Name(theQ.Get());
		}
	return iVal;
	}

ZValPred sRenamed(const map<string,string>& iRename, const ZValPred& iValPred)
	{
	return ZValPred(
		sRenamed(iRename, iValPred.GetLHS()),
		iValPred.GetComparator(),
		sRenamed(iRename, iValPred.GetRHS()));
	}

set<ZValPred> sRenamed(const map<string,string>& iRename,
	const set<ZValPred>& iValPreds)
	{
	set<ZValPred> result;
	for (set<ZValPred>::const_iterator i = iValPreds.begin(); i != iValPreds.end(); ++i)
		result.insert(sRenamed(iRename,*i));
	return result;
	}

} // namespace ZooLib
