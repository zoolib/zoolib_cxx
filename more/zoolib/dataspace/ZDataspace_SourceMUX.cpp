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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/dataspace/ZDataspace_SourceMUX.h"

namespace ZooLib {
namespace ZDataspace {

using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX::PSearch

class SourceMUX::PSearch
	{
	int64 fRefcon;
	ZRef<ZQE::Result> fResult;
	set<ClientSearch*> fClientSearches;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX::ClientSearch

class SourceMUX::ClientSearch
	{
	ClientSource* fCS;
	int64 fRefcon;
	PSearch* fPSearch;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX::ClientSource

class SourceMUX::ClientSource : public Source
	{
public:
	ClientSource(ZRef<SourceMUX> iMUX);

// From ZCounted via Source
	virtual void Finalize();

// From Source
	virtual RelHead GetRelHead();

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

	ZRef<SourceMUX> fMUX;
	};

SourceMUX::ClientSource::ClientSource(ZRef<SourceMUX> iMUX)
:	fMUX(iMUX)
	{}

void SourceMUX::ClientSource::Finalize()
	{ fMUX->pFinalizeProxy(this); }

RelHead SourceMUX::ClientSource::GetRelHead()
	{ return fMUX->pGetRelHead(this); }

void SourceMUX::ClientSource::ModifyRegistrations(
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{ fMUX->pModifyRegistrations(this, iAdded, iAddedCount, iRemoved, iRemovedCount); }

void SourceMUX::ClientSource::CollectResults(std::vector<SearchResult>& oChanged)
	{ fMUX->pCollectResults(this, oChanged); }

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX

SourceMUX::SourceMUX(ZRef<Source> iSource)
:	fSource(iSource)
,	fCallable_ResultsAvailable(MakeCallable(MakeWeakRef(this), &SourceMUX::pResultsAvailable))
	{
	fSource->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);
	ZUnimplemented();
	}

SourceMUX::~SourceMUX()
	{
	fSource->SetCallable_ResultsAvailable(null);
	}

ZRef<Source> SourceMUX::Make()
	{
	ZAcqMtxR acq(fMtxR);

	ZRef<ClientSource> theCS = new ClientSource(this);
	fClientSources.insert(theCS.Get());
	return theCS;
	}

RelHead SourceMUX::pGetRelHead(ZRef<ClientSource> iCS)
	{ return fSource->GetRelHead(); }

void SourceMUX::pModifyRegistrations(ZRef<ClientSource> iCS,
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	}

void SourceMUX::pCollectResults(ZRef<ClientSource> iCS,
	std::vector<SearchResult>& oChanged)
	{
	}

void SourceMUX::pResultsAvailable(ZRef<Source> iSource)
	{
	
	}

void SourceMUX::pFinalizeProxy(ClientSource* iCS)
	{
	ZAcqMtxR acq(fMtxR);

	if (!iCS->FinishFinalize())
		return;

	// Detach iCS from all PSearches

	

	ZUtil_STL::sEraseMustContain(1, fClientSources, iCS);
	delete iCS;
	}

} // namespace ZDataspace
} // namespace ZooLib
