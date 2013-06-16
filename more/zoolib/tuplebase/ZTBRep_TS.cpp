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

#include "zoolib/tuplebase/ZTBRep_TS.h"
#include "zoolib/tuplebase/ZTupleQuisitioner.h"
#include "zoolib/tuplebase/ZTupleIndex.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#ifndef ZCONFIG_TBRep_TS_Logging
#	define ZCONFIG_TBRep_TS_Logging 0
#endif

#if ZCONFIG_TBRep_TS_Logging
#	include "zoolib/ZLog.h"
#endif

using std::less;
using std::map;
using std::min;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace ZooLib {

using namespace ZUtil_STL;

#define ASSERTLOCKED(a) ZAssertStop(kDebug, a.IsLocked())
#define ASSERTUNLOCKED(a) ZAssertStop(kDebug, !a.IsLocked())

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS::TransTuple

class ZTBRep_TS::TransTupleUsing
:	public DListLink<ZTBRep_TS::TransTuple, ZTBRep_TS::TransTupleUsing, ZTBRep_TS::kDebug>
	{};

class ZTBRep_TS::TransTuple : public ZTBRep_TS::TransTupleUsing
	{
public:
	TransTuple(TupleInUse& iTIU, const ZTuple& iValue, uint64 iClockStart, bool iWritten)
	:	fTupleInUse(iTIU),
		fValue_Current(iValue),
		fClock_Start(iClockStart),
		fClock_End(0),
		fRead(!iWritten),
		fWritten(iWritten)
		{}

	TupleInUse& fTupleInUse;

	ZTuple fValue_Current;
	uint64 fClock_Start;
	uint64 fClock_End;

	bool fRead;
	bool fWritten;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Non-releasing allocator

namespace { // anonymous

// RealAllocator and TTAllocator, names I'm not overly fond of.
// RealAllocator is a simple class that allocates space
// suballocated from pages that are not themselves disposed till the RealAllocator is
// disposed, and which makes no provision for taking back space once allocated. It's used
// to provide the backing store for the map of IDs to TransTuples, which only ever grows
// during the course of a transaction, and is then tossed in its entirety.
// TTAllocator is the actual class passed to the map<uint64, TransTuple> template.

class RealAllocator
	{
public:
	vector<char*> fPages;
	static const size_t kPageSize = 4096;
	size_t fCurOffset;

	RealAllocator();
	~RealAllocator();
	void* Allocate(size_t iSize);
	};


RealAllocator::RealAllocator()
:	fCurOffset(kPageSize)
	{}

RealAllocator::~RealAllocator()
	{
	for (vector<char*>::iterator i = fPages.begin(); i != fPages.end(); ++i)
		delete[] *i;
	}

void* RealAllocator::Allocate(size_t iSize)
	{
	ZAssert(iSize <= kPageSize);
	fCurOffset += iSize;
	if (fCurOffset > kPageSize)
		{
		fPages.push_back(new char[kPageSize]);
		fCurOffset = iSize;
		}
	return fPages.back() + fCurOffset - iSize;
	}

template <typename T>
class TTAllocator;

template <>
class TTAllocator<void>
	{
public:
	typedef void value_type;
	typedef TTAllocator<void> class_type;
	typedef void *pointer;
	typedef void const *const_pointer;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;

	template <typename U>
	struct rebind
		{
		typedef TTAllocator<U> other;
		};
	};

template <typename T>
class TTAllocator
	{
public:
	typedef T value_type;
	typedef TTAllocator<value_type> class_type;
	typedef value_type *pointer;
	typedef value_type const *const_pointer;
	typedef value_type &reference;
	typedef value_type const &const_reference;
	typedef ptrdiff_t difference_type;
	typedef size_t size_type;
	typedef void *deallocate_pointer;

	template <typename U>
	struct rebind
		{
		typedef TTAllocator<U> other;
		};

	RealAllocator& fRealAllocator;

	TTAllocator(RealAllocator& iRealAllocator) throw ()
	:	fRealAllocator(iRealAllocator)
		{}

	TTAllocator(class_type const & iOther) throw ()
	:	fRealAllocator(iOther.fRealAllocator)
		{}

	template <typename U>
	TTAllocator(TTAllocator<U> const & iOther) throw ()
	:	fRealAllocator(iOther.fRealAllocator)
		{}

	~TTAllocator() throw ()
		{}

	size_type max_size() const throw ()
		{
		return static_cast<size_type>(-1) / sizeof(value_type);
		}

	pointer address(reference x) const throw ()
		{ return &x; }

	const_pointer address(const_reference x) const throw ()
		{ return &x; }

	pointer allocate(size_type n, TTAllocator<void>::const_pointer pv = 0)
		{
		((void)(pv)) ;
		TTAllocator<void>::pointer p = fRealAllocator.Allocate(n * sizeof(value_type));
		if (p == 0)
			throw ::std::bad_alloc();

		return static_cast<pointer>(p);
		}

	void deallocate(deallocate_pointer p, size_type n)
		{}

	void deallocate(deallocate_pointer p)
		{}

	void construct(pointer p, value_type const &x)
		{
		new(p) value_type(x);
		}

	void construct(pointer p)
		{
		new(p) value_type();
		}

	void destroy(pointer p) throw ()
		{
		static_cast<pointer>(p)->~value_type();
		}
	};

template <typename T>
inline bool operator ==(const TTAllocator<T>& , const TTAllocator<T>&)
	{ return true; }

template <typename T>
inline bool operator !=(const TTAllocator<T>& , const TTAllocator<T>&)
	{ return false; }

typedef std::map<uint64, ZTBRep_TS::TransTuple, less<uint64>,
	TTAllocator<pair<const uint64, ZTBRep_TS::TransTuple> > > MapTransTuple_t;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS::Transaction declaration

class ZTBRep_TS::Transaction : public ZTBRepTransaction
	{
public:
	Transaction(ZRef<ZTBRep_TS> iTBRep, const vector<ZRef<ZTupleIndexFactory> >& iFactories);
	~Transaction();

// From ZTBRepTransaction
	virtual ZRef<ZTBRep> GetTBRep();

	virtual void Search(const ZTBQuery& iQuery, Callback_Search_t iCallback, void* iRefcon);
	virtual void Count(const ZTBQuery& iQuery, Callback_Count_t iCallback, void* iRefcon);
	virtual void GetTuples(size_t iCount, const uint64* iIDs,
					Callback_GetTuple_t iCallback_Get, void* iRefcon);
	virtual void SetTuple(uint64 iID, const ZTuple& iTuple);
	virtual void AbortPreValidate();
	virtual void Validate(Callback_Validate_t iCallback_Validate, void* iRefcon);
	virtual void AcceptFailure();
	virtual void CancelPostValidate();
	virtual void Commit(Callback_Commit_t iCallback_Commit, void* iRefcon);

	void* Allocate(size_t iSize);

private:
	ZRef<ZTBRep_TS> fTBRep;
	vector<ZTupleIndex*> fIndices;

	Transaction* fWaiting_Prev;
	Transaction* fWaiting_Next;

	Transaction* fWaiting_NextReader;

	RealAllocator fRealAllocator;
	MapTransTuple_t fTransTuples;
	set<uint64> fSet_Written;

	bool fValidateCalled;
	MapTransTuple_t::iterator fIter_Validate;
	Callback_Validate_t fCallback_Validate;
	void* fRefcon;

	class TupleQuisitioner;
	friend class TupleQuisitioner;

	friend class ZTBRep_TS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS::Transaction::TupleQuisitioner

class ZTBRep_TS::Transaction::TupleQuisitioner : public ZTupleQuisitioner
	{
public:
	TupleQuisitioner(Transaction* iTransaction);

	virtual void Search(const ZTBSpec& iSpec, set<uint64>& ioIDs);
	virtual void Search(const ZTBSpec& iSpec, vector<uint64>& oIDs);

	virtual void FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual ZTuple FetchTuple(uint64 iID);

private:
	Transaction* fTransaction;
	};

ZTBRep_TS::Transaction::TupleQuisitioner::TupleQuisitioner(Transaction* iTransaction)
:	fTransaction(iTransaction)
	{}

void ZTBRep_TS::Transaction::TupleQuisitioner::Search(const ZTBSpec& iSpec, set<uint64>& ioIDs)
	{ fTransaction->fTBRep->Trans_Search(fTransaction, iSpec, ioIDs); }

void ZTBRep_TS::Transaction::TupleQuisitioner::Search(const ZTBSpec& iSpec, vector<uint64>& oIDs)
	{ fTransaction->fTBRep->Trans_Search(fTransaction, iSpec, oIDs); }

void ZTBRep_TS::Transaction::TupleQuisitioner::FetchTuples(
	size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{ fTransaction->fTBRep->Trans_FetchTuples(fTransaction, iCount, iIDs, oTuples); }

ZTuple ZTBRep_TS::Transaction::TupleQuisitioner::FetchTuple(uint64 iID)
	{ return fTransaction->fTBRep->Trans_FetchTuple(fTransaction, iID); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS::Transaction definition

ZTBRep_TS::Transaction::Transaction(
	ZRef<ZTBRep_TS> iTBRep, const vector<ZRef<ZTupleIndexFactory> >& iFactories)
:	fTBRep(iTBRep),
	fWaiting_Prev(nullptr),
	fWaiting_Next(nullptr),
	fWaiting_NextReader(nullptr),
	fTransTuples(less<uint64>(), MapTransTuple_t::allocator_type(fRealAllocator)),
	fValidateCalled(false),
	fCallback_Validate(nullptr),
	fRefcon(nullptr)
	{
	for (vector<ZRef<ZTupleIndexFactory> >::const_iterator factIter = iFactories.begin();
		factIter != iFactories.end(); ++factIter)
		{
		ZTupleIndex* theIndex = (*factIter)->Make();
		fIndices.push_back(theIndex);
		}
	}

ZTBRep_TS::Transaction::~Transaction()
	{
	ZAssertStop(kDebug, !fWaiting_Prev);
	ZAssertStop(kDebug, !fWaiting_Next);
	ZAssertStop(kDebug, !fWaiting_NextReader);
	ZAssertStop(kDebug, fTransTuples.empty());

	ZUtil_STL::sDeleteAll(fIndices.begin(), fIndices.end());
	}

ZRef<ZTBRep> ZTBRep_TS::Transaction::GetTBRep()
	{ return fTBRep; }

void ZTBRep_TS::Transaction::Search(
	const ZTBQuery& iQuery, Callback_Search_t iCallback, void* iRefcon)
	{
	#if ZCONFIG_TBRep_TS_Logging
		if (ZLOG(s, eInfo, "ZTBRep_TS::Transaction::Search"))
			s << "\n" << iQuery.AsTuple() << "\n";
	#endif

	vector<uint64> vectorResults;
	if (ZRef<ZTBQueryNode> theNode = iQuery.GetNode())
		TupleQuisitioner(this).Query(theNode, nullptr, vectorResults);

	iCallback(iRefcon, vectorResults);
	}

void ZTBRep_TS::Transaction::Count(
	const ZTBQuery& iQuery, Callback_Count_t iCallback, void* iRefcon)
	{
	set<uint64> setResults;
	if (ZRef<ZTBQueryNode> theNode = iQuery.GetNode())
		TupleQuisitioner(this).Query_Unordered(theNode, nullptr, setResults);

	iCallback(iRefcon, setResults.size());
	}

void ZTBRep_TS::Transaction::GetTuples(size_t iCount, const uint64* iIDs,
					Callback_GetTuple_t iCallback, void* iRefcon)
	{
	if (iCount)
		{
		if (iCallback)
			{
			vector<ZTuple> theTuples(iCount, ZTuple());
			fTBRep->Trans_FetchTuples(this, iCount, iIDs, &theTuples[0]);
			iCallback(iRefcon, iCount, iIDs, &theTuples[0]);
			}
		else
			{
			fTBRep->Trans_FetchTuples(this, iCount, iIDs, nullptr);
			}
		}
	else
		{
		if (iCallback)
			iCallback(iRefcon, 0, nullptr, nullptr);
		}
	}

void ZTBRep_TS::Transaction::SetTuple(uint64 iID, const ZTuple& iTuple)
	{ fTBRep->Trans_SetTuple(this, iID, iTuple); }

void ZTBRep_TS::Transaction::AbortPreValidate()
	{ fTBRep->Trans_AbortPreValidate(this); }

void ZTBRep_TS::Transaction::Validate(Callback_Validate_t iCallback_Validate, void* iRefcon)
	{ fTBRep->Trans_Validate(this, iCallback_Validate, iRefcon); }

void ZTBRep_TS::Transaction::AcceptFailure()
	{ fTBRep->Trans_AcceptFailure(this); }

void ZTBRep_TS::Transaction::CancelPostValidate()
	{ fTBRep->Trans_CancelPostValidate(this); }

void ZTBRep_TS::Transaction::Commit(Callback_Commit_t iCallback_Commit, void* iRefcon)
	{
	fTBRep->Trans_Commit(this);
	// The TS has all the tuples. Without a flush we don't know that they're in
	// stable storage, but we're as close to committed as we guarantee, so make the callback.
	if (iCallback_Commit)
		iCallback_Commit(iRefcon);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_TS

ZTBRep_TS::ZTBRep_TS(const vector<ZRef<ZTupleIndexFactory> >& iIndexFactories, ZRef<ZTS> iTS)
:	fIndexFactories(iIndexFactories),
	fTS(iTS)
	{
	ZAssertStop(kDebug, fTS);
	fClock = 1;
	}

ZTBRep_TS::~ZTBRep_TS()
	{}

void ZTBRep_TS::AllocateIDs(size_t iCount,
				Callback_AllocateIDs_t iCallback_AllocateIDs, void* iRefcon)
	{
	while (iCount)
		{
		uint64 baseID;
		size_t countIssued;
		fTS->AllocateIDs(iCount, baseID, countIssued);
		iCount -= countIssued;
		iCallback_AllocateIDs(iRefcon, baseID, countIssued);
		}
	}

ZTBRepTransaction* ZTBRep_TS::CreateTransaction()
	{
	Transaction* newTransaction = new Transaction(this, fIndexFactories);

	ZMutexLocker lock(fMutex_Structure);
	ZUtil_STL::sInsertMust(kDebug, fTransactions, newTransaction);
	return newTransaction;
	}

void ZTBRep_TS::AddIndex(ZRef<ZTupleIndexFactory> iFactory)
	{
	fIndexFactories.push_back(iFactory);
	}

void ZTBRep_TS::AddIndices(const vector<ZRef<ZTupleIndexFactory> >& iFactories)
	{
	fIndexFactories.insert(fIndexFactories.end(), iFactories.begin(), iFactories.end());
	}

ZRef<ZTS> ZTBRep_TS::GetTS()
	{ return fTS; }

// =================================================================================================
#pragma mark -
#pragma mark * Trans

ZTuple ZTBRep_TS::Trans_FetchTuple(Transaction* iTransaction, uint64 iID)
	{
	ZLocker locker(fTS->GetReadLock());
	ZTuple result;
	this->pFetchTuples(iTransaction, 1, &iID, &result);
	return result;
	}

void ZTBRep_TS::Trans_FetchTuples(Transaction* iTransaction,
	size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{
	ZLocker locker(fTS->GetReadLock());
	this->pFetchTuples(iTransaction, iCount, iIDs, oTuples);
	}

void ZTBRep_TS::Trans_Search(Transaction* iTransaction, const ZTBSpec& iSpec, set<uint64>& ioIDs)
	{
	ZLocker locker(fTS->GetReadLock());

	#if ZCONFIG_TBRep_TS_Logging
		ZTime startTime = ZTime::sSystem();
		ZLog::S s(ZLog::eInfo, "ZTBRep_TS::Trans_Search");
		s << iSpec.AsTuple();
	#endif

	// fTS->Search will return all tuples matching iSpec. Because we copy written data from
	// TupleInUse back to the TS the search will also return the same tuples as if we were to
	// walk TuplesInUse manually. It will also return any tuples that match iSpec, but which
	// now have unmatching values in iTransaction, so we pass the list of touched tuples
	// (maintained in iTransaction->fSet_Written) so that fTS can ignore those.
	if (ioIDs.empty())
		{
		fTS->Search(iSpec, iTransaction->fSet_Written, ioIDs);
		this->pFetchTuples(iTransaction, ioIDs);
		}
	else
		{
		set<uint64> tsResults;
		fTS->Search(iSpec, iTransaction->fSet_Written, tsResults);
		this->pFetchTuples(iTransaction, tsResults);
		ioIDs.insert(tsResults.begin(), tsResults.end());
		}

	// Now augment the results with those matching tuples written by this transaction.
	const ZTBSpec::CriterionUnion& theCriterionUnion = iSpec.GetCriterionUnion();
	vector<ZTupleIndex*> indicesToUse;
	if (not ZTupleIndex::sMatchIndices(theCriterionUnion, iTransaction->fIndices, indicesToUse))
		{
		#if ZCONFIG_TBRep_TS_Logging
			s << "\nAt least one clause doesn't have a usable index\n";
			size_t resultSize = 0;
		#endif

		// At least one of iSpec's OR clauses could not be accelerated by
		// an index. We're going to have to walk every tuple for that
		// clause, so we might as well do so for all of them.
		for (MapTransTuple_t::iterator touchedIter = iTransaction->fTransTuples.begin();
			touchedIter != iTransaction->fTransTuples.end(); ++touchedIter)
			{
			TransTuple& theTT = (*touchedIter).second;
			if (theTT.fWritten && iSpec.Matches(theTT.fValue_Current))
				{
				#if ZCONFIG_TBRep_TS_Logging
					++resultSize;
				#endif
				ioIDs.insert(theTT.fTupleInUse.fID);
				}
			}

		#if ZCONFIG_TBRep_TS_Logging
			s.Writef("\nResult size: %d, of %d, overall elapsed: %gms.",
				resultSize,
				iTransaction->fTransTuples.size(),
				1000*(ZTime::sSystem() - startTime));
		#endif
		}
	else
		{
		// We found an index for each clause of iSpec. So now use them.
		const set<uint64> emptySet;

		vector<ZTupleIndex*>::const_iterator iterIndex = indicesToUse.begin();
		ZTBSpec::CriterionUnion::const_iterator iterCriterionUnion = theCriterionUnion.begin();

		for (/*no init*/;iterIndex != indicesToUse.end(); ++iterIndex, ++iterCriterionUnion)
			{
			vector<const ZTBSpec::Criterion*> uncheckedCriteria;

			vector<uint64> currentResults;
			currentResults.reserve(100);

			(*iterIndex)->Find(*iterCriterionUnion, emptySet, uncheckedCriteria, currentResults);

			if (uncheckedCriteria.empty())
				{
				ioIDs.insert(currentResults.begin(), currentResults.end());
				}
			else
				{
				for (vector<uint64>::iterator resultIter = currentResults.begin();
					resultIter != currentResults.end(); /*no increment*/)
					{
					uint64 currentID = *resultIter++;
					MapTransTuple_t::iterator gottenIter =
						iTransaction->fTransTuples.find(currentID);

					ZAssertStop(kDebug, gottenIter != iTransaction->fTransTuples.end());
					ZAssertStop(kDebug, (*gottenIter).second.fWritten);
					const ZTuple& theTuple = (*gottenIter).second.fValue_Current;
					bool allOkay = true;
					for (vector<const ZTBSpec::Criterion*>::iterator
						critIter = uncheckedCriteria.begin();
						critIter != uncheckedCriteria.end(); ++critIter)
						{
						if (!(*critIter)->Matches(theTuple))
							{
							allOkay = false;
							break;
							}
						}
					if (allOkay)
						ioIDs.insert(currentID);
					}
				}
			}
		}
	}

void ZTBRep_TS::Trans_Search(Transaction* iTransaction, const ZTBSpec& iSpec, vector<uint64>& oIDs)
	{
	set<uint64> results;
	this->Trans_Search(iTransaction, iSpec, results);
	oIDs.insert(oIDs.end(), results.begin(), results.end());
	}

void ZTBRep_TS::Trans_SetTuple(Transaction* iTransaction, uint64 iID, const ZTuple& iTuple)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTUNLOCKED(fMutex_Structure);

	MapTransTuple_t::iterator position = iTransaction->fTransTuples.lower_bound(iID);
	if (position != iTransaction->fTransTuples.end() && (*position).first == iID)
		{
		// We've previously gotten this tuple.
		TransTuple& theTransTuple = (*position).second;

		// It must have been read or written or it wouldn't be in fTransTuples.
		ZAssertStop(kDebug, theTransTuple.fWritten || theTransTuple.fRead);

		if (theTransTuple.fWritten)
			{
			// We've written to this tuple previously, so it had been placed in our
			// indices under the value that we're about to overwrite, and must be removed.
			for (vector<ZTupleIndex*>::iterator
				i = iTransaction->fIndices.begin(); i != iTransaction->fIndices.end(); ++i)
				{
				(*i)->Erase(iID, &theTransTuple.fValue_Current);
				}
			}
		else
			{
			theTransTuple.fWritten = true;
			iTransaction->fSet_Written.insert(iID);
			}
		theTransTuple.fValue_Current = iTuple;
		}
	else
		{
		// We've never seen this tuple before.
		ZLocker lockerRW(fTS->GetReadLock());
		ZMutexLocker lockerMutex(fMutex_Structure);

		TupleInUse& theTupleInUse = this->pGetTupleInUse(iID);

		this->pAllocateTransTuple(&position, iTransaction, theTupleInUse, iTuple, 0, true);

		iTransaction->fSet_Written.insert(iID);
		}

	// We've written to this tuple so need to enter it in the transaction's indices.
	for (vector<ZTupleIndex*>::iterator i = iTransaction->fIndices.begin();
		i != iTransaction->fIndices.end(); ++i)
		{
		(*i)->Insert(iID, &iTuple);
		}
	}

void ZTBRep_TS::Trans_AbortPreValidate(Transaction* iTransaction)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTUNLOCKED(fMutex_Structure);

	ZMutexLocker lock(fMutex_Structure);
	ZAssertStop(kDebug, !iTransaction->fValidateCalled);

	// Dispose all TransTuples.
	for (MapTransTuple_t::iterator
		i = iTransaction->fTransTuples.begin(), theEnd = iTransaction->fTransTuples.end();
		i != theEnd; ++i)
		{
		this->pDisposeTransTuple((*i).second);
		}

	iTransaction->fTransTuples.clear();

	ZUtil_STL::sEraseMust(kDebug, fTransactions, iTransaction);
	delete iTransaction;
	}

void ZTBRep_TS::Trans_Validate(Transaction* iTransaction,
				ZTBRepTransaction::Callback_Validate_t iCallback_Validate, void* iRefcon)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTUNLOCKED(fMutex_Structure);

	ZMutexLocker lock(fMutex_Structure);

	iTransaction->fValidateCalled = true;

	iTransaction->fCallback_Validate = iCallback_Validate;
	iTransaction->fRefcon = iRefcon;

	iTransaction->fIter_Validate = iTransaction->fTransTuples.begin();
	this->pTryValidation(iTransaction);
	}

void ZTBRep_TS::Trans_AcceptFailure(Transaction* iTransaction)
	{
	ASSERTUNLOCKED(fMutex_Structure);

	ZMutexLocker lock(fMutex_Structure);
	ZUtil_STL::sEraseMust(kDebug, fTransactions, iTransaction);
	delete iTransaction;
	}

void ZTBRep_TS::Trans_CancelPostValidate(Transaction* iTransaction)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTUNLOCKED(fMutex_Structure);

	ZMutexLocker lock(fMutex_Structure);
	ZAssertStop(kDebug, iTransaction->fValidateCalled);

	// Release all the locks that have been acquired, and dispose the TransTuples.
	for (MapTransTuple_t::iterator
		iterTT = iTransaction->fTransTuples.begin();
		iterTT != iTransaction->fIter_Validate; ++iterTT)
		{
		TransTuple& currentTT = (*iterTT).second;
		if (currentTT.fWritten || currentTT.fRead)
			this->pReleaseLock(iTransaction, currentTT.fTupleInUse, currentTT.fWritten);
		this->pDisposeTransTuple(currentTT);
		}

	// Dispose the remaining TransTuples.
	for (MapTransTuple_t::iterator
		iterTT = iTransaction->fIter_Validate, theEnd = iTransaction->fTransTuples.end();
		iterTT != theEnd; ++iterTT)
		{
		this->pDisposeTransTuple((*iterTT).second);
		}

	iTransaction->fTransTuples.clear();

	ZUtil_STL::sEraseMust(kDebug, fTransactions, iTransaction);
	delete iTransaction;
	}

static void spLatestStartEarliestEnd(
	const MapTransTuple_t& iTransTuples, uint64& oLatestStart, uint64& ioEarliestEnd)
	{
	oLatestStart = 0;
	for (MapTransTuple_t::const_iterator iterTT = iTransTuples.begin();
		iterTT != iTransTuples.end(); ++iterTT)
		{
		const ZTBRep_TS::TransTuple& currentTT = (*iterTT).second;
		if (currentTT.fRead)
			{
			if (oLatestStart < currentTT.fClock_Start)
				oLatestStart = currentTT.fClock_Start;

			if (currentTT.fClock_End && ioEarliestEnd > currentTT.fClock_End)
				ioEarliestEnd = currentTT.fClock_End;
			}
		}
	}

void ZTBRep_TS::Trans_Commit(Transaction* iTransaction)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTUNLOCKED(fMutex_Structure);

	// We've locked the tuples. Now we need to run through and update the TupleInUse instances
	// with the changed values, and update referencing TransTuples with the time at which
	// the value got changed.

	if (not iTransaction->fSet_Written.empty())
		{
		ZLocker locker_Write(fTS->GetWriteLock());
		ZMutexLocker lockerStructure(fMutex_Structure);

		// It is commit operations that advance our clock.
		++fClock;

		if (not iTransaction->fSet_Written.empty())
			{
			uint64 latestStart;
			uint64 earliestEnd = fClock;
			spLatestStartEarliestEnd(iTransaction->fTransTuples, latestStart, earliestEnd);

			vector<uint64> storedIDs;
			vector<ZTuple> storedTuples;
			storedIDs.reserve(iTransaction->fTransTuples.size());
			storedTuples.reserve(iTransaction->fTransTuples.size());

			for (MapTransTuple_t::iterator
				iterTT = iTransaction->fTransTuples.begin(),
				theEnd = iTransaction->fTransTuples.end();
				iterTT != theEnd; ++iterTT)
				{
				TransTuple& currentTT = (*iterTT).second;
				if (currentTT.fWritten)
					{
					TupleInUse& currentTIU = currentTT.fTupleInUse;

					if (currentTIU.fClock_LastWritten < earliestEnd)
						{
						// We're writing later than the last writer, so our value gets stored.
						currentTIU.fClock_LastWritten = earliestEnd;
						currentTIU.fValue = currentTT.fValue_Current;
						storedIDs.push_back(currentTIU.fID);
						storedTuples.push_back(currentTIU.fValue);
						}

					for (DListIterator<TransTuple, TransTupleUsing>
						iter = currentTIU.fUsingTransTuples;
						iter; iter.Advance())
						{
						TransTuple* otherTT = iter.Current();
						if (otherTT->fClock_End == 0 || otherTT->fClock_End > earliestEnd)
							otherTT->fClock_End = earliestEnd;
						}
					}
				}

			if (size_t storedSize = storedIDs.size())
				fTS->SetTuples(storedSize, &storedIDs[0], &storedTuples[0]);
			}
		}

	ZMutexLocker lockerStructure2(fMutex_Structure);

	// Release our locks and TransTuples
	for (MapTransTuple_t::iterator
		iterTT = iTransaction->fTransTuples.begin(), theEnd = iTransaction->fTransTuples.end();
		iterTT != theEnd; ++iterTT)
		{
		TransTuple& currentTT = (*iterTT).second;
		if (currentTT.fRead || currentTT.fWritten)
			this->pReleaseLock(iTransaction, currentTT.fTupleInUse, currentTT.fWritten);
		this->pDisposeTransTuple(currentTT);
		}
	iTransaction->fTransTuples.clear();

	// And dispose the transaction.
	ZUtil_STL::sEraseMust(kDebug, fTransactions, iTransaction);
	lockerStructure2.Release();

	delete iTransaction;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Private

void ZTBRep_TS::pFetchTuples(Transaction* iTransaction,
	size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{
	ASSERTLOCKED(fTS->GetReadLock());

	ZMutexLocker lock(fMutex_Structure);

	// Increment iCount, so we can use pre-decrement in the loops.
	++iCount;

	if (oTuples)
		{
		while (--iCount)
			*oTuples++ = this->pGetTransTuple(iTransaction, *iIDs++).fValue_Current;
		}
	else
		{
		while (--iCount)
			this->pGetTransTuple(iTransaction, *iIDs++);
		}
	}

/**This variant takes a set of IDs, and is called by TransSearch to mark the tuples as being read,
without having to copy the IDs into a temporary vector before passing it to the other variant.*/
void ZTBRep_TS::pFetchTuples(Transaction* iTransaction, const set<uint64>& iIDs)
	{
	ASSERTLOCKED(fTS->GetReadLock());

	ZMutexLocker lock(fMutex_Structure);

	for (set<uint64>::const_iterator iterID = iIDs.begin(), theEnd = iIDs.end();
		iterID != theEnd; ++iterID)
		{
		this->pGetTransTuple(iTransaction, *iterID);
		}
	}

void ZTBRep_TS::pTryValidation(Transaction* iTransaction)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTLOCKED(fMutex_Structure);

	while (iTransaction->fIter_Validate != iTransaction->fTransTuples.end())
		{
		TransTuple& currentTT = (*iTransaction->fIter_Validate).second;
		if (currentTT.fWritten || currentTT.fRead)
			{
			if (!this->pAcquireLockOrQueue(iTransaction, currentTT.fTupleInUse, currentTT.fWritten))
				return;
			}
		++iTransaction->fIter_Validate;
		}

	// When we get here we've locked everything, otherwise we would have return-ed already.

	// Check that all our reads have an intersection.
	uint64 latestStart;
	uint64 earliestEnd = fClock;
	spLatestStartEarliestEnd(iTransaction->fTransTuples, latestStart, earliestEnd);

	if (latestStart > earliestEnd)
		{
		// We've failed. Release our locks and TransTuples
		for (MapTransTuple_t::iterator
			iterTT = iTransaction->fTransTuples.begin(), theEnd = iTransaction->fTransTuples.end();
			iterTT != theEnd; ++iterTT)
			{
			TransTuple& currentTT = (*iterTT).second;
			if (currentTT.fWritten || currentTT.fRead)
				this->pReleaseLock(iTransaction, currentTT.fTupleInUse, currentTT.fWritten);
			this->pDisposeTransTuple(currentTT);
			}
		iTransaction->fTransTuples.clear();

		// Make the callback
		if (iTransaction->fCallback_Validate)
			iTransaction->fCallback_Validate(false, iTransaction->fRefcon);
		}
	else
		{
		// We've succeeded.
		if (iTransaction->fCallback_Validate)
			iTransaction->fCallback_Validate(true, iTransaction->fRefcon);
		}
	}

bool ZTBRep_TS::pAcquireLockOrQueue(Transaction* iTransaction, TupleInUse& iTIU, bool iWrite)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (iTIU.fTransaction_Waiting)
		{
		// Other transactions are already waiting.
		if (not iWrite)
			{
			// We're a reader. Try to find another one already in the queue.
			Transaction* current = iTIU.fTransaction_Waiting;
			for (;;)
				{
				TransTuple& waitingTT = (*current->fIter_Validate).second;
				if (!waitingTT.fWritten)
					{
					// Found one, attach ourselves to it.
					iTransaction->fWaiting_NextReader = current->fWaiting_NextReader;
					current->fWaiting_NextReader = iTransaction;
					return false;
					}
				current = current->fWaiting_Next;
				if (current == iTIU.fTransaction_Waiting)
					{
					// We've returned to the beginning of the list.
					break;
					}
				}
			// Didn't find a reader, so drop through
			}
		// Join the back of the queue by inserting ourselves after fTransaction_Waiting->fPrev.
		iTransaction->fWaiting_Next = iTIU.fTransaction_Waiting;
		iTransaction->fWaiting_Prev = iTIU.fTransaction_Waiting->fWaiting_Prev;
		iTransaction->fWaiting_Prev->fWaiting_Next = iTransaction;
		iTransaction->fWaiting_Next->fWaiting_Prev = iTransaction;
		return false;
		}
	else
		{
		// There's nothing waiting.
		if (iTIU.fTransaction_Writer || (iWrite && iTIU.fTransaction_ReaderCount))
			{
			// But another transaction has the write lock or we want the write lock
			// and there are already readers. Establish the queue.
			iTIU.fTransaction_Waiting = iTransaction;
			iTransaction->fWaiting_Next = iTransaction;
			iTransaction->fWaiting_Prev = iTransaction;
			return false;
			}
		else if (iWrite)
			{
			// We acquire the write lock
			ZAssertStop(kDebug, !iTIU.fTransaction_ReaderCount);
			iTIU.fTransaction_Writer = iTransaction;
			}
		else
			{
			// We acquire the read lock.
			++iTIU.fTransaction_ReaderCount;
			}
		return true;
		}
	}

void ZTBRep_TS::pReleaseLock(Transaction* iTransaction, TupleInUse& iTIU, bool iWrite)
	{
	ASSERTUNLOCKED(fTS->GetReadLock());
	ASSERTLOCKED(fMutex_Structure);

	bool releaseWaiting = false;
	if (iWrite)
		{
		// Release the write lock
		ZAssertStop(kDebug, iTIU.fTransaction_Writer == iTransaction);
		ZAssertStop(kDebug, iTIU.fTransaction_ReaderCount == 0);
		iTIU.fTransaction_Writer = nullptr;
		releaseWaiting = true;
		}
	else
		{
		// Release the write lock
		ZAssertStop(kDebug, iTIU.fTransaction_Writer == nullptr);
		ZAssertStop(kDebug, iTIU.fTransaction_ReaderCount != 0);
		if (--iTIU.fTransaction_ReaderCount == 0)
			releaseWaiting = true;
		}

	if (releaseWaiting)
		{
		ZAssertStop(kDebug, iTIU.fTransaction_Writer == nullptr);
		ZAssertStop(kDebug, iTIU.fTransaction_ReaderCount == 0);

		if (Transaction* waitingTransaction = iTIU.fTransaction_Waiting)
			{
			Transaction* nextWaitingTransaction = waitingTransaction->fWaiting_Next;
			if (nextWaitingTransaction == waitingTransaction)
				{
				// This is the last in the list.
				iTIU.fTransaction_Waiting = nullptr;
				}
			else
				{
				// Unlink from the circular list.
				nextWaitingTransaction->fWaiting_Prev = waitingTransaction->fWaiting_Prev;
				waitingTransaction->fWaiting_Prev->fWaiting_Next = nextWaitingTransaction;
				iTIU.fTransaction_Waiting = nextWaitingTransaction;
				}

			waitingTransaction->fWaiting_Prev = nullptr;
			waitingTransaction->fWaiting_Next = nullptr;

			// Look at the TransTuple that waitingTransaction is using to lock iTIU.
			TransTuple& waitingTT = (*waitingTransaction->fIter_Validate).second;

			if (waitingTT.fWritten)
				{
				// The transaction that's waiting needs the write lock.
				iTIU.fTransaction_Writer = waitingTransaction;

				// Move waitingTransaction's iterator along to the next tuple.
				++waitingTransaction->fIter_Validate;

				// and try to finish waitingTransaction's validation.
				this->pTryValidation(waitingTransaction);
				}
			else
				{
				ZAssertStop(kDebug, waitingTT.fRead);

				// The transaction that's waiting needs the read lock. And there may be
				// additional transactions queued up being it.
				while (waitingTransaction)
					{
					// We've got another reader
					++iTIU.fTransaction_ReaderCount;

					// Move waitingTransaction's iterator along to the next tuple.
					++waitingTransaction->fIter_Validate;

					// Remember the next reader, and unlink from it.
					Transaction* nextWaitingReader = waitingTransaction->fWaiting_NextReader;
					waitingTransaction->fWaiting_NextReader = nullptr;

					// Try to finish waitingTransaction's validation.
					this->pTryValidation(waitingTransaction);

					// Move on to the next reader.
					waitingTransaction = nextWaitingReader;
					}
				}
			}
		}
	}

ZTBRep_TS::TransTuple& ZTBRep_TS::pGetTransTuple(Transaction* iTransaction, uint64 iID)
	{
	// We don't require that fMutex_Structure be locked to get an extant TransTuple
	// from iTransaction->fTransTuples, but we do for ones which have not been
	// created yet. The overhead of acquiring/releasing the lock here can be significant,
	// so we require that our caller have done it already.
	ASSERTLOCKED(fMutex_Structure);

	MapTransTuple_t::iterator position = iTransaction->fTransTuples.lower_bound(iID);
	if (position != iTransaction->fTransTuples.end() && (*position).first == iID)
		return (*position).second;

	TupleInUse& theTupleInUse = this->pGetTupleInUse(iID);

	if (!theTupleInUse.fClock_LastWritten)
		{
		// It's a fresh TIU, so we need to get its value from the TS and record a clock for it.
		ZTuple theValue = fTS->GetTuple(iID);

		theTupleInUse.fValue = theValue;
		theTupleInUse.fClock_LastWritten = fClock;
		}

	return this->pAllocateTransTuple(&position, iTransaction, theTupleInUse,
		theTupleInUse.fValue, theTupleInUse.fClock_LastWritten, false);
	}

ZTBRep_TS::TransTuple& ZTBRep_TS::pAllocateTransTuple(const void* iPosition,
	Transaction* iTransaction, TupleInUse& iTIU,
	const ZTuple& iValue, uint64 iClockStart, bool iWritten)
	{
	const MapTransTuple_t::iterator* thePosition =
		static_cast<const MapTransTuple_t::iterator*>(iPosition);

	ASSERTLOCKED(fMutex_Structure);

	TransTuple& theTransTuple =
		iTransaction->fTransTuples.insert(*thePosition, pair<uint64, TransTuple>(iTIU.fID,
		TransTuple(iTIU, iValue, iClockStart, iWritten)))->second;

	sInsertBackMust(iTIU.fUsingTransTuples, &theTransTuple);
	return theTransTuple;
	}

void ZTBRep_TS::pDisposeTransTuple(TransTuple& iTransTuple)
	{
	ASSERTLOCKED(fMutex_Structure);

	TupleInUse& theTupleInUse = iTransTuple.fTupleInUse;

	sEraseMust(theTupleInUse.fUsingTransTuples, &iTransTuple);

	this->pReleaseTupleInUse(theTupleInUse);
	}

ZTBRep_TS::TupleInUse& ZTBRep_TS::pGetTupleInUse(uint64 iID)
	{
	ASSERTLOCKED(fMutex_Structure);

	MapTupleInUse_t::iterator position = fTuplesInUse.lower_bound(iID);
	if (position != fTuplesInUse.end() && (*position).first == iID)
		{
		return (*position).second;
		}
	else
		{
		// This tuple is not in use by any other transaction.
		TupleInUse& theTIU = fTuplesInUse.insert(
			position, MapTupleInUse_t::value_type(iID, TupleInUse()))->second;

		theTIU.fID = iID;
		theTIU.fTransaction_Writer = nullptr;
		theTIU.fTransaction_ReaderCount = 0;
		theTIU.fTransaction_Waiting = nullptr;
		// A zero value for fClock_LastWritten indicates that theTIU.fValue
		// is invalid -- it will have to be initialized from the TS if anything
		// ever needs to read it.
		theTIU.fClock_LastWritten = 0;

		return theTIU;
		}
	}

void ZTBRep_TS::pReleaseTupleInUse(TupleInUse& iTupleInUse)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (iTupleInUse.fUsingTransTuples)
		return;

	sEraseMust(kDebug, fTuplesInUse, iTupleInUse.fID);
	}

} // namespace ZooLib
