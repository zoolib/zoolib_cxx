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

#include "zoolib/ZCallable.h"
#include "zoolib/ZIntervalTreeClock.h"

#include "zoolib/zqe/ZQE_Result.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"
#include "zoolib/zra/ZRA_RelHead.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

using ZIntervalTreeClock::Clock;
using ZIntervalTreeClock::Event;

using ZRA::RelHead;

// =================================================================================================
#pragma mark -
#pragma mark * AddedSearch

class AddedSearch
	{
public:
	AddedSearch();
	AddedSearch(const AddedSearch& iOther);
	~AddedSearch();
	AddedSearch& operator=(const AddedSearch& iOther);

	AddedSearch(int64 iRefcon, const ZRef<ZRA::Expr_Rel>& iRel);

	int64 GetRefcon() const;
	ZRef<ZRA::Expr_Rel> GetRel() const;

private:
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SearchResult

class SearchResult
	{
public:
	SearchResult();
	SearchResult(const SearchResult& iOther);
	~SearchResult();
	SearchResult& operator=(const SearchResult& iOther);

	SearchResult(int64 iRefcon, const ZRef<ZQE::Result>& iResult, const ZRef<Event>& iEvent);

	int64 GetRefcon() const;
	ZRef<ZQE::Result> GetResult() const;
	ZRef<Event> GetEvent() const;

private:
	int64 fRefcon;
	ZRef<ZQE::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source

class Source : public ZCounted
	{
protected:
	Source();

public:
	virtual ~Source();

	virtual std::set<RelHead> GetRelHeads() = 0;

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount) = 0;

	virtual void CollectResults(std::vector<SearchResult>& oChanged) = 0;

	typedef ZCallable<void(ZRef<Source>)> Callable_ResultsAvailable;
	void SetCallable_ResultsAvailable(ZRef<Callable_ResultsAvailable> iCallable);

protected:
	void pInvokeCallable_ResultsAvailable();

private:
	ZRef<Callable_ResultsAvailable> fCallable_ResultsAvailable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SourceFactory

class SourceFactory : public ZCounted
	{
protected:
	SourceFactory();

public:
	virtual ~SourceFactory();
	
	virtual ZRef<Source> Make() = 0;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source__
