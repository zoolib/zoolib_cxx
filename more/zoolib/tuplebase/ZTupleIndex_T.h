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

#ifndef __ZTupleIndex_T__
#define __ZTupleIndex_T__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTupleIndex.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_T

template <int kPropCount>
class ZTupleIndex_T : public ZTupleIndex
	{
private:
	class Key
		{
	public:
		bool operator<(const Key& iOther) const
			{
			// When called by lower_bound this is a key in the set, and iOther
			// is a key that's being looked for. So when this has more entries
			// than iOther then this is considered bigger than iOther.

			// When called by upper_bound this is a key that's being looked for
			// and iOther is a key in the set. So when this has fewer entries
			// than iOther then this is considered bigger than iOther.

			// The upshot being that when one or other vector of values is
			// exhausted we return false, indicating that this is not smaller
			// than iOther.

			const ZTValue* const* otherIter = &iOther.fValues[0];

			const ZTValue* const* thisIter = &fValues[0];

			for (size_t x = kPropCount; x; --x)
				{
				if (!*otherIter || !*thisIter)
					{
					// Our vectors are of different lengths, so we return false as discussed above.
					return false;
					}

				if (int compare = (*thisIter)->Compare(**otherIter))
					return compare < 0;
				++thisIter;
				++otherIter;
				}
			return fID < iOther.fID;
			}

		uint64 fID;
		const ZTValue* fValues[kPropCount];
		};

public:
	ZTupleIndex_T(const ZTName* iPropNames)
		{
		std::copy(iPropNames, iPropNames + kPropCount, fPropNames);
		}

	virtual void Insert(uint64 iID, const ZTuple* iTuple)
		{
		Key theKey;
		if (this->pKeyFromTuple(iID, iTuple, theKey))
			fSet.insert(theKey);
		}

	virtual void Erase(uint64 iID, const ZTuple* iTuple)
		{
		Key theKey;
		if (this->pKeyFromTuple(iID, iTuple, theKey))
			fSet.erase(theKey);
		}

	virtual void Find(const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs)
		{
		typename std::set<Key>::const_iterator lowerBound, upperBound;
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

	virtual size_t CanHandle(const ZTBSpec::CriterionSect& iCriterionSect)
		{
		std::vector<const ZTBSpec::Criterion*> remainingCriteria;
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
			propertyCount < kPropCount; ++propertyCount)
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

			ZAssertStop(ZCONFIG_TupleIndex_Debug, !bestValueLess && !bestValueLessEqual
				&& !bestValueGreater && !bestValueGreaterEqual);

			likelyResultSize /= 2;
			}

		if (propertyCount != kPropCount)
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

//	virtual void WriteDescription(const ZStrimW& s);

private:
	bool pSetupIterators(std::vector<const ZTBSpec::Criterion*>& ioCriteria,
		typename std::set<Key>::const_iterator& oLowerBound,
		typename std::set<Key>::const_iterator& oUpperBound)
		{
		Key theKey;

		theKey.fValues[0] = nullptr;

		if (kPropCount > 1)
			theKey.fValues[1] = nullptr;
		if (kPropCount > 2)
			theKey.fValues[2] = nullptr;
		if (kPropCount > 3)
			theKey.fValues[3] = nullptr;
		if (kPropCount > 4)
			theKey.fValues[4] = nullptr;
		if (kPropCount > 5)
			{
			for (size_t x = 5; x < kPropCount; ++x)
				theKey.fValues[x] = nullptr;
			}

		size_t keyPropCount = 0;

		const ZTValue* valueEqual;
		const ZTValue* bestValueLess;
		const ZTValue* bestValueLessEqual;
		const ZTValue* bestValueGreater;
		const ZTValue* bestValueGreaterEqual;

		// Walk through our properties, looking for and removing entries in
		// ioCriteria where where the effective constraint is equivalence.
		for (const ZTName* propNameIter = fPropNames;
			keyPropCount < kPropCount; ++propNameIter)
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

			ZAssertStop(ZCONFIG_TupleIndex_Debug, !bestValueLess && !bestValueLessEqual
				&& !bestValueGreater && !bestValueGreaterEqual);
			theKey.fValues[keyPropCount++] = valueEqual;
			}

		if (bestValueGreater)
			{
			ZAssertStop(ZCONFIG_TupleIndex_Debug, !bestValueGreaterEqual);
			theKey.fID = kMaxID;
			theKey.fValues[keyPropCount] = bestValueGreater;
			oLowerBound = fSet.upper_bound(theKey);
			theKey.fValues[keyPropCount] = nullptr;
			}
		else if (bestValueGreaterEqual)
			{
			theKey.fID = 0;
			theKey.fValues[keyPropCount] = bestValueGreaterEqual;
			oLowerBound = fSet.lower_bound(theKey);
			theKey.fValues[keyPropCount] = nullptr;
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
			theKey.fValues[keyPropCount] = bestValueLess;
			oUpperBound = fSet.lower_bound(theKey);
			}
		else if (bestValueLessEqual)
			{
			theKey.fID = kMaxID;
			theKey.fValues[keyPropCount] = bestValueLessEqual;
			oUpperBound = fSet.upper_bound(theKey);
			}
		else
			{
			// Find the first tuple greater than the (possibly
			// empty) equals portion of the key.
			theKey.fID = kMaxID;
			oUpperBound = fSet.upper_bound(theKey);
			}

		return true;
		}

	bool pKeyFromTuple(uint64 iID, const ZTuple* iTuple, Key& oKey)
		{
		ZTuple::const_iterator tupleIter = iTuple->IteratorOf(fPropNames[0]);
		if (tupleIter == iTuple->end())
			{
			// The tuple does not have our first property, so there's no point
			// in storing it -- no search we can do will help find this tuple.
			return false;
			}

		oKey.fValues[0] = iTuple->PGet(tupleIter);

		for (size_t x = 1; x < kPropCount; ++x)
			{
			if (const ZTValue* theVal = iTuple->PGet(fPropNames[x]))
				oKey.fValues[x] = theVal;
			else
				oKey.fValues[x] = &spEmptyVal;
			}

		oKey.fID = iID;
		return true;
		}

	ZTName fPropNames[kPropCount];
	std::set<Key> fSet;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_T

template <int kPropCount>
class ZTupleIndexFactory_T : public ZTupleIndexFactory
	{
public:
	ZTupleIndexFactory_T(const std::string* iPropNames)
		{
		std::copy(iPropNames, iPropNames + kPropCount, fPropNames);
		}

	// From ZTupleIndexFactory
	virtual ZTupleIndex* Make()
		{ return new ZTupleIndex_T<kPropCount>(fPropNames); }

private:
	ZTName fPropNames[kPropCount];
	};

} // namespace ZooLib

#endif // __ZTupleIndex_T__
