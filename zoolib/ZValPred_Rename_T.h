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

#ifndef __ZValPred_Rename_T__
#define __ZValPred_Rename_T__ 1
#include "zconfig.h"

#include "zoolib/ZValPred_T.h"

#include <map>

namespace ZooLib {

template <class Val>
ZRef<ZValComparand_T<Val> > sRenamed(
	const std::map<std::string,std::string>& iRename,
	const ZRef<ZValComparand_T<Val> >& iVal)
	{
	// For some reason ZRef::DynamicCast isn't usable here.
	if (ZRef<ZValComparand_Name_T<Val> > as = dynamic_cast<ZValComparand_Name_T<Val>*>(iVal.Get()))
		return new ZValComparand_Name_T<Val>(iRename.find(as->GetName())->second);
	return iVal;
	}

template <class Val>
ZValPred_T<Val> sRenamed(
	const std::map<std::string,std::string>& iRename,
	const ZValPred_T<Val>& iValPred)
	{
	return ZValPred_T<Val>(
		sRenamed(iRename, iValPred.GetLHS()),
		iValPred.GetComparator(),
		sRenamed(iRename, iValPred.GetRHS()));
	}

} // namespace ZooLib

#endif // __ZValPred_Rename_T__
