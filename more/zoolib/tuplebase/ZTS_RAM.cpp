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

#include "zoolib/tuplebase/ZTS_RAM.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZStream.h"
#include "zoolib/tuplebase/ZTupleIndex.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_Strim.h"

using std::map;
using std::pair;
using std::runtime_error;
using std::set;
using std::vector;

namespace ZooLib {

#define kDebug_TS_RAM 2

#ifndef ZCONFIG_TS_RAM_Logging
#	define ZCONFIG_TS_RAM_Logging 0
#endif

#ifndef ZCONFIG_TS_RAM_WarnWhenScanningAllTuples
#	define ZCONFIG_TS_RAM_WarnWhenScanningAllTuples 1
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_RAM

ZTS_RAM::ZTS_RAM()
:	fRWLock("ZTS_RAM::fRWLock"),
	fNextUnusedID(1),
	fChanged(false)
	{}

ZTS_RAM::ZTS_RAM(const vector<ZRef<ZTupleIndexFactory> >& iIndexFactories)
:	fRWLock("ZTS_RAM::fRWLock"),
	fNextUnusedID(1),
	fChanged(false)
	{
	this->AddIndices(iIndexFactories);
	}

ZTS_RAM::ZTS_RAM(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories,
	uint64 iNextUnusedID, const map<uint64, ZTuple>& iTuples)
:	fRWLock("ZTS_RAM::fRWLock"),
	fNextUnusedID(iNextUnusedID),
	fTuples(iTuples),
	fChanged(false)
	{
	this->AddIndices(iIndexFactories);
	}

ZTS_RAM::ZTS_RAM(const std::vector<ZRef<ZTupleIndexFactory> >& iIndexFactories,
	uint64 iNextUnusedID, map<uint64, ZTuple>& ioTuples, bool iKnowWhatImDoing)
:	fRWLock("ZTS_RAM::fRWLock"),
	fNextUnusedID(iNextUnusedID),
	fChanged(false)
	{
	ZAssertStop(kDebug_TS_RAM, iKnowWhatImDoing);
	fTuples.swap(ioTuples);
	this->AddIndices(iIndexFactories);
	}

ZTS_RAM::~ZTS_RAM()
	{
	ZUtil_STL::sDeleteAll(fIndices.begin(), fIndices.end());
	}

void ZTS_RAM::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount)
	{
	ZMutexLocker lock(fMutex_ID);
	oBaseID = fNextUnusedID;
	oCount = iCount;
	fNextUnusedID += iCount;
	if (iCount)
		fChanged = true;
	}

void ZTS_RAM::SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanWrite());

	if (iCount)
		fChanged = true;

	while (iCount--)
		{
		const uint64 theID = *iIDs++;
		const ZTuple& newTuple = *iTuples++;

		if (0 == theID || fNextUnusedID <= theID)
			{
			if (ZLOG(s, eErr, "ZTS_RAM::SetTuples"))
				{
				s.Writef("Ignoring ID %llx, which is zero or lies beyond the allocated range (%llx)",
					theID, fNextUnusedID);
				}
			continue;
			}

		map<uint64, ZTuple>::iterator position = fTuples.lower_bound(theID);
		const bool hadIt = position != fTuples.end() && position->first == theID;

		if (hadIt)
			{
			// We had it before, so must remove it from indices.
			const ZTuple* oldTuple = &position->second;
			for (vector<ZTupleIndex*>::iterator i = fIndices.begin(); i != fIndices.end(); ++i)
				(*i)->Erase(theID, oldTuple);
			}

		if (not newTuple.IsEmpty())
			{
			// The new tuple is not empty, so it must be stored.
			if (hadIt)
				{
				// The old tuple was not empty, so we can just replace it.
				position->second = newTuple;
				}
			else
				{
				// The old tuple was unstored, but we know where it should be placed.
				position =
					fTuples.insert(position, map<uint64, ZTuple>::value_type(theID, newTuple));
				}
			// Enter the tuple in our indices.
			const ZTuple* storedTuple = &position->second;
			for (vector<ZTupleIndex*>::iterator i = fIndices.begin(); i != fIndices.end(); ++i)
				(*i)->Insert(theID, storedTuple);
			}
		else if (hadIt)
			{
			// The new tuple is empty, but the old one was not, so we remove the entry.
			fTuples.erase(position);
			}
		}
	}

void ZTS_RAM::GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanRead());

	while (iCount--)
		{
		map<uint64, ZTuple>::const_iterator iter = fTuples.find(*iIDs++);
		if (iter != fTuples.end())
			*oTuples++ = (*iter).second;
		else(
			*oTuples++).Clear();
		}
	}

void ZTS_RAM::Search(const ZTBSpec& iSpec, const set<uint64>& iSkipIDs, set<uint64>& oIDs)
	{
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanRead());
	ZAssertStop(kDebug_TS_RAM, oIDs.empty());

	#if ZCONFIG_TS_RAM_Logging
		ZTime startTime = ZTime::sSystem();
		ZLog::S s(ZLog::eInfo, "ZTS_RAM::Search");
		s << iSpec.AsTuple();
	#endif

	// Find the best index to satisfy iSpec.
	const ZTBSpec::CriterionUnion& theCriterionUnion = iSpec.GetCriterionUnion();
	vector<ZTupleIndex*> indicesToUse;
	if (not ZTupleIndex::sMatchIndices(theCriterionUnion, fIndices, indicesToUse))
		{
		if (ZCONFIG_TS_RAM_WarnWhenScanningAllTuples)
			{
			if (ZLOG(s, eWarning, "ZTS_RAM::Search"))
				{
//##				s << "In the spec:\n" << iSpec.AsTuple()
//##					<< "\nat least one clause does not have a usable index";
				}
			}

		#if ZCONFIG_TS_RAM_Logging
			size_t resultSize = 0;
		#endif

		if (iSpec.IsAny())
			{
			ZDebugLogf(0, ("ZTS_RAM::Search was passed an 'any' ZTBSpec, "
				"which would produce 2^64 IDs. Returning none instead"));
			return;
			}

		// At least one of iSpec's OR clauses could not be accelerated by
		// an index, so we're going to have to walk every tuple for that
		// clause, so we might as well do so for all of them.
		if (iSkipIDs.empty())
			{
			for (map<uint64, ZTuple>::iterator i = fTuples.begin(); i != fTuples.end(); ++i)
				{
				if (iSpec.Matches((*i).second))
					{
					#if ZCONFIG_TS_RAM_Logging
						++resultSize;
					#endif
					oIDs.insert((*i).first);
					}
				}
			}
		else
			{
			for (map<uint64, ZTuple>::iterator i = fTuples.begin(); i != fTuples.end(); ++i)
				{
				uint64 theID = (*i).first;
				if (iSkipIDs.end() == iSkipIDs.find(theID))
					{
					if (iSpec.Matches((*i).second))
						{
						#if ZCONFIG_TS_RAM_Logging
							++resultSize;
						#endif
						oIDs.insert(theID);
						}
					}
				}
			}
		#if ZCONFIG_TS_RAM_Logging
			s.Writef("\nResult size: %d, of %d, overall elapsed: %gms.",
				resultSize,
				fTuples.size(),
				1000*(ZTime::sSystem() - startTime));
		#endif
		}
	else
		{
		// We found an index for each clause of iSpec. So now use them.
		ZTBSpec::CriterionUnion::const_iterator iterCriterionUnion = theCriterionUnion.begin();
		vector<ZTupleIndex*>::const_iterator iterIndex = indicesToUse.begin();

		#if ZCONFIG_TS_RAM_Logging
			if (indicesToUse.size() > 1)
				s << "\nUsing multiple indices";
		#endif

		for (/*no init*/;iterIndex != indicesToUse.end(); ++iterIndex, ++iterCriterionUnion)
			{
			#if ZCONFIG_TS_RAM_Logging
				s << "\nSearching for criteria: ";
				for (ZTBSpec::CriterionSect::const_iterator i = (*iterCriterionUnion).begin();
					i != (*iterCriterionUnion).end(); ++i)
					{
					s << (*i).AsTuple() << " ";
					}

				s << "\nUsing index:\n";
				(*iterIndex)->WriteDescription(s);
				ZTime startOfThisSearch = ZTime::sSystem();
			#endif

			vector<const ZTBSpec::Criterion*> uncheckedCriteria;

			vector<uint64> currentResults;
			currentResults.reserve(100);

			(*iterIndex)->Find(*iterCriterionUnion, iSkipIDs, uncheckedCriteria, currentResults);

			if (uncheckedCriteria.empty())
				{
				#if ZCONFIG_TS_RAM_Logging
					s.Writef("\nResult size: %d, of %d, elapsed: %gms, no unchecked criteria.",
						currentResults.size(),
						fTuples.size(),
						1000 * (ZTime::sSystem() - startOfThisSearch));
				#endif

				oIDs.insert(currentResults.begin(), currentResults.end());
				}
			else
				{
				#if ZCONFIG_TS_RAM_Logging
					s.Writef("\nInitial result size: %d, Unchecked criteria: ",
						currentResults.size());
					for (vector<const ZTBSpec::Criterion*>::iterator i = uncheckedCriteria.begin();
						i != uncheckedCriteria.end(); ++i)
						{
						s << (*i)->AsTuple() << " ";
						}
					size_t finalResultSize = 0;
				#endif

				vector<const ZTBSpec::Criterion*>::const_iterator critBegin =
					uncheckedCriteria.begin();
				vector<const ZTBSpec::Criterion*>::const_iterator critEnd =
					uncheckedCriteria.end();

				for (vector<uint64>::iterator resultIter = currentResults.begin();
					resultIter != currentResults.end(); /*no increment*/)
					{
					uint64 currentID = *resultIter++;
					map<uint64, ZTuple>::iterator mapIter = fTuples.find(currentID);
					ZAssertStop(kDebug_TS_RAM, mapIter != fTuples.end());
					const ZTuple& theTuple = (*mapIter).second;
					bool allOkay = true;
					for (vector<const ZTBSpec::Criterion*>::const_iterator critIter = critBegin;
						allOkay && critIter != critEnd; ++critIter)
						{
						allOkay = (*critIter)->Matches(theTuple);
						}

					if (allOkay)
						{
						oIDs.insert(currentID);
						#if ZCONFIG_TS_RAM_Logging
							++finalResultSize;
						#endif
						}
					}
				#if ZCONFIG_TS_RAM_Logging
					s.Writef("\nFinal result size: %d, of %d, elapsed: %gms",
						finalResultSize,
						fTuples.size(),
						1000*(ZTime::sSystem() - startOfThisSearch));
				#endif
				}
			}
		#if ZCONFIG_TS_RAM_Logging
			s.Writef("\nOverall elapsed: %gms.", 1000*(ZTime::sSystem() - startTime));
		#endif
		}
	}

ZMutexBase& ZTS_RAM::GetReadLock()
	{ return fRWLock.GetRead(); }

ZMutexBase& ZTS_RAM::GetWriteLock()
	{ return fRWLock.GetWrite(); }

void ZTS_RAM::AddIndex(ZRef<ZTupleIndexFactory> iIndexFactory)
	{
	ZTupleIndex* theIndex = iIndexFactory->Make();
	fIndices.push_back(theIndex);
	for (map<uint64, ZTuple>::const_iterator i = fTuples.begin(); i != fTuples.end(); ++i)
		theIndex->Insert(i->first, &i->second);
	}

void ZTS_RAM::AddIndices(const vector<ZRef<ZTupleIndexFactory> >& iIndexFactories)
	{
	for (vector<ZRef<ZTupleIndexFactory> >::const_iterator factIter = iIndexFactories.begin();
		factIter != iIndexFactories.end(); ++factIter)
		{
		this->AddIndex(*factIter);
		}
	}

bool ZTS_RAM::GetDataIfChanged(uint64& oNextUnusedID, map<uint64, ZTuple>& oTuples)
	{
	// At least our read lock must be held.
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanRead());

	if (fChanged)
		{
		fChanged = false;
		this->GetData(oNextUnusedID, oTuples);
		return true;
		}
	return false;
	}

bool ZTS_RAM::GetDataIfChanged(uint64& oNextUnusedID, vector<pair<uint64, ZTuple> >& oTuples)
	{
	// At least our read lock must be held.
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanRead());

	if (fChanged)
		{
		fChanged = false;
		oTuples.reserve(fTuples.size());
		std::copy(fTuples.begin(), fTuples.end(), back_inserter(oTuples));
		return true;
		}
	return false;
	}

void ZTS_RAM::GetData(uint64& oNextUnusedID, map<uint64, ZTuple>& oTuples)
	{
	// At least our read lock must be held.
	ZAssertStop(kDebug_TS_RAM, fRWLock.CanRead());

	ZMutexLocker lockerID(fMutex_ID);
	oNextUnusedID = fNextUnusedID;
	lockerID.Release();

	oTuples = fTuples;
	}

} // namespace ZooLib
