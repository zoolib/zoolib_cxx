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

#include "zoolib/ZUtil_STL.h"

#include "zoolib/dataspace/ZDataspace_Source.h"

namespace ZooLib {
namespace ZDataspace {

using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * AddedSearch

AddedSearch::AddedSearch(int64 iRefcon, const ZRef<ZRA::Expr_Rel>& iRel)
:	fRefcon(iRefcon)
,	fRel(iRel)
	{}

AddedSearch::AddedSearch()
	{}

AddedSearch::AddedSearch(const AddedSearch& iOther)
:	fRefcon(iOther.fRefcon)
,	fRel(iOther.fRel)
	{}

AddedSearch::~AddedSearch()
	{}

AddedSearch& AddedSearch::operator=(const AddedSearch& iOther)
	{
	fRefcon = iOther.fRefcon;
	fRel = iOther.fRel;
	return *this;
	}

int64 AddedSearch::GetRefcon() const
	{ return fRefcon; }

ZRef<ZRA::Expr_Rel> AddedSearch::GetRel() const
	{ return fRel; }

// =================================================================================================
#pragma mark -
#pragma mark * SearchResult

SearchResult::SearchResult()
	{}

SearchResult::SearchResult(const SearchResult& iOther)
:	fRefcon(iOther.fRefcon)
,	fResult(iOther.fResult)
,	fEvent(iOther.fEvent)
	{}

SearchResult::~SearchResult()
	{}

SearchResult& SearchResult::operator=(const SearchResult& iOther)
	{
	fRefcon = iOther.fRefcon;
	fResult = iOther.fResult;
	fEvent = iOther.fEvent;
	return *this;
	}

SearchResult::SearchResult(
	int64 iRefcon, const ZRef<ZQE::Result>& iResult, const ZRef<Event>& iEvent)
:	fRefcon(iRefcon)
,	fResult(iResult)
,	fEvent(iEvent)
	{}

int64 SearchResult::GetRefcon() const
	{ return fRefcon; }

ZRef<ZQE::Result> SearchResult::GetResult() const
	{ return fResult; }

ZRef<Event> SearchResult::GetEvent() const
	{ return fEvent; }

// =================================================================================================
#pragma mark -
#pragma mark * Source

Source::Source()
	{}

Source::~Source()
	{}

void Source::SetCallable_ResultsAvailable(ZRef<Callable_ResultsAvailable> iCallable)
	{
	if (iCallable)
		ZAssert(!fCallable_ResultsAvailable);
	fCallable_ResultsAvailable = iCallable;
	}

void Source::pInvokeCallable_ResultsAvailable()
	{
	if (ZRef<Callable_ResultsAvailable> theCallable = fCallable_ResultsAvailable)
		theCallable->Call(this);
	}

} // namespace ZDataspace
} // namespace ZooLib

