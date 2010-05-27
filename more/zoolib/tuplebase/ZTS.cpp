/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTS.h"

namespace ZooLib {

/**
\defgroup Tuplestore
A tuplestore is simply a humungous array of 2^64 tuples. Each entry in the array can be
considered as being an ID/tuple pair, where the ID is simply the index into the array,
and thus each entry's ID is unique within the tuplestore.

A tuplestore is fixed in size and always fully populated so that every entry has a tuple.
When first created it contains 2^64 \em empty tuples, which is why you don't need
2^64 * x bytes of storage to create a tuplestore -- it's a sparse array in which empty tuples
needn't be physically stored.

In addition a tuplestore allocates IDs, on demand it will return IDs that have never been
allocated before and never will again. This lets us add new data to a tuplestore without
overwriting IDs that have already been used.

The class ZTS defines an API for reading and writing entries in a tuplestore. Generally though
you'll use a \ref Tuplebase or a \ref Tuplesoup instance, depending on whether you want
transactional database-like features, or the more UI-friendly soup access.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZTS

/**
\class ZTS
\nosubgrouping
\ingroup Tuplestore

ZTS defines an API for managing the huge, fixed-size, sparse array of tuples discussed in
\ref Tuplestore. ZTS::SetTuples and ZTS::GetTuples provide access to the tuples themselves.
ZTS::AllocateIDs manages the issuance of entries that have never been used before. ZTS::Search
allows the identification of entries whose tuples match a ZTBSpec. ZTS::GetReadLock and
ZTS::GetWriteLock provide access to the reader/writer lock which arbitrates access to the tuples.
*/

ZTS::ZTS()
	{}

ZTS::~ZTS()
	{}

/**
\fn void ZTS::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
Allocate some number of IDs that have never been allocated before, nor ever will again.
\param iCount The count of IDs we'd like to be issued
\param oBaseID The first ID issued
\param oCountIssued The count of IDs consecutive to \a oBaseID that were issused. This will
be <= iCount, and may even be zero. If insufficient IDs have been issued, call AllocateIDs again.
*/

/**
\fn void ZTS::SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
Store the tuples in \a iTuples at the corresponding IDs in \a iIDs. The arrays referenced
by \a iIDs and \a iTuples must each contain at least \a iCount values.

The ZMutexBase returned by ZTS::GetWriteLock must have been acquired by the current thread.
*/

/**
\fn void ZTS::GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
Take the tuples at the IDs in \a iIDs and copy them to the corresponding indices in
\a oTuples. The arrays referenced by \a iIDs and \a iTuples must each contain at
least \a iCount values.

The ZMutexBase returned by ZTS::GetReadLock must have been acquired by the current thread.
*/

/**
\fn void ZTS::Search(const ZTBSpec& iSpec, const std::set<uint64>& iSkipIDs, std::set<uint64>& oIDs)
For each entry in the tuplestore whose tuple satisfies \a iSpec, insert the ID into \a oIDs (which
must be empty), but do not insert any ID which occurs in \a iSkipIDs.

The ZMutexBase returned by ZTS::GetReadLock must have been acquired by the current thread.
*/

/**
Ensure that any data is pushed out to stable storage.
*/
void ZTS::Flush()
	{}

/**
\fn ZMutexBase& ZTS::GetReadLock()
Return a ZMutexBase reference to the reader portion of a reader/writer lock.
*/

/**
\fn ZMutexBase& ZTS::GetWriteLock()
Return a ZMutexBase reference to the writer portion of a reader/writer lock.
*/

/** This is simply a helper function that returns the tuple stored at ID \a iID by
calling ZTS::GetTuples. As such the ZMutexBase returned by ZTS::GetReadLock must have
been acquired by the current thread.
*/
ZTuple ZTS::GetTuple(uint64 iID)
	{
	ZTuple theTuple;
	this->GetTuples(1, &iID, &theTuple);
	return theTuple;
	}

} // namespace ZooLib
