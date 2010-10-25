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

#include "zoolib/dataspace/ZDataspace_SearchSpec.h"
#include "zoolib/zqe/ZQE_Row.h"
#include "zoolib/zra/ZRA_RelHead.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

using ZIntervalTreeClock::Event;
using ZIntervalTreeClock::Clock;

using ZRA::RelHead;

// =================================================================================================
#pragma mark -
#pragma mark * AddedSearch

class AddedSearch
	{
public:
	AddedSearch(int64 iRefcon, const SearchSpec& iSearchSpec);

	int64 fRefcon;
	SearchSpec fSearchSpec;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SearchRows

class SearchRows : public ZCountedWithoutFinalize
	{
public:
	SearchRows(const std::vector<string8>& iRowHead, ZRef<ZQE::RowVector> iRowVector);
	SearchRows(std::vector<string8>* ioRowHead, ZRef<ZQE::RowVector> iRowVector);
	SearchRows(std::vector<string8>* ioRowHead);

	const std::vector<string8>& GetRowHead();
	ZRef<ZQE::RowVector> GetRowVector();

private:
	std::vector<string8> fRowHead;
	ZRef<ZQE::RowVector> fRowVector;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SearchResult

class SearchResult
	{
public:
	int64 fRefcon;
	ZRef<SearchRows> fSearchRows;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source

class Source
	{
protected:
	Source();

public:
	virtual ~Source();

	virtual std::set<RelHead> GetRelHeads() = 0;

	virtual void Update(
		bool iLocalOnly,
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount,
		std::vector<SearchResult>& oChanged,
		ZRef<Event>& oEvent) = 0;

	typedef ZCallable<void(Source*)> Callable;
	void SetCallable(ZRef<Callable> iCallable);

protected:
	void pInvokeCallables();

private:
	ZRef<Callable> fCallable;
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
	
	virtual Source* Make() = 0;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source__
