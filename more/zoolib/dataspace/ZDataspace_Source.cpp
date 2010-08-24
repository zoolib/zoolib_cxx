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
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark *

AddedSearch::AddedSearch(int64 iRefcon, const SearchSpec& iSearchSpec)
:	fRefcon(iRefcon)
,	fSearchSpec(iSearchSpec)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * SearchRows

SearchRows::SearchRows(const std::vector<string8>& iRowHead, ZRef<ZQE::RowVector> iRowVector)
:	fRowHead(iRowHead)
,	fRowVector(iRowVector)
	{
	ZAssert(fRowVector);
	}

SearchRows::SearchRows(std::vector<string8>* ioRowHead, ZRef<ZQE::RowVector> iRowVector)
:	fRowVector(iRowVector)
	{
	ZAssert(fRowVector);
	ioRowHead->swap(fRowHead);
	}

SearchRows::SearchRows(std::vector<string8>* ioRowHead)
	{
	ioRowHead->swap(fRowHead);
	fRowVector = new ZQE::RowVector;
	}

const std::vector<string8>& SearchRows::GetRowHead()
	{ return fRowHead; }

ZRef<ZQE::RowVector> SearchRows::GetRowVector()
	{ return fRowVector; }

// =================================================================================================
#pragma mark -
#pragma mark *

Source::Source()
	{}

Source::~Source()
	{}

void Source::SetCallable(ZRef<Callable> iCallable)
	{ fCallable = iCallable; }

void Source::pInvokeCallables()
	{
	if (ZRef<Callable> theCallable = fCallable)
		theCallable->Invoke(this);
	}

} // namespace ZDataspace
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark *

// Notes

#if 0

//##typedef string ReadableBy;

class DeltaReadable
	{
	vector<ReadableBy> fReadableBy;
	vector<ZData> fInserted;
	vector<DataHash> fErased;
	};

class DeltaPack
	{
	Timestamp fTimestamp;
	WrittenBy fWrittenBy;
	vector<DeltaReadable> fDRs;
	};

Source
	{
	void Register(ZRef<Searcher>, Rel iRel);

	void Update();
	void Commit();
	};

// Synced from elsewhere, but no writes possible.
// Some readonly datasets will actually fetch *everything* from elsewhere,
// which they can then pass on to others, but will only make a subset
// usable by the database layer.
Source_ReadOnly
	{
	};

class Source_Client
	{
	// Specify the list of ReadableBys we care about for this connection.
	// If we're readwrite on the connection we'll also need to specify our WrittenBy thing.
	};

Source_ReadWrite
	{
	// Specify WrittenBy thing
	void Insert(ReadableSet iRS, ZData iData);
	void Erase(ReadableSet iRS, ZData iData);
	};

#endif
