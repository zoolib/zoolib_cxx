/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTupleIndex_General.h"

#include "zoolib/ZStrim.h"
#include "zoolib/ZUtil_Strim_Operators.h"

using std::set;
using std::string;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_General::Key

inline bool ZTupleIndex_General::Key::operator<(const Key& iOther) const
	{
	// When called by lower_bound this is a key in the set, and iOther
	// is a key that's being looked for. So when this has more entries
	// than iOther then this is considered bigger than iOther.

	// When called by upper_bound this is a key that's being looked for
	// and iOther is a key in the set. So when this has fewer entries
	// than iOther then this is is considered bigger than iOther.

	// The upshot being that when one or other vector of values is
	// exhausted we return false, indicating that this is not smaller
	// than iOther.

	const ZTValue* const* otherIter = &iOther.fValues[0];
	const ZTValue* const* otherEnd = &iOther.fValues[iOther.fPropCount];

	const ZTValue* const* thisIter = &fValues[0];
	const ZTValue* const* thisEnd = &fValues[fPropCount];

	for (;;)
		{
		if (otherIter == otherEnd)
			{
			// We've run out of iOther, and must be equal so far.
			if (thisIter == thisEnd)
				{
				// We've also run out of this. We use IDs as a tiebreak.
				return fID < iOther.fID;
				}
			else
				{
				// Our vectors are of different lengths, so we
				// return false as discussed above.
				return false;
				}
			}
		else if (thisIter == thisEnd)
			{
			// Again, our vectors are of different lengths so we return false.
			return false;
			}

		if (int compare = (*thisIter)->Compare(**otherIter))
			return compare < 0;
		++thisIter;
		++otherIter;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_General

ZTupleIndex_General::ZTupleIndex_General(const std::string* iPropNames, size_t iPropNameCount)
:	fPropNameCount(iPropNameCount)
	{
	copy(iPropNames, iPropNames + iPropNameCount, fPropNames);
	}

void ZTupleIndex_General::Insert(uint64 iID, const ZTuple* iTuple)
	{
	Key theKey;
	if (this->pKeyFromTuple(iID, iTuple, theKey))
		fSet.insert(theKey);
	}

void ZTupleIndex_General::Erase(uint64 iID, const ZTuple* iTuple)
	{
	Key theKey;
	if (this->pKeyFromTuple(iID, iTuple, theKey))
		fSet.erase(theKey);
	}

void ZTupleIndex_General::Find(const set<uint64>& iSkipIDs,
		vector<const ZTBSpec::Criterion*>& ioCriteria, vector<uint64>& oIDs)
	{
	set<Key>::const_iterator lowerBound, upperBound;
	if (this->pSetupIterators(ioCriteria, lowerBound, upperBound))
		{
		if (iSkipIDs.empty())
			{
			for (/*no init*/; lowerBound != upperBound; ++lowerBound)
				oIDs.push_back((*lowerBound).fID);
			}
		else
			{
			for (/*no init*/; lowerBound != upperBound; ++lowerBound)
				{
				if (iSkipIDs.end() == iSkipIDs.find((*lowerBound).fID))
					oIDs.push_back((*lowerBound).fID);
				}
			}
		}
	}

size_t ZTupleIndex_General::CanHandle(const ZTBSpec::CriterionSect& iCriterionSect)
	{
	vector<const ZTBSpec::Criterion*> remainingCriteria;
	remainingCriteria.reserve(iCriterionSect.size());
	for (ZTBSpec::CriterionSect::const_iterator i = iCriterionSect.begin(),
		theEnd = iCriterionSect.end();
		i != theEnd; ++i)
		{
		remainingCriteria.push_back(&(*i));
		}

	size_t likelyResultSize = fSet.size();

	size_t propertyCount = 0;

	const ZTValue* valueEqual;
	const ZTValue* bestValueLess;
	const ZTValue* bestValueLessEqual;
	const ZTValue* bestValueGreater;
	const ZTValue* bestValueGreaterEqual;

	// Walk through our properties, looking for and removing entries in
	// remainingCriteria where where the effective constraint is equivalence.
	for (const ZTName* propNameIter = fPropNames;
		propertyCount < fPropNameCount; ++propertyCount)
		{
		if (!sGatherMergeConstraints(*propNameIter++, remainingCriteria,
			valueEqual,
			bestValueLess, bestValueLessEqual, bestValueGreater, bestValueGreaterEqual))
			{
			// We found a contradiction, so we can trivially handle this search.
			return 1;
			}

		if (!valueEqual)
			break;

		ZAssert(!bestValueLess && !bestValueLessEqual
			&& !bestValueGreater && !bestValueGreaterEqual);

		likelyResultSize /= 2;
		}

	if (propertyCount != fPropNameCount)
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

void ZTupleIndex_General::WriteDescription(const ZStrimW& s)
	{
	s << "ZTupleIndex_General. ";
	s.Writef("%zu entries, indexing on ", fSet.size());
	for (size_t x = 0; x < fPropNameCount; ++x)
		s << string8(fPropNames[x]) << ", ";
	}

bool ZTupleIndex_General::pSetupIterators(vector<const ZTBSpec::Criterion*>& ioCriteria,
	std::set<Key>::const_iterator& oLowerBound, std::set<Key>::const_iterator& oUpperBound)
	{
	Key theKey;
	theKey.fPropCount = 0;

	const ZTValue* valueEqual;
	const ZTValue* bestValueLess;
	const ZTValue* bestValueLessEqual;
	const ZTValue* bestValueGreater;
	const ZTValue* bestValueGreaterEqual;

	// Walk through our properties, looking for and removing entries in
	// ioCriteria where where the effective constraint is equivalence.
	for (const ZTName* propNameIter = fPropNames;
		theKey.fPropCount < fPropNameCount; ++propNameIter)
		{
		if (!sGatherMergeConstraints(*propNameIter, ioCriteria,
			valueEqual,
			bestValueLess, bestValueLessEqual, bestValueGreater, bestValueGreaterEqual))
			{
			// We found a contradiction, so we have completed the search, with no results.
			return false;
			}

		if (!valueEqual)
			break;

		ZAssert(!bestValueLess && !bestValueLessEqual
			&& !bestValueGreater && !bestValueGreaterEqual);
		theKey.fValues[theKey.fPropCount++] = valueEqual;
		}

	if (theKey.fPropCount == fPropNameCount)
		{
		// We're doing an equals search on every entry in fPropNames.
		theKey.fID = 0;
		oLowerBound = fSet.lower_bound(theKey);

		theKey.fID = kMaxID;
		oUpperBound = fSet.upper_bound(theKey);

		return true;
		}

	// We still have property names on which we index not referenced by the criteria we've seen.
	if (!bestValueLess && !bestValueLessEqual && !bestValueGreater && !bestValueGreaterEqual)
		{
		// There is no range check on this entry, so we're just doing an equality check
		// of the entries found so far. There will be entries in ioCriteria that match
		// our remaining properties (or CanHandle would have returned false) and our
		// caller will check them against anything between lower and upper.
		theKey.fID = 0;
		oLowerBound = fSet.lower_bound(theKey);

		theKey.fID = kMaxID;
		oUpperBound = fSet.upper_bound(theKey);
		}
	else
		{
		// We have a range check on this entry.
		if (bestValueGreater)
			{
			ZAssertStop(ZCONFIG_TupleIndex_Debug, !bestValueGreaterEqual);
			theKey.fID = kMaxID;
			theKey.fValues[theKey.fPropCount++] = bestValueGreater;
			oLowerBound = fSet.upper_bound(theKey);
			// Reset fPropCount, in case we're subsequently doing a less-bounded search
			--theKey.fPropCount;
			}
		else if (bestValueGreaterEqual)
			{
			theKey.fID = 0;
			theKey.fValues[theKey.fPropCount++] = bestValueGreaterEqual;
			oLowerBound = fSet.lower_bound(theKey);
			--theKey.fPropCount;
			}
		else
			{
			// Find the first entry greater than or equal to
			// the (possibly empty) equals portion of the key.
			theKey.fID = 0;
			oLowerBound = fSet.lower_bound(theKey);
			}

		if (bestValueLess)
			{
			ZAssertStop(ZCONFIG_TupleIndex_Debug, !bestValueLessEqual);
			theKey.fID = 0;
			theKey.fValues[theKey.fPropCount++] = bestValueLess;
			oUpperBound = fSet.lower_bound(theKey);
			}
		else if (bestValueLessEqual)
			{
			theKey.fID = kMaxID;
			theKey.fValues[theKey.fPropCount++] = bestValueLessEqual;
			oUpperBound = fSet.upper_bound(theKey);
			}
		else
			{
			// Find the first tuple greater than the (possibly
			// empty) equals portion of the key.
			theKey.fID = kMaxID;
			oUpperBound = fSet.upper_bound(theKey);
			}
		}

	return true;
	}

bool ZTupleIndex_General::pKeyFromTuple(uint64 iID, const ZTuple* iTuple, Key& oKey)
	{
	ZTuple::const_iterator tupleIter = iTuple->IteratorOf(fPropNames[0]);
	if (tupleIter == iTuple->end())
		{
		// The tuple does not have our first property, so there's no point
		// in storing it -- no search we can do will help find this tuple.
		return false;
		}

	oKey.fPropCount = fPropNameCount;

	oKey.fValues[0] = iTuple->PGet(tupleIter);

	for (size_t x = 1; x < fPropNameCount; ++x)
		{
		if (const ZTValue* theVal = iTuple->PGet(fPropNames[x]))
			oKey.fValues[x] = theVal;
		else
			oKey.fValues[x] = &spEmptyVal;
		}

	oKey.fID = iID;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_General

ZTupleIndexFactory_General::ZTupleIndexFactory_General(
	const vector<string>& iPropNames)
:	fPropNameCount(iPropNames.size())
	{
	ZAssertStop(ZCONFIG_TupleIndex_Debug, fPropNameCount < ZTupleIndex_General::kMaxProperties);

	if (ZCONFIG_Debug >= ZCONFIG_TupleIndex_Debug)
		{
		set<string> theSet;
		for (vector<string>::const_iterator i = iPropNames.begin();
			i != iPropNames.end(); ++i)
			{
			if ((*i).empty())
				{
				ZDebugStopf(0,
					("You're initializing an index factory with an empty property name"));
				}

			if (theSet.end() != theSet.find(*i))
				{
				ZDebugStopf(0,
					("You're initializing an index factory with a repeated property name"));
				}

			theSet.insert(*i);
			}
		}

	for (size_t x = 0; x < fPropNameCount; ++x)
		fPropNames[x] = iPropNames[x];
	}

ZTupleIndex* ZTupleIndexFactory_General::Make()
	{ return new ZTupleIndex_General(fPropNames, fPropNameCount); }

} // namespace ZooLib
