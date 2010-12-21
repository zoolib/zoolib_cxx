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

// >>> Debugging
#include "zoolib/ZLog.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"
// <<< Debugging

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"

#include "zoolib/dataspace/ZDataspace_Source_Union.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union

Source_Union::Source_Union()
:	fNextRefcon(1)
,	fEvent(Event::sZero())
,	fCallable(MakeCallable(MakeWeakRef(this), &Source_Union::pCallback))
	{
	}

Source_Union::~Source_Union()
	{
	}

set<RelHead> Source_Union::GetRelHeads()
	{
	}

void Source_Union::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	}

void Source_Union::CollectResults(std::vector<SearchResult>& oChanged)
	{
	}

void Source_Union::InsertSource(ZRef<Source> iSource, const string8& iPrefix)
	{
	}

void Source_Union::EraseSource(ZRef<Source> iSource)
	{
	}

void Source_Union::pCallback(ZRef<Source> iSource)
	{}

} // namespace ZDataspace
} // namespace ZooLib
