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

#ifndef __ZDataspace_Source__
#define __ZDataspace_Source__ 1
#include "zconfig.h"

#include "zoolib/ZValPredCompound.h"

#include "zoolib/zqe/ZQE_Result.h"

#include "zoolib/zra/ZRA_Expr_Rel.h"
#include "zoolib/zra/ZRA_RelRename.h"

#include "ZIntervalTreeClock.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

using namespace std;

using ZRA::RelHead;
using ZRA::RelRename;

typedef ZIntervalTreeClock::Clock Clock;

typedef string ReadableBy;

// =================================================================================================
#pragma mark -
#pragma mark * 

class SearchThing
	{
public:
//	RelHead fProject;
	vector<RelRename> fRelRenames;
	ZValPredCompound fPredCompound;
	};

SearchThing sAsSearchThing(ZRef<ZRA::Expr_Rel> iRel);

ZRef<ZRA::Expr_Rel> sAsRel(const RelHead& iRelHead);
ZRef<ZRA::Expr_Rel> sAsRel(const RelRename& iRelRenames);
ZRef<ZRA::Expr_Rel> sAsRel(const vector<RelRename>& iRelRenames);
ZRef<ZRA::Expr_Rel> sAsRel(const SearchThing& iSearchThing);

ZRef<ZRA::Expr_Rel> sAsRelFrom(const vector<RelRename>& iRelRenames);

// =================================================================================================
#pragma mark -
#pragma mark * 

class AddedSearch
	{
public:
	AddedSearch(int64 iRefcon, ZRef<ZRA::Expr_Rel> iRel);
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark -
#pragma mark * 

class SearchResult
	{
public:
	int64 fRefcon;
	vector<ZRef<ZQE::Result> > fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * 

class Source
	{
protected:
	Source();

public:
	virtual ~Source();

	virtual set<RelHead> GetRelHeads() = 0;

	virtual void Update(
		bool iLocalOnly,
		AddedSearch* iAdded, size_t iAddedCount,
		int64* iRemoved, size_t iRemovedCount,
		vector<SearchResult>& oChanged,
		Clock& oClock) = 0;	
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source__
