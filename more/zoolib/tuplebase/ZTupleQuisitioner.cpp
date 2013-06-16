/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTupleQuisitioner.h"

#define kDebug_TupleQuisitioner 2

using std::pair;
using std::set;
using std::vector;

namespace std {

inline void swap(
	pair<ZStdInt::uint64, vector<const ZooLib::ZTValue*> >& a,
	pair<ZStdInt::uint64, vector<const ZooLib::ZTValue*> >& b)
	{
	swap(a.first, b.first);
	a.second.swap(b.second);
	}

} // namespace std

namespace ZooLib {

// =================================================================================================

static int spTupleValueComp(ZTextCollator* ioTextCollators, int iStrength,
	const ZTValue& iLeft, const ZTValue& iRight)
	{
	if (const string8* lp = iLeft.PGet<string8>())
		{
		if (const string8* rp = iRight.PGet<string8>())
			{
			if (iStrength <= 4)
				{
				if (not ioTextCollators[iStrength - 1])
					ioTextCollators[iStrength - 1] = ZTextCollator(iStrength);
				return ioTextCollators[iStrength - 1].Compare(*lp, *rp);
				}
			else
				{
				return ZTextCollator(iStrength).Compare(*lp, *rp);
				}
			}
		}
	return iLeft.Compare(iRight);
	}

/*! Remove from ioIDs any entry whose corresponding tuple does not match iFilter. */
static void spFilter(const ZTBSpec& iFilter, const vector<ZTuple>& iTuples, vector<uint64>& ioIDs)
	{
	vector<ZTuple>::const_iterator iterTuple = iTuples.begin();
	vector<ZTuple>::const_iterator theEnd = iTuples.end();
	vector<uint64>::iterator iterIDRead = ioIDs.begin();
	vector<uint64>::iterator iterIDWrite = iterIDRead;

	while (iterTuple != theEnd)
		{
		if (iFilter.Matches(*iterTuple++))
			*iterIDWrite++ = *iterIDRead++;
		else
			++iterIDRead;
		}
	ioIDs.erase(iterIDWrite, iterIDRead);
	}

/*! For each property listed in iSort, put the corresponding value in oProps. */
static void spExtractProps(const vector<ZTBQuery::SortSpec>& iSort, const ZTuple& iTuple,
	vector<const ZTValue*>& oProps)
	{
	oProps.reserve(iSort.size());
	for (vector<ZTBQuery::SortSpec>::const_iterator i = iSort.begin(), theEnd = iSort.end();
		i != theEnd; ++i)
		{
		oProps.push_back(iTuple.PGet((*i).fPropName));
		}
	}

// =================================================================================================

namespace { // anonymous
struct CompareTuples_t
	{
	CompareTuples_t(ZTextCollator* ioTextCollators, const vector<ZTBQuery::SortSpec>& iSort);

	bool operator()(const pair<uint64, vector<const ZTValue*> >& iLeft,
		const pair<uint64, vector<const ZTValue*> >& iRight) const;

	const vector<ZTBQuery::SortSpec>& fSort;

	typedef pair<uint64, ZTuple> first_argument_type;
	typedef pair<uint64, ZTuple> second_argument_type;
	typedef bool result_type;
	ZTextCollator* fTextCollators;
	};

CompareTuples_t::CompareTuples_t(
	ZTextCollator* ioTextCollators, const vector<ZTBQuery::SortSpec>& iSort)
:	fSort(iSort),
	fTextCollators(ioTextCollators)
	{}

inline bool CompareTuples_t::operator()(const pair<uint64, vector<const ZTValue*> >& iLeft,
	const pair<uint64, vector<const ZTValue*> >& iRight) const
	{
	vector<const ZTValue*>::const_iterator leftIter = iLeft.second.begin();
	vector<const ZTValue*>::const_iterator rightIter = iRight.second.begin();
	size_t count = fSort.size();
	for (size_t x = 0; x < count; ++x)
		{
		const ZTValue* left = *leftIter++;
		const ZTValue* right = *rightIter++;
		if (left && right)
			{
			if (int compare = spTupleValueComp(fTextCollators, fSort[x].fStrength, *left, *right))
				return fSort[x].fAscending == (compare < 0);
			}
		else if (right)
			{
			return fSort[x].fAscending;
			}
		}
	// Tiebreak using the entries' IDs.
	return iLeft.first < iRight.first;
	}
} // anonymous namespace

// =================================================================================================

static void spSort(ZTextCollator ioTextCollators[], const vector<ZTBQuery::SortSpec>& iSort,
	const vector<ZTuple>& iTuples, vector<uint64>& ioIDs)
	{
	// This is ugly, but simple. We extract the values of properties we care
	// about into a local vector, sort that and then put the IDs back into ioIDs.
	vector<pair<uint64, vector<const ZTValue*> > > theArray;
	theArray.reserve(iTuples.size());
	for (size_t x = 0; x < iTuples.size(); ++x)
		{
		theArray.push_back(
			pair<uint64, vector<const ZTValue*> >(ioIDs[x], vector<const ZTValue*>()));
		spExtractProps(iSort, iTuples[x], theArray.back().second);
		}

	sort(theArray.begin(), theArray.end(), CompareTuples_t(ioTextCollators, iSort));

	vector<pair<uint64, vector<const ZTValue*> > >::const_iterator source = theArray.begin();
	for (vector<uint64>::iterator dest = ioIDs.begin(), theEnd = ioIDs.end();
		dest != theEnd; ++dest)
		{
		*dest = (*source++).first;
		}
	}

/*! Compare the first \a iOffset entries in \a iLeft and \a iRight
using the strength/ascending values from \a iSort. */
static int spComparePrefix(ZTextCollator* ioTextCollators, size_t iOffset,
	const vector<ZTBQuery::SortSpec>& iSort,
	const vector<const ZTValue*>& iLeft, const vector<const ZTValue*>& iRight)
	{
	vector<const ZTValue*>::const_iterator leftIter = iLeft.begin();
	vector<const ZTValue*>::const_iterator rightIter = iRight.begin();
	for (size_t index = 0; index < iOffset; ++index)
		{
		if (int compare = spTupleValueComp(ioTextCollators, iSort[index].fStrength,
			*(*leftIter++), *(*rightIter++)))
			{
			if (iSort[index].fAscending)
				return compare;
			else
				return -compare;
			}
		}
	return 0;
	}

/*! Compare the entries in \a iLeft and \a iRight from \a iOffset to
the end using the strength/ascending values from \a iSort. */
static int spCompareSuffix(ZTextCollator* ioTextCollators, size_t iOffset,
	const vector<ZTBQuery::SortSpec>& iSort,
	const vector<const ZTValue*>& iLeft, const vector<const ZTValue*>& iRight)
	{
	vector<const ZTValue*>::const_iterator leftIter = iLeft.begin() + iOffset;
	vector<const ZTValue*>::const_iterator rightIter = iRight.begin() + iOffset;
	size_t count = iLeft.size();
	for (size_t index = iOffset; index < count; ++index)
		{
		if (int compare = spTupleValueComp(ioTextCollators, iSort[index].fStrength,
			*(*leftIter++), *(*rightIter++)))
			{
			if (iSort[index].fAscending)
				return compare;
			else
				return -compare;
			}
		}
	return 0;
	}

// =================================================================================================

namespace { // anonymous
struct ComparePrefix_t
	{
	ComparePrefix_t(ZTextCollator* ioTextCollators, size_t iOffset,
		const vector<ZTBQuery::SortSpec>& iSort);

	bool operator()(const vector<const ZTValue*>& iLeft,
		const vector<const ZTValue*>& iRight) const;

	size_t fOffset;
	const vector<ZTBQuery::SortSpec>& fSort;

	typedef vector<const ZTValue*> first_argument_type;
	typedef vector<const ZTValue*> second_argument_type;
	typedef bool result_type;
	ZTextCollator* fTextCollators;
	};

ComparePrefix_t::ComparePrefix_t(ZTextCollator* ioTextCollators, size_t iOffset,
	const vector<ZTBQuery::SortSpec>& iSort)
:	fOffset(iOffset),
	fSort(iSort),
	fTextCollators(ioTextCollators)
	{}

inline bool ComparePrefix_t::operator()(const vector<const ZTValue*>& iLeft,
	const vector<const ZTValue*>& iRight) const
	{
	return -1 == spComparePrefix(fTextCollators, fOffset, fSort, iLeft, iRight);
	}
} // anonymous namespace

// =================================================================================================

/*! Takes the name of a property, a sort spec and matched vectors of tuples and
their IDs. We effectively sort the tuples using iSort, and for entries which have
the same values to the left of the property named iFirst (the sequence coming from
iSort) we preserve only the smallest. */
static void spFirst(ZTextCollator* ioTextCollators, const ZTName& iFirst,
	const vector<ZTBQuery::SortSpec>& iSort,
	const vector<ZTuple>& iTuples, vector<uint64>& ioIDs)
	{
	ZAssertStop(kDebug_TupleQuisitioner, iTuples.size() == ioIDs.size());

	size_t firstOffset = 0;
	for (vector<ZTBQuery::SortSpec>::const_iterator i = iSort.begin(), theEnd = iSort.end();
		i != theEnd && (*i).fPropName != iFirst;
		++i, ++firstOffset)
		{}

	ComparePrefix_t theComparePrefix_t(ioTextCollators, firstOffset, iSort);

	vector<vector<const ZTValue*> > sortedProps;
	vector<uint64> sortedIDs;

	for (size_t x = 0; x < ioIDs.size(); ++x)
		{
		vector<const ZTValue*> curProps;
		spExtractProps(iSort, iTuples[x], curProps);

		// Find the entry in sortedProps which is greater than or
		// equal to curProps, comparing as far as firstOffset.
		vector<vector<const ZTValue*> >::iterator foundIter =
						lower_bound(sortedProps.begin(), sortedProps.end(),
						curProps, theComparePrefix_t);

		if (foundIter == sortedProps.end())
			{
			sortedProps.push_back(curProps);
			sortedIDs.push_back(ioIDs[x]);
			}
		else
			{
			int compare = spComparePrefix(
				ioTextCollators, firstOffset, iSort, curProps, *foundIter);

			if (compare == 0)
				{
				// Their prefix is the same. We now need to compare the suffix and
				// keep whichever is "less".
				if (spCompareSuffix(ioTextCollators, firstOffset, iSort, curProps, *foundIter) < 0)
					{
					sortedIDs[foundIter - sortedProps.begin()] = ioIDs[x];
					*foundIter = curProps;
					}
				}
			else
				{
				// curProps must be less than *foundIter, otherwise lower_bound messed up.
				ZAssertStop(kDebug_TupleQuisitioner, compare < 0);
				sortedIDs.insert(sortedIDs.begin() + (foundIter - sortedProps.begin()), ioIDs[x]);
				sortedProps.insert(foundIter, curProps);
				}
			}
		}

	ioIDs.swap(sortedIDs);
	}

// =================================================================================================

/*! Does the same job as spFirst, except that it then removes all
the resulting tuples that do not match iFilter.*/
static void spFirst_Filter(ZTextCollator* ioTextCollators, const ZTBSpec& iFilter,
	const ZTName& iFirst, const vector<ZTBQuery::SortSpec>& iSort,
	const vector<ZTuple>& iTuples, vector<uint64>& ioIDs)
	{
	ZAssertStop(kDebug_TupleQuisitioner, iTuples.size() == ioIDs.size());

	size_t firstOffset = 0;
	for (vector<ZTBQuery::SortSpec>::const_iterator i = iSort.begin(), theEnd = iSort.end();
		i != theEnd && (*i).fPropName != iFirst;
		++i, ++firstOffset)
		{}

	ComparePrefix_t theComparePrefix_t(ioTextCollators, firstOffset, iSort);

	vector<vector<const ZTValue*> > sortedProps;
	vector<uint64> sortedIDs;
	vector<ZTuple> sortedTuples;

	for (size_t x = 0; x < ioIDs.size(); ++x)
		{
		uint64 curID = ioIDs[x];
		vector<const ZTValue*> curProps;
		ZTuple curTuple = iTuples[x];
		spExtractProps(iSort, curTuple, curProps);

		// Find the entry in sortedProps which is greater than or equal
		// to curProps, comparing as far as firstOffset.
		vector<vector<const ZTValue*> >::iterator foundIter =
						lower_bound(sortedProps.begin(), sortedProps.end(),
						curProps, theComparePrefix_t);
		if (foundIter == sortedProps.end())
			{
			sortedIDs.push_back(curID);
			sortedTuples.push_back(curTuple);
			sortedProps.push_back(curProps);
			}
		else
			{
			size_t foundOffset = foundIter - sortedProps.begin();
			int compare = spComparePrefix(
				ioTextCollators, firstOffset, iSort, curProps, *foundIter);

			if (compare == 0)
				{
				// Their prefix is the same. We now need to compare the suffix and
				// keep whichever is "less".
				if (spCompareSuffix(ioTextCollators, firstOffset, iSort, curProps, *foundIter) < 0)
					{
					sortedIDs[foundOffset] = curID;
					sortedTuples[foundOffset] = curTuple;
					*foundIter = curProps;
					}
				}
			else
				{
				// curProps must be less than *foundIter, otherwise lower_bound messed up.
				ZAssertStop(kDebug_TupleQuisitioner, compare < 0);
				sortedIDs.insert(sortedIDs.begin() + foundOffset, curID);
				sortedTuples.insert(sortedTuples.begin() + foundOffset, curTuple);
				sortedProps.insert(foundIter, curProps);
				}
			}
		}

	spFilter(iFilter, sortedTuples, sortedIDs);
	ioIDs.swap(sortedIDs);
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Unordered(
	const ZRef<ZTBQueryNode>& iNode, const ZTBSpec* iFilter, set<uint64>& ioIDs)
	{
	if (iNode.DynamicCast<ZTBQueryNode_All>())
		{
		// We must have a filter, it's not possible to construct a ZTBQueryNode_All
		// except as a clause in a combo with a filter.
		ZAssertStop(kDebug_TupleQuisitioner, iFilter);
		this->Search(*iFilter, ioIDs);
		}
	else if (ZTBQueryNode_First* theNode_First = iNode.DynamicCast<ZTBQueryNode_First>())
		{
		if (ZRef<ZTBQueryNode> firstSource = theNode_First->GetSourceNode())
			{
			if (ZTBQueryNode_Combo* theNode_Combo =
				firstSource.DynamicCast<ZTBQueryNode_Combo>())
				{
				// The source is a combo, so we can apply a first to it.
				const ZTName& theFirst = theNode_First->GetPropName();
				const vector<ZTBQuery::SortSpec>& theSort = theNode_Combo->GetSort();
				if (theSort.empty() || sIsEmpty(theFirst))
					{
					// If there's no first then the results of any sort will not be preserved
					// after they're dumped into ioIDs, so we just ignore the sort.
					this->Query_Combo_Unordered(theNode_Combo->GetIntersections(), iFilter, ioIDs);
					}
				else
					{
					vector<uint64> theIDs;
					this->Query_Combo_First(theFirst, theSort,
						theNode_Combo->GetIntersections(), iFilter, theIDs);
					ioIDs.insert(theIDs.begin(), theIDs.end());
					}
				}
			else
				{
				// The source is not a combo, so we'll just execute a normal query against it.
				this->Query_Unordered(firstSource, iFilter, ioIDs);
				}
			}
		}
	else if (ZTBQueryNode_Combo* theNode_Combo = iNode.DynamicCast<ZTBQueryNode_Combo>())
		{
		// We're returning a set of results, so any sort will have no effect.
		this->Query_Combo_Unordered(theNode_Combo->GetIntersections(), iFilter, ioIDs);
		}
	else if (ZTBQueryNode_ID_Constant* theNode_ID_Constant =
		iNode.DynamicCast<ZTBQueryNode_ID_Constant>())
		{
		if (iFilter)
			{
			vector<uint64> destIDVector = theNode_ID_Constant->GetIDs();
			vector<ZTuple> destTuples(destIDVector.size(), ZTuple());
			this->FetchTuples(destIDVector.size(), &destIDVector[0], &destTuples[0]);
			spFilter(*iFilter, destTuples, destIDVector);
			ioIDs.insert(destIDVector.begin(), destIDVector.end());
			}
		else
			{
			const vector<uint64>& theIDs = theNode_ID_Constant->GetIDs();
			ioIDs.insert(theIDs.begin(), theIDs.end());
			}
		}
	else if (ZTBQueryNode_ID_FromSource* theNode_ID_FromSource =
		iNode.DynamicCast<ZTBQueryNode_ID_FromSource>())
		{
		set<uint64> sourceIDSet;
		this->Query_Unordered(theNode_ID_FromSource->GetSourceNode(), nullptr, sourceIDSet);

		if (iFilter)
			{
			for (set<uint64>::iterator i = sourceIDSet.begin(); i != sourceIDSet.end(); ++i)
				{
				if (uint64 theID =
					this->FetchTuple(*i).Get(theNode_ID_FromSource->GetSourcePropName()).Get<uint64>())
					{
					if (ioIDs.end() == ioIDs.find(theID))
						{
						if (iFilter->Matches(this->FetchTuple(theID)))
							ioIDs.insert(theID);
						}
					}
				}
			}
		else
			{
			for (set<uint64>::iterator i = sourceIDSet.begin(); i != sourceIDSet.end(); ++i)
				{
				if (uint64 theID =
					this->FetchTuple(*i).Get(theNode_ID_FromSource->GetSourcePropName()).Get<uint64>())
					{
					ioIDs.insert(theID);
					}
				}
			}
		}
	else if (ZTBQueryNode_Property* theNode_Property =
		iNode.DynamicCast<ZTBQueryNode_Property>())
		{
		set<uint64> sourceIDSet;
		this->Query_Unordered(theNode_Property->GetSourceNode(), nullptr, sourceIDSet);

		for (set<uint64>::iterator i = sourceIDSet.begin(); i != sourceIDSet.end(); ++i)
			{
			ZTBSpec theSpec = ZTBSpec::sEquals(theNode_Property->GetPropName(), *i);
			if (iFilter)
				theSpec &= *iFilter;
			this->Search(theSpec, ioIDs);
			}
		}
	else
		{
		// nil or unrecognized node, so no results.
		}
	}

// =================================================================================================

void ZTupleQuisitioner::Query(const ZRef<ZTBQueryNode>& iNode,
	const ZTBSpec* iFilter, vector<uint64>& ioIDs)
	{
	if (iNode.DynamicCast<ZTBQueryNode_All>())
		{
		// We must have a filter, it's not possible to construct a ZTBQueryNode_All
		// except as a clause in a combo with a filter.
		ZAssertStop(kDebug_TupleQuisitioner, iFilter);
		set<uint64> results;
		this->Search(*iFilter, results);
		ioIDs.insert(ioIDs.end(), results.begin(), results.end());
		}
	else if (ZTBQueryNode_First* theNode_First = iNode.DynamicCast<ZTBQueryNode_First>())
		{
		if (ZRef<ZTBQueryNode> firstSource = theNode_First->GetSourceNode())
			{
			if (ZTBQueryNode_Combo* theNode_Combo =
				firstSource.DynamicCast<ZTBQueryNode_Combo>())
				{
				const ZTName& theFirst = theNode_First->GetPropName();
				const vector<ZTBQuery::SortSpec>& theSort = theNode_Combo->GetSort();
				if (theSort.empty())
					{
					this->Query_Combo(theNode_Combo->GetIntersections(), iFilter, ioIDs);
					}
				else
					{
					this->Query_Combo_First(theFirst, theSort,
						theNode_Combo->GetIntersections(), iFilter, ioIDs);
					}
				}
			else
				{
				this->Query(firstSource, iFilter, ioIDs);
				}
			}
		}
	else if (ZTBQueryNode_Combo* theNode_Combo = iNode.DynamicCast<ZTBQueryNode_Combo>())
		{
		const vector<ZTBQuery::SortSpec>& theSort = theNode_Combo->GetSort();
		if (theSort.empty())
			this->Query_Combo(theNode_Combo->GetIntersections(), iFilter, ioIDs);
		else
			this->Query_Combo_Sorted(theSort, theNode_Combo->GetIntersections(), iFilter, ioIDs);
		}
	else if (ZTBQueryNode_ID_Constant* theNode_ID_Constant =
		iNode.DynamicCast<ZTBQueryNode_ID_Constant>())
		{
		if (iFilter)
			{
			vector<uint64> destIDVector = theNode_ID_Constant->GetIDs();
			vector<ZTuple> destTuples(destIDVector.size(), ZTuple());
			this->FetchTuples(destIDVector.size(), &destIDVector[0], &destTuples[0]);
			spFilter(*iFilter, destTuples, destIDVector);
			ioIDs.insert(ioIDs.end(), destIDVector.begin(), destIDVector.end());
			}
		else
			{
			const vector<uint64>& theIDs = theNode_ID_Constant->GetIDs();
			ioIDs.insert(ioIDs.end(), theIDs.begin(), theIDs.end());
			}
		}
	else if (ZTBQueryNode_ID_FromSource* theNode_ID_FromSource =
		iNode.DynamicCast<ZTBQueryNode_ID_FromSource>())
		{
		vector<uint64> sourceIDs;
		this->Query(theNode_ID_FromSource->GetSourceNode(), nullptr, sourceIDs);
		vector<ZTuple> sourceTuples(sourceIDs.size(), ZTuple());
		this->FetchTuples(sourceIDs.size(), &sourceIDs[0], &sourceTuples[0]);

		const ZTName& sourcePropName = theNode_ID_FromSource->GetSourcePropName();
		vector<uint64> destIDs;
		for (vector<ZTuple>::iterator i = sourceTuples.begin(); i != sourceTuples.end(); ++i)
			{
			if (ZQ<uint64> theID = i->QGet<uint64>(sourcePropName))
				destIDs.push_back(*theID);
			}

		if (iFilter)
			{
			vector<uint64> destIDVector(destIDs.begin(), destIDs.end());
			vector<ZTuple> destTuples(destIDVector.size(), ZTuple());
			this->FetchTuples(destIDVector.size(), &destIDVector[0], &destTuples[0]);
			spFilter(*iFilter, destTuples, destIDVector);
			ioIDs.insert(ioIDs.end(), destIDVector.begin(), destIDVector.end());
			}
		else
			{
			ioIDs.insert(ioIDs.end(), destIDs.begin(), destIDs.end());
			}
		}
	else if (ZTBQueryNode_Property* theNode_Property =
		iNode.DynamicCast<ZTBQueryNode_Property>())
		{
		vector<uint64> sourceIDs;
		this->Query(theNode_Property->GetSourceNode(), nullptr, sourceIDs);
		//##
		// At this point we can probably walk sourceIDs and remove duplicates.

		for (vector<uint64>::iterator i = sourceIDs.begin(); i != sourceIDs.end(); ++i)
			{
			set<uint64> theIDs;
			ZTBSpec theSpec = ZTBSpec::sEquals(theNode_Property->GetPropName(), *i);
			if (iFilter)
				theSpec &= *iFilter;
			this->Search(theSpec, theIDs);
			ioIDs.insert(ioIDs.end(), theIDs.begin(), theIDs.end());
			}
		}
	else
		{
		// nil or unrecognized node, so no results.
		}
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Combo_Unordered(
	const vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
	const ZTBSpec* iFilter, set<uint64>& ioIDs)
	{
	if (iIntersections.empty())
		return;

	if (iIntersections.size() == 1)
		{
		this->Query_Intersection_Unordered(iIntersections.front(), iFilter, ioIDs);
		return;
		}

	for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
		iterSect = iIntersections.begin(), theEnd = iIntersections.end();
		iterSect != theEnd; ++iterSect)
		{
		this->Query_Intersection_Unordered(*iterSect, iFilter, ioIDs);
		}
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Combo(const vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
				const ZTBSpec* iFilter, vector<uint64>& ioIDs)
	{
	if (iIntersections.empty())
		return;

	if (iIntersections.size() == 1)
		{
		// If there's only a single intersection we may
		// be able to preserve the order of the sources. Or not.
		this->Query_Intersection(iIntersections.front(), iFilter, ioIDs);
		return;
		}

	// There's more than one intersection to union together.
	// We can't easily or sensibly preserve an order.
	set<uint64> results;
	for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
		iterSect = iIntersections.begin(), theEnd = iIntersections.end();
		iterSect != theEnd; ++iterSect)
		{
		this->Query_Intersection_Unordered(*iterSect, iFilter, results);
		}

	ioIDs.insert(ioIDs.end(), results.begin(), results.end());
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Combo_Sorted(const vector<ZTBQuery::SortSpec>& iSort,
				const vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
				const ZTBSpec* iFilter, vector<uint64>& ioIDs)
	{
	if (iIntersections.empty())
		return;

	vector<uint64> theIDs;
	if (iIntersections.size() == 1)
		{
		this->Query_Intersection(iIntersections.front(), iFilter, theIDs);
		}
	else
		{
		// There's more than one intersection to union together.
		// We can't easily or sensibly preserve an order.
		set<uint64> results;
		for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
			iterSect = iIntersections.begin(), theEnd = iIntersections.end();
			iterSect != theEnd; ++iterSect)
			{
			this->Query_Intersection_Unordered(*iterSect, iFilter, results);
			}
		theIDs.insert(theIDs.end(), results.begin(), results.end());
		}

	if (!theIDs.empty())
		{
		vector<ZTuple> tuples(theIDs.size(), ZTuple());
		this->FetchTuples(theIDs.size(), &theIDs[0], &tuples[0]);

		spSort(fTextCollators, iSort, tuples, theIDs);

		ioIDs.insert(ioIDs.end(), theIDs.begin(), theIDs.end());
		}
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Combo_First(const ZTName& iFirst,
	const vector<ZTBQuery::SortSpec>& iSort,
	const vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
	const ZTBSpec* iFilter, vector<uint64>& ioIDs)
	{
	ZAssertStop(kDebug_TupleQuisitioner, sNotEmpty(iFirst));

	if (iIntersections.empty())
		return;

	vector<uint64> results;
	if (iIntersections.size() == 1)
		{
		this->Query_Intersection(iIntersections.front(), nullptr, results);
		}
	else
		{
		// There's more than one intersection to union together.
		// We can't easily or sensibly preserve an order.
		set<uint64> curResults;
		for (vector<ZTBQueryNode_Combo::Intersection>::const_iterator
			iterSect = iIntersections.begin(), theEnd = iIntersections.end();
			iterSect != theEnd; ++iterSect)
			{
			this->Query_Intersection_Unordered(*iterSect, nullptr, curResults);
			}
		results.insert(results.end(), curResults.begin(), curResults.end());
		}

	vector<ZTuple> tuples(results.size(), ZTuple());
	this->FetchTuples(results.size(), &results[0], &tuples[0]);

	if (iFilter)
		spFirst_Filter(fTextCollators, *iFilter, iFirst, iSort, tuples, results);
	else
		spFirst(fTextCollators, iFirst, iSort, tuples, results);

	ioIDs.insert(ioIDs.end(), results.begin(), results.end());
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Intersection_Unordered(
	const ZTBQueryNode_Combo::Intersection& iIntersection,
	const ZTBSpec* iFilter, set<uint64>& ioIDs)
	{
	if (iIntersection.fNodes.empty())
		return;

	ZTBSpec theFilter = iIntersection.fFilter;
	if (iFilter)
		theFilter &= *iFilter;

	if (iIntersection.fNodes.size() == 1)
		{
		this->Query_Unordered(iIntersection.fNodes.front(), &theFilter, ioIDs);
		return;
		}

	set<uint64> results;
	this->Query_Unordered(iIntersection.fNodes.front(), &theFilter, results);

	for (vector<ZRef<ZTBQueryNode> >::const_iterator
		iterNodes = iIntersection.fNodes.begin() + 1, theEnd = iIntersection.fNodes.end();
		iterNodes != theEnd; ++iterNodes)
		{
		set<uint64> curResults;
		this->Query_Unordered(*iterNodes, &theFilter, curResults);
		set<uint64> sectSet;
		set_intersection(results.begin(), results.end(),
			curResults.begin(), curResults.end(),
			inserter(sectSet, sectSet.begin()));
		results.swap(sectSet);
		}

	ioIDs.insert(results.begin(), results.end());
	}

// =================================================================================================

void ZTupleQuisitioner::Query_Intersection(const ZTBQueryNode_Combo::Intersection& iIntersection,
	const ZTBSpec* iFilter, vector<uint64>& ioIDs)
	{
	if (iIntersection.fNodes.empty())
		return;

	ZTBSpec theFilter = iIntersection.fFilter;
	if (iFilter)
		theFilter &= *iFilter;

	if (iIntersection.fNodes.size() == 1)
		{
		this->Query(iIntersection.fNodes.front(), &theFilter, ioIDs);
		return;
		}

	set<uint64> results;
	this->Query_Unordered(iIntersection.fNodes.front(), &theFilter, results);

	for (vector<ZRef<ZTBQueryNode> >::const_iterator
		iterNodes = iIntersection.fNodes.begin() + 1, theEnd = iIntersection.fNodes.end();
		iterNodes != theEnd; ++iterNodes)
		{
		set<uint64> curResults;
		this->Query_Unordered(*iterNodes, &theFilter, curResults);
		set<uint64> sectSet;
		set_intersection(results.begin(), results.end(),
			curResults.begin(), curResults.end(),
			inserter(sectSet, sectSet.begin()));
		results.swap(sectSet);
		}

	ioIDs.insert(ioIDs.end(), results.begin(), results.end());
	}

} // namespace ZooLib
