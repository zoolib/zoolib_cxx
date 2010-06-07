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

#include "zoolib/ZExpr_Logic.h"

#include "zoolib/ZStdIO.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"

namespace ZooLib {
namespace ZDataspace {

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
	AddedSearch* iAdded, size_t iAddedCount,
	int64* iRemoved, size_t iRemovedCount,
	vector<SearchResult>& oChanged,
	Clock& oClock)
	{
	while (iAddedCount--)
		{
		SearchResult dummy;
		dummy.fRefcon = iAdded->fRefcon;
		oChanged.push_back(dummy);

		ZRA::Util_Strim_Rel::sToStrim(iAdded->fRel, ZStdIO::strim_err);
		ZStdIO::strim_err << "\n";
		++iAdded;
		}
	}

} // namespace ZDataspace
} // namespace ZooLib
