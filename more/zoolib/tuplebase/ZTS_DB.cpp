/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTS_DB.h"

#if ZCONFIG_API_Enabled(TS_DB)

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStream_Memory.h"
#include "zoolib/ZUtil_STL.h"

#include <fcntl.h> // For O_RDWR etc

namespace ZooLib {

using std::exception;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

#define kDebug_TS_DB 1

#ifndef ZCONFIG_TS_DB_UsePrefix
#	define ZCONFIG_TS_DB_UsePrefix 0
#endif

/*
ZTS_DB uses a single Berkeley DB instance to store mappings from
IDs to tuples, and from tuplevalues to IDs.

Keys come in two forms, the first byte of the key differentiating them.

If the first byte is zero, then the key is nine bytes in length and
the last eight bytes contain a little-endian tuple ID. The data
stored under such a key is the result of calling ToStream on
the tuple to be stored at that ID.

As a special case, if the ID is zero then the stored data is the meta
data for the database, consisting of a version number (zero currently),
the next unused ID, and the list of property names for each index.

If the first byte is non-zero then its value indicates that this is an
entry in the index with the corresponding index ID. The key is of
variable length, with the last eight bytes being a tuple ID, and the
intervening bytes containing a list of streamed tuplevalues. No actual
data is stored under such a key, the ID is what we're interested in and
it's available from the key itself.

For alignment purposes it would be better if the ID were placed in the
first eight bytes, but then we cannot use prefix compression. That said,
prefix compression is disabled by default because I'm only seeing about
5% reduction in file size.

Sorting is by key type (the first byte), then by any tuplevalues, and
finally by ID.
*/

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace { // anonymous
struct TypeID_t
	{
	TypeID_t(uint8 iType, const uint64& iID);
	uint8 fBuffer[9];
	};

inline TypeID_t::TypeID_t(uint8 iType, const uint64& iID)
	{
	fBuffer[0] = iType;
	*reinterpret_cast<uint64*>(&fBuffer[1]) = iID;
	ZByteSwap_HostToLittle64(reinterpret_cast<uint64*>(&fBuffer[1]));
	}
} // anonymous namespace

static inline void spPackTypeID(const TypeID_t& iTI, DBT& oDBT)
	{
	oDBT.data = const_cast<uint8*>(iTI.fBuffer);
	oDBT.size = 9;
	}

static inline uint8 spUnpackType(const DBT& iDBT)
	{
	return static_cast<const uint8*>(iDBT.data)[0];
	}

static inline uint64 spUnpackID(const void* iAddress)
	{
	uint64 result = *static_cast<const uint64*>(iAddress);
	ZByteSwap_LittleToHost64(&result);
	return result;
	}

static inline uint64 spUnpackID(const DBT& iDBT)
	{
	return spUnpackID(static_cast<const uint8*>(iDBT.data) + iDBT.size - 8);
	}

namespace { // anonymous
class DBT_Tuple : public DBT
	{
public:
	DBT_Tuple(const ZTuple& iTuple);
	ZMemoryBlock fMB;
	};

DBT_Tuple::DBT_Tuple(const ZTuple& iTuple)
	{
	iTuple.ToStream(ZStreamRWPos_MemoryBlock(fMB, 1024));
	data = fMB.GetData();
	size = fMB.GetSize();
	}
} // anonymous namespace

static inline ZTuple spUnpackTuple(const DBT& iDBT)
	{
	return ZTuple(ZStreamRPos_Memory(iDBT.data, iDBT.size));
	}

static inline int spCompareInc(const uint8*& ioLeft, const uint8*& ioRight)
	{
	// It would be feasible to do an optimized implementation that
	// does not unpack a tuplevalue from either left or right. Punt
	// on that for now.

	ZStreamR_Memory leftStream(ioLeft);
	const ZTValue leftValue(leftStream);
	ioLeft = static_cast<const uint8*>(leftStream.GetCurrentAddress());

	ZStreamR_Memory rightStream(ioRight);
	const ZTValue rightValue(rightStream);
	ioRight = static_cast<const uint8*>(rightStream.GetCurrentAddress());

	return leftValue.Compare(rightValue);
	}

static int spCompareIndex(const uint8*& ioLeftCur, const uint8* iLeftEnd,
	const uint8*& ioRightCur, const uint8* iRightEnd)
	{
	while (ioLeftCur < iLeftEnd && ioRightCur < iRightEnd)
		{
		if (int compare = spCompareInc(ioLeftCur, ioRightCur))
			return compare;

		// And their values match.
		}

	if (ioRightCur < iRightEnd)
		{
		// Still have data in right, but none in left, so left is less.
		// If we're doing a lower_bound search (key >= stored) then
		// this is the right answer.
		// If we're doing an upper_bound search (key > stored) then
		// the ID in iLeftData will be kMaxID, and left should be
		// considered to be greater.
		// Because kMaxID is all ones, we don't need to do little->host conversion.
		if (ZTupleIndex::kMaxID == *static_cast<const uint64*>(static_cast<const void*>(ioLeftCur)))
			return 1;
		return -1;
		}

	// Left is a search key, so it can never be longer than the stored key.
	// However, that's only the case if we're not using prefix compression.
	ZAssertStop(kDebug_TS_DB, ZCONFIG_TS_DB_UsePrefix || ioLeftCur == iLeftEnd);

	// Our values all match.
	return 0;
	}

static int spCompareIndex(const void* iLeftData, size_t iLeftSize,
	const void* iRightData, size_t iRightSize)
	{
	const uint8* leftData = static_cast<const uint8*>(iLeftData);
	const uint8* leftCur = leftData + 1;
	const uint8* leftEnd = leftData + iLeftSize - 8;

	const uint8* rightData = static_cast<const uint8*>(iRightData);
	const uint8* rightCur = rightData + 1;
	const uint8* rightEnd = rightData + iRightSize - 8;

	if (int compare = spCompareIndex(leftCur, leftEnd, rightCur, rightEnd))
		return compare;

	// Left and right are the same length, and match exactly,
	// so our tie-breaker is the IDs.
	uint64 leftID = *static_cast<const uint64*>(static_cast<const void*>(leftEnd));
	ZByteSwap_LittleToHost64(&leftID);

	uint64 rightID = *static_cast<const uint64*>(static_cast<const void*>(rightEnd));
	ZByteSwap_LittleToHost64(&rightID);

	if (leftID < rightID)
		return -1;
	if (leftID > rightID)
		return 1;

	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_DB

/*
ZTS_DB::Index inherits from ZTupleIndex in order that it can be passed to
ZTupleIndex::sMatchIndices. At some point that may get refactored or templatized
in some fashion in which case ZTS_DB::Index may become a simple list of
property names in ZTS_DB.
*/

class ZTS_DB::Index : public ZTupleIndex
	{
public:
	Index(ZTS_DB* iTS, uint8 iIndexID, const ZStreamR& iStreamR);
	Index(ZTS_DB* iTS, uint8 iIndexID, const vector<string>& iPropNames);

// From ZTupleIndex
	virtual void Insert(uint64 iID, const ZTuple* iTuple);

	virtual void Erase(uint64 iID, const ZTuple* iTuple);

	virtual void Find(const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs);

	virtual size_t CanHandle(const ZTBSpec::CriterionSect& iCriterionSect);

// Our protocol
	void ToStream(const ZStreamW& iStreamW);

	void Insert(uint64 iID, const ZTuple& iTuple);
	void Erase(uint64 iID, const ZTuple& iTuple);
	void Update(uint64 iID, const ZTuple& iOldTuple, const ZTuple& iNewTuple);

private:
	ZMemoryBlock pKeyFromTuple(uint64 iID, const ZTuple& iTuple);

	ZTS_DB* fTS;
	uint8 fIndexID;
	size_t fSize;
	vector<ZTName> fPropNames;
	};

ZTS_DB::Index::Index(ZTS_DB* iTS, uint8 iIndexID, const ZStreamR& iStreamR)
:	fTS(iTS),
	fIndexID(iIndexID)
	{
	fSize = iStreamR.ReadUInt32();
	size_t count = iStreamR.ReadUInt8();
	for (size_t x = 0; x < count; ++x)
		fPropNames.push_back(ZTName(iStreamR));
	}

ZTS_DB::Index::Index(ZTS_DB* iTS, uint8 iIndexID, const vector<string>& iPropNames)
:	fTS(iTS),
	fIndexID(iIndexID),
	fSize(0),
	fPropNames(iPropNames.begin(), iPropNames.end())
	{}

void ZTS_DB::Index::Insert(uint64 iID, const ZTuple* iTuple)
	{
	ZUnimplemented();
	}

void ZTS_DB::Index::Erase(uint64 iID, const ZTuple* iTuple)
	{
	ZUnimplemented();
	}

void ZTS_DB::Index::Find(const std::set<uint64>& iSkipIDs,
	std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs)
	{
	ZMemoryBlock theKeyMB;
	ZStreamRWPos_MemoryBlock theKeyStream(theKeyMB, 1024);
	theKeyStream.WriteUInt8(fIndexID);

	DB* theDB = fTS->fDB;

	const ZTValue* valueEqual;
	const ZTValue* bestValueLess;
	const ZTValue* bestValueLessEqual;
	const ZTValue* bestValueGreater;
	const ZTValue* bestValueGreaterEqual;

	// Walk through our properties, looking for and removing entries in
	// ioCriteria where where the effective constraint is equivalence.
	for (vector<ZTName>::const_iterator propNameIter = fPropNames.begin();
		propNameIter != fPropNames.end(); ++propNameIter)
		{
		if (!sGatherMergeConstraints(*propNameIter, ioCriteria,
			valueEqual,
			bestValueLess, bestValueLessEqual, bestValueGreater, bestValueGreaterEqual))
			{
			// We found a contradiction, so we have completed the search, with no results.
			return;
			}

		if (!valueEqual)
			break;

		ZAssert(!bestValueLess && !bestValueLessEqual
			&& !bestValueGreater && !bestValueGreaterEqual);
		valueEqual->ToStream(theKeyStream);
		}

	const uint64 endOfEqualValues = theKeyStream.GetPosition();
	ZAssertStop(kDebug_TS_DB, !bestValueGreater || !bestValueGreaterEqual);
	ZAssertStop(kDebug_TS_DB, !bestValueLess || !bestValueLessEqual);

	// Comments in ZTupleIndex_T::Key::operator< have relevance to
	// how we handle searching for keys that are shorter than
	// stored keys.

	// DB search is equivalent to lower_bound, so spCompareIndex
	// has to handle edge cases specially to simulate upper_bound.
	// I suppose we could swap in a different comparison function
	// for this operation.
	if (bestValueGreater)
		{
		bestValueGreater->ToStream(theKeyStream);
		theKeyStream.WriteUInt64LE(kMaxID);
		}
	else
		{
		if (bestValueGreaterEqual)
			bestValueGreaterEqual->ToStream(theKeyStream);
		theKeyStream.WriteUInt64LE(0);
		}

	DBT theKey;
	theKey.data = theKeyMB.GetData();
	theKey.size = theKeyStream.GetPosition();

	DBT theData;
	if (0 != theDB->seq(theDB, &theKey, &theData, R_CURSOR))
		return;

	theKeyStream.SetPosition(endOfEqualValues);
	if (bestValueLess)
		{
		bestValueLess->ToStream(theKeyStream);
		theKeyStream.WriteUInt64LE(0);
		}
	else
		{
		if (bestValueLessEqual)
			bestValueLessEqual->ToStream(theKeyStream);
		theKeyStream.WriteUInt64LE(kMaxID);
		}

	const void* limitData = theKeyMB.GetData();
	const size_t limitSize = theKeyStream.GetPosition();
	for (;;)
		{
		// Bail if we've moved out of this index's nodes.
		if (fIndexID != spUnpackType(theKey))
			break;

		// Verify that theKey is less (or less equal) than the limit.
		int compare = spCompareIndex(limitData, limitSize, theKey.data, theKey.size);
		if (bestValueLess)
			{
			if (compare <= 0)
				{
				// limit <= theKey, so !(theKey < limit)
				break;
				}
			}
		else
			{
			if (compare < 0)
				{
				// limit < theKey, so !(theKey <= limit)
				break;
				}
			}
		// The tuple ID is in the key -- we don't actually store data for it.
		const uint64 theID = spUnpackID(theKey);
		oIDs.push_back(theID);
		if (0 != theDB->seq(theDB, &theKey, &theData, R_NEXT))
			break;
		}
	}

size_t ZTS_DB::Index::CanHandle(const ZTBSpec::CriterionSect& iCriterionSect)
	{
	// This is pretty much copied from ZTupleIndex_General, with mods for
	// the handling of our current size and list of property names.

	vector<const ZTBSpec::Criterion*> remainingCriteria;
	remainingCriteria.reserve(iCriterionSect.size());
	for (ZTBSpec::CriterionSect::const_iterator i = iCriterionSect.begin(),
		theEnd = iCriterionSect.end();
		i != theEnd; ++i)
		{
		remainingCriteria.push_back(&(*i));
		}

	size_t likelyResultSize = fSize;

	size_t propertyCount = 0;

	const ZTValue* valueEqual;
	const ZTValue* bestValueLess;
	const ZTValue* bestValueLessEqual;
	const ZTValue* bestValueGreater;
	const ZTValue* bestValueGreaterEqual;

	// Walk through our properties, looking for and removing entries in
	// remainingCriteria where where the effective constraint is equivalence.
	for (/*no init*/; propertyCount < fPropNames.size(); ++propertyCount)
		{
		if (!sGatherMergeConstraints(fPropNames[propertyCount], remainingCriteria,
			valueEqual,
			bestValueLess, bestValueLessEqual, bestValueGreater, bestValueGreaterEqual))
			{
			// We found a contradiction, so we can trivially handle this search.
			return 1;
			}

		if (!valueEqual)
			break;

		ZAssertStop(kDebug_TS_DB, !bestValueLess && !bestValueLessEqual
			&& !bestValueGreater && !bestValueGreaterEqual);

		likelyResultSize /= 2;
		}

	if (propertyCount != fPropNames.size())
		{
		// We still have properties on which we index not referenced by the criteria we've seen.
		if (bestValueLess || bestValueLessEqual || bestValueGreater || bestValueGreaterEqual)
			{
			// We at least had a range check on the last item, so move on.
			likelyResultSize /= 2;
			++propertyCount;
			}
		}

	if (!propertyCount)
		{
		// We didn't see any matching property names at all, so we can't help.
		return 0;
		}

	// Add 1, so we can't return zero.
	return likelyResultSize + 1;
	}

void ZTS_DB::Index::ToStream(const ZStreamW& iStreamW)
	{
	iStreamW.WriteUInt32(fSize);
	iStreamW.WriteUInt8(fPropNames.size());
	for (vector<ZTName>::const_iterator i = fPropNames.begin();
		i != fPropNames.end(); ++i)
		{
		(*i).ToStream(iStreamW);
		}
	}

static const DBT spNilData = { 0, 0 };

void ZTS_DB::Index::Insert(uint64 iID, const ZTuple& iTuple)
	{
	if (ZMemoryBlock theMB = this->pKeyFromTuple(iID, iTuple))
		{
		DBT theKey;
		theKey.data = theMB.GetData();
		theKey.size = theMB.GetSize();
		fTS->fDB->put(fTS->fDB, &theKey, &spNilData, 0);
		++fSize;
		}
	}

void ZTS_DB::Index::Erase(uint64 iID, const ZTuple& iTuple)
	{
	if (ZMemoryBlock theMB = this->pKeyFromTuple(iID, iTuple))
		{
		DBT theDBT;
		theDBT.data = theMB.GetData();
		theDBT.size = theMB.GetSize();
		fTS->fDB->del(fTS->fDB, &theDBT, 0);
		ZAssertStop(kDebug_TS_DB, fSize);
		--fSize;
		}
	}

void ZTS_DB::Index::Update(uint64 iID, const ZTuple& iOldTuple, const ZTuple& iNewTuple)
	{
	ZMemoryBlock theOldMB = this->pKeyFromTuple(iID, iOldTuple);
	ZMemoryBlock theNewMB = this->pKeyFromTuple(iID, iNewTuple);
	if (theOldMB == theNewMB)
		return;

	// The keys are different, so iOldTuple and iNewTuple differ
	// in a way that is significant to this index.
	if (theOldMB)
		{
		DBT theDBT;
		theDBT.data = theOldMB.GetData();
		theDBT.size = theOldMB.GetSize();
		fTS->fDB->del(fTS->fDB, &theDBT, 0);
		ZAssertStop(kDebug_TS_DB, fSize);
		--fSize;
		}

	if (theNewMB)
		{
		DBT theKey;
		theKey.data = theNewMB.GetData();
		theKey.size = theNewMB.GetSize();
		fTS->fDB->put(fTS->fDB, &theKey, &spNilData, 0);
		++fSize;
		}
	}

ZMemoryBlock ZTS_DB::Index::pKeyFromTuple(uint64 iID, const ZTuple& iTuple)
	{
	ZMemoryBlock theMB;
	vector<ZTName>::iterator nameIter = fPropNames.begin();
	ZTuple::const_iterator tupleIter = iTuple.IteratorOf(*nameIter++);
	if (tupleIter != iTuple.end())
		{
		ZStreamRWPos_MemoryBlock theStream(theMB, 1024);
		theStream.WriteUInt8(fIndexID);
		tupleIter->fVal.ToStream(theStream);
		while (nameIter != fPropNames.end())
			iTuple.Get(*nameIter++).ToStream(theStream);
		theStream.WriteUInt64LE(iID);
		}
	return theMB;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_DB

ZTS_DB::ZTS_DB(const ZFileSpec& iFileSpec)
:	fMutex("ZTS_DB::fMutex"),
	fChanged(false)
	{
	string dbfile = iFileSpec.AsString_Native();
	BTREEINFO theBTI;
	theBTI.flags = 0;
	theBTI.cachesize = 0; // use default
	theBTI.maxkeypage = 0; // use default
	theBTI.minkeypage = 0; // use default
	theBTI.psize = 0; // use default
	theBTI.compare = spBTree_Compare;

	if (ZCONFIG_TS_DB_UsePrefix)
		theBTI.prefix = spBTree_Prefix;
	else
		theBTI.prefix = nullptr;

	theBTI.lorder = 0; // use default
	fDB = ::dbopen(dbfile.c_str(), O_RDWR, 0, DB_BTREE, &theBTI);
	if (!fDB)
		throw runtime_error("ZTS_DB, couldn't open DB file");

	uint8 zeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	DBT theKey;
	theKey.data = zeroes;
	theKey.size = 9;
	DBT theData;
	if (0 != fDB->get(fDB, &theKey, &theData, 0))
		throw runtime_error("ZTS_DB, no meta node");

	try
		{
		ZStreamRPos_Memory theStream(theData.data, theData.size);
		if (0 != theStream.ReadUInt32())
			throw runtime_error("ZTS_DB, bad version in meta node");

		fNextUnusedID = theStream.ReadUInt64();
		size_t count = theStream.ReadUInt8();
		for (size_t indexID = 1; indexID <= count; ++indexID)
			fIndices.push_back(new Index(this, indexID, theStream));
		}
	catch (exception& ex)
		{
		throw runtime_error(string("ZTS_DB, caught exception: ") + ex.what());
		}
	}

ZTS_DB::ZTS_DB(const ZFileSpec& iFileSpec, const vector<vector<string> >& iIndexSpecs)
:	fMutex("ZTS_DB::fMutex"),
	fNextUnusedID(1),
	fChanged(true)
	{
	uint8 indexID = 1;
	for (vector<vector<string> >::const_iterator i = iIndexSpecs.begin();
		i != iIndexSpecs.end(); ++i)
		{
		fIndices.push_back(new Index(this, indexID++, *i));
		}

	string dbfile = iFileSpec.AsString_Native();
	BTREEINFO theBTI;
	theBTI.flags = 0;
	theBTI.cachesize = 0; // use default
	theBTI.maxkeypage = 0; // use default
	theBTI.minkeypage = 0; // use default
	theBTI.psize = 0; // use default
	theBTI.compare = spBTree_Compare;

	if (ZCONFIG_TS_DB_UsePrefix)
		theBTI.prefix = spBTree_Prefix;
	else
		theBTI.prefix = nullptr;

	theBTI.lorder = 0; // use default
	fDB = ::dbopen(dbfile.c_str(), O_RDWR | O_CREAT, 0644, DB_BTREE, &theBTI);
	if (!fDB)
		throw runtime_error("ZTS_DB, couldn't create DB file");

	this->pFlush();
	}

ZTS_DB::~ZTS_DB()
	{
	this->pFlush();
	ZUtil_STL::sDeleteAll(fIndices.begin(), fIndices.end());
	fDB->close(fDB);
	}

void ZTS_DB::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount)
	{
	ZMutexLocker lock(fMutex_ID);
	oBaseID = fNextUnusedID;
	oCount = iCount;
	fNextUnusedID += iCount;
	if (iCount)
		fChanged = true;
	}

void ZTS_DB::SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	ZAssertStop(kDebug_TS_DB, fMutex.IsLocked());

	if (iCount)
		fChanged = true;

	while (iCount--)
		{
		int result;
		uint64 theID = *iIDs++;
		const ZTuple& newTuple = *iTuples++;
		if (!theID)
			{
			// Skip any zero IDs that might have been passed.
			continue;
			}

		TypeID_t ti(0, theID);
		DBT theKey;
		spPackTypeID(ti, theKey);
		DBT theData;
		if (0 == fDB->seq(fDB, &theKey, &theData, R_CURSOR)
			&& 0 == spUnpackType(theKey)
			&& theID == spUnpackID(theKey))
			{
			// We had it before.
			const ZTuple oldTuple = spUnpackTuple(theData);
			if (newTuple)
				{
				// And we have a new value, so replace the stored tuple.
				DBT_Tuple theDBT_Tuple(newTuple);
				result = fDB->put(fDB, &theKey, &theDBT_Tuple, R_CURSOR);

				// Update the indices
				for (vector<ZTupleIndex*>::iterator i = fIndices.begin(); i != fIndices.end(); ++i)
					static_cast<Index*>(*i)->Update(theID, oldTuple, newTuple);
				}
			else
				{
				// The new value is empty, so clear theKey.
				fDB->del(fDB, &theKey, R_CURSOR);
				// And remove the old value from indices.
				for (vector<ZTupleIndex*>::iterator i = fIndices.begin(); i != fIndices.end(); ++i)
					static_cast<Index*>(*i)->Erase(theID, oldTuple);
				}
			}
		else
			{
			// Didn't have it previously.
			spPackTypeID(ti, theKey);
			DBT_Tuple theDBT_Tuple(newTuple);
			result = fDB->put(fDB, &theKey, &theDBT_Tuple, 0);

			for (vector<ZTupleIndex*>::iterator i = fIndices.begin(); i != fIndices.end(); ++i)
				static_cast<Index*>(*i)->Insert(theID, newTuple);
			}
		}
	}

void ZTS_DB::GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{
	ZAssertStop(kDebug_TS_DB, fMutex.IsLocked());

	while (iCount--)
		{
		if (!this->pGetTuple(*iIDs++, *oTuples))
			oTuples->Clear();
		++oTuples;
		}
	}

void ZTS_DB::Search(const ZTBSpec& iSpec, const set<uint64>& iSkipIDs, set<uint64>& oIDs)
	{
	ZAssertStop(kDebug_TS_DB, fMutex.IsLocked());
	ZAssertStop(kDebug_TS_DB, oIDs.empty());

	// Find the best index to satisfy iSpec.
	const ZTBSpec::CriterionUnion& theCriterionUnion = iSpec.GetCriterionUnion();
	vector<ZTupleIndex*> indicesToUse;
	if (not ZTupleIndex::sMatchIndices(theCriterionUnion, fIndices, indicesToUse))
		{
		if (iSpec.IsAny())
			{
			ZDebugLogf(0, ("ZTS_DB::Search was passed an 'any' ZTBSpec, "
				"which would produce 2^64 IDs. Returning none instead"));
			return;
			}

		// At least one of iSpec's OR clauses could not be accelerated by
		// an index, so we're going to have to walk every tuple for that
		// clause, so we might as well do so for all of them.

		// Look for the first key of type zero, ID >= 1
		TypeID_t ti(0, 1);
		DBT theKey;
		spPackTypeID(ti, theKey);
		DBT theData;
		if (0 == fDB->seq(fDB, &theKey, &theData, R_FIRST))
			{
			for (;;)
				{
				if (0 != spUnpackType(theKey))
					{
					// We've moved past the end of the ID-->tuple keys.
					break;
					}

				const uint64 theID = spUnpackID(theKey);
				if (iSkipIDs.end() == iSkipIDs.find(theID))
					{
					ZTuple theTuple = spUnpackTuple(theData);
					if (iSpec.Matches(theTuple))
						oIDs.insert(theID);
					}

				if (0 != fDB->seq(fDB, &theKey, &theData, R_NEXT))
					{
					// We've run off the end of the tree.
					break;
					}
				}
			}
		}
	else
		{
		// We found an index for each clause of iSpec. So now use them.
		ZTBSpec::CriterionUnion::const_iterator iterCriterionUnion = theCriterionUnion.begin();
		vector<ZTupleIndex*>::const_iterator iterIndex = indicesToUse.begin();

		for (/*no init*/;iterIndex != indicesToUse.end(); ++iterIndex, ++iterCriterionUnion)
			{
			vector<const ZTBSpec::Criterion*> uncheckedCriteria;

			vector<uint64> currentResults;
			currentResults.reserve(100);

			(*iterIndex)->Find(*iterCriterionUnion, iSkipIDs, uncheckedCriteria, currentResults);

			if (uncheckedCriteria.empty())
				{
				oIDs.insert(currentResults.begin(), currentResults.end());
				}
			else
				{
				vector<const ZTBSpec::Criterion*>::const_iterator critBegin = uncheckedCriteria.begin();
				vector<const ZTBSpec::Criterion*>::const_iterator critEnd = uncheckedCriteria.end();
				for (vector<uint64>::iterator resultIter = currentResults.begin();
					resultIter != currentResults.end(); /*no increment*/)
					{
					uint64 currentID = *resultIter++;
					ZTuple theTuple;
					bool gotIt = this->pGetTuple(currentID, theTuple);
					ZAssertStop(kDebug_TS_DB, gotIt);
					bool allOkay = true;
					for (vector<const ZTBSpec::Criterion*>::const_iterator critIter = critBegin;
						allOkay && critIter != critEnd; ++critIter)
						{
						allOkay = (*critIter)->Matches(theTuple);
						}

					if (allOkay)
						oIDs.insert(currentID);
					}
				}
			}
		}
	}

ZMutexBase& ZTS_DB::GetReadLock()
	{ return fMutex; }

ZMutexBase& ZTS_DB::GetWriteLock()
	{ return fMutex; }

void ZTS_DB::Flush()
	{
	ZAssertStop(kDebug_TS_DB, fMutex.IsLocked());
	this->pFlush();
	}

bool ZTS_DB::pGetTuple(uint64 iID, ZTuple& oTuple)
	{
	if (iID)
		{
		TypeID_t ti(0, iID);
		DBT theKey;
		spPackTypeID(ti, theKey);
		DBT theData;
		if (0 == fDB->get(fDB, &theKey, &theData, 0))
			{
			oTuple = spUnpackTuple(theData);
			return true;
			}
		}
	return false;
	}

void ZTS_DB::pFlush()
	{
	if (!fChanged)
		return;

	fChanged = false;

	// Write the meta data into an MB.
	ZMemoryBlock theMB;
	{
	ZStreamRWPos_MemoryBlock theStream(theMB, 1024);
	theStream.WriteUInt32(0); // Version
	theStream.WriteUInt64(fNextUnusedID);
	theStream.WriteUInt8(fIndices.size());
	for (vector<ZTupleIndex*>::const_iterator i = fIndices.begin(); i != fIndices.end(); ++i)
		static_cast<Index*>(*i)->ToStream(theStream);
	}

	// The key under which it is stored is of type zero, ID zero.
	uint8 zeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	DBT theKey;
	theKey.data = zeroes;
	theKey.size = 9;

	DBT theData;
	theData.data = theMB.GetData();
	theData.size = theMB.GetSize();
	fDB->put(fDB, &theKey, &theData, 0);

	fDB->sync(fDB, 0);
	}

size_t ZTS_DB::spBTree_Prefix(const DBT* iLeft, const DBT* iRight)
	{
	ZAssert(ZCONFIG_TS_DB_UsePrefix);

	const int leftIndexID = spUnpackType(*iLeft);
	const int rightIndexID = spUnpackType(*iRight);
	if (leftIndexID != rightIndexID)
		return 1;

	if (0 == leftIndexID)
		return 9;

	const uint8* leftData = static_cast<const uint8*>(iLeft->data);
	const uint8* leftEnd = leftData + iLeft->size - 8;
	const uint8* leftCur = leftData + 1;

	const uint8* rightData = static_cast<const uint8*>(iRight->data);
	const uint8* rightCur = rightData + 1;
	const uint8* rightEnd = rightData + iRight->size - 8;

	if (0 != spCompareIndex(leftCur, leftEnd, rightCur, rightEnd))
		{
		// Not all our values match.
		return rightCur - rightData;
		}

	// Our values all match. We'll need to use the entirety of iRight
	// to determine that it's different from iLeft.
	return iRight->size;
	}

int ZTS_DB::spBTree_Compare(const DBT* iLeft, const DBT* iRight)
	{
	const int leftIndexID = spUnpackType(*iLeft);
	const int rightIndexID = spUnpackType(*iRight);

	if (const int compare = leftIndexID - rightIndexID)
		return compare;

	if (leftIndexID == 0)
		{
		const uint64 leftID = spUnpackID(static_cast<uint8*>(iLeft->data) + 1);
		const uint64 rightID = spUnpackID(static_cast<uint8*>(iRight->data) + 1);

		// It's an ID
		if (leftID < rightID)
			return -1;
		if (leftID > rightID)
			return 1;
		}
	else
		{
		return spCompareIndex(iLeft->data, iLeft->size, iRight->data, iRight->size);
		}
	return 0;
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TS_DB)
