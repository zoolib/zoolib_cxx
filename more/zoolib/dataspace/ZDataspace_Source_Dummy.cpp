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

#include "zoolib/dataspace/ZDataspace_Source_Dummy.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

//#include "zoolib/ZExpr_Bool.h"

#include "zoolib/ZLog.h"

namespace ZooLib {
namespace ZDataspace {

using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dummy

Source_Dummy::Source_Dummy(const set<RelHead>& iRelHeads)
:	fRelHeads(iRelHeads)
	{}

Source_Dummy::~Source_Dummy()
	{}

set<RelHead> Source_Dummy::GetRelHeads()
	{ return fRelHeads; }

void Source_Dummy::Update(
	bool iLocalOnly,
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	ZRef<Event>& oEvent)
	{
	while (iAddedCount--)
		{
		SearchResult dummy(iAdded->GetRefcon(), null, null);
		oChanged.push_back(dummy);

#if 0
		if (ZLOGPF(s, eDebug))
			s << iAdded->fSearchSpec;
		vector<string8> theRowHead;
		for (vector<NameMap>::const_iterator i = iAdded->fSearchSpec.fNameMaps.begin();
			i != iAdded->fSearchSpec.fNameMaps.end(); ++i)
			{
			RelHead theRH = i->GetRelHead_To();
			theRowHead.insert(theRowHead.end(), theRH.begin(), theRH.end());
			}
#endif
		++iAdded;
		}
	oEvent = Event::sZero();
	}

} // namespace ZDataspace
} // namespace ZooLib
