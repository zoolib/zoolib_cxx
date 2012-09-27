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

#include "zoolib/tuplebase/ZTupleIndex.h"

#include "zoolib/ZUtil_Strim_operators.h"

using std::set;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex

ZTValue ZTupleIndex::spEmptyVal;

ZTupleIndex::~ZTupleIndex()
	{}

void ZTupleIndex::Find(const ZTBSpec::CriterionSect& iCriterionSect, const set<uint64>& iSkipIDs,
	vector<const ZTBSpec::Criterion*>& oUncheckedCriteria, vector<uint64>& oIDs)
	{
	oUncheckedCriteria.reserve(iCriterionSect.size());
	for (ZTBSpec::CriterionSect::const_iterator i = iCriterionSect.begin(),
		theEnd = iCriterionSect.end();
		i != theEnd; ++i)
		{
		oUncheckedCriteria.push_back(&(*i));
		}

	this->Find(iSkipIDs, oUncheckedCriteria, oIDs);
	}

void ZTupleIndex::WriteDescription(const ZStrimW& s)
	{
	s << "An index which doesn't understand WriteDescription";
	}

bool ZTupleIndex::sMatchIndices(const ZTBSpec::CriterionUnion& iCriterionUnion,
	const vector<ZTupleIndex*>& iIndices, vector<ZTupleIndex*>& oIndices)
	{
	oIndices.reserve(iCriterionUnion.size());

	for (ZTBSpec::CriterionUnion::const_iterator critIter = iCriterionUnion.begin();
		critIter != iCriterionUnion.end(); ++critIter)
		{
		size_t bestWeight = 0;
		ZTupleIndex* bestIndex = nullptr;
		for (vector<ZTupleIndex*>::const_iterator indIter = iIndices.begin();
			indIter != iIndices.end(); ++indIter)
			{
			if (size_t weight = (*indIter)->CanHandle(*critIter))
				{
				if (bestIndex == nullptr || bestWeight > weight)
					{
					bestWeight = weight;
					bestIndex = *indIter;
					}
				}
			}
		if (!bestWeight)
			return false;
		oIndices.push_back(bestIndex);
		}
	return true;
	}

bool ZTupleIndex::sGatherMergeConstraints(const ZTName& iPropName,
	vector<const ZTBSpec::Criterion*>& ioCriteria,
	const ZTValue*& oValueEqual,
	const ZTValue*& oBestValueLess, const ZTValue*& oBestValueLessEqual,
	const ZTValue*& oBestValueGreater, const ZTValue*& oBestValueGreaterEqual)
	{
	oValueEqual = nullptr;
	oBestValueLess = nullptr;
	oBestValueLessEqual = nullptr;
	oBestValueGreater = nullptr;
	oBestValueGreaterEqual = nullptr;

	for (vector<const ZTBSpec::Criterion*>::iterator critIter = ioCriteria.begin();
		critIter != ioCriteria.end();
		/*no increment*/)
		{
		if (0 != (*critIter)->GetComparator().fStrength
			|| not((*critIter)->GetPropName() == iPropName))
			{
			++critIter;
			continue;
			}

		const ZTValue* currentValue = &(*critIter)->GetTValue();
		switch ((*critIter)->GetComparator().fRel)
			{
			case ZTBSpec::eRel_Equal:
				{
				if (oValueEqual && not (*oValueEqual == *currentValue))
					{
					// We've got two equals constraints with different values, so nothing
					// can satisfy them both and we return an indication of contradiction.
					return false;
					}
				oValueEqual = currentValue;
				break;
				}
			case ZTBSpec::eRel_Less:
				{
				if (oBestValueLessEqual)
					{
					ZAssert(!oBestValueLess);
					if (*currentValue <= *oBestValueLessEqual)
						{
						oBestValueLessEqual = nullptr;
						oBestValueLess = currentValue;
						}
					}
				else if (!oBestValueLess || *currentValue < *oBestValueLess)
					{
					oBestValueLess = currentValue;
					}
				break;
				}
			case ZTBSpec::eRel_LessEqual:
				{
				if (oBestValueLess)
					{
					ZAssert(!oBestValueLessEqual);
					if (*currentValue < *oBestValueLess)
						{
						oBestValueLess = nullptr;
						oBestValueLessEqual = currentValue;
						}
					}
				else if (!oBestValueLessEqual || *currentValue < *oBestValueLessEqual)
					{
					oBestValueLessEqual = currentValue;
					}
				break;
				}
			case ZTBSpec::eRel_Greater:
				{
				if (oBestValueGreaterEqual)
					{
					ZAssert(!oBestValueGreater);
					if (*currentValue >= *oBestValueGreater)
						{
						oBestValueGreaterEqual = nullptr;
						oBestValueGreater = currentValue;
						}
					}
				else if (!oBestValueGreater || *currentValue > *oBestValueGreater)
					{
					oBestValueGreater = currentValue;
					}
				break;
				}
			case ZTBSpec::eRel_GreaterEqual:
				{
				if (oBestValueGreater)
					{
					ZAssert(!oBestValueGreaterEqual);
					if (*currentValue > *oBestValueGreater)
						{
						oBestValueGreater = nullptr;
						oBestValueGreaterEqual = currentValue;
						}
					}
				else if (!oBestValueGreaterEqual || *currentValue > *oBestValueGreaterEqual)
					{
					oBestValueGreaterEqual = currentValue;
					}
				break;
				}
			default:
				{
				// It's some other kind of relationship, so move on to the next criteria.
				++critIter;
				continue;
				}
			}
		critIter = ioCriteria.erase(critIter);
		}

	// Now ensure we don't have a contradiction between the equals,
	// less and greater constraints (if any).
	if (oValueEqual)
		{
		if (oBestValueLess)
			{
			ZAssert(!oBestValueLessEqual);
			if (*oBestValueLess <= *oValueEqual)
				return false;
			oBestValueLess = nullptr;
			}
		else if (oBestValueLessEqual)
			{
			if (*oBestValueLessEqual < *oValueEqual)
				return false;
			oBestValueLessEqual = nullptr;
			}

		if (oBestValueGreater)
			{
			ZAssert(!oBestValueGreaterEqual);
			if (*oBestValueGreater >= *oValueEqual)
				return false;
			oBestValueGreater = nullptr;
			}
		else if (oBestValueGreaterEqual)
			{
			if (*oBestValueGreaterEqual > *oValueEqual)
				return false;
			oBestValueGreaterEqual = nullptr;
			}
		}
	else
		{
		if (oBestValueLess)
			{
			ZAssert(!oBestValueLessEqual);
			if (oBestValueGreater)
				{
				ZAssert(!oBestValueGreaterEqual);
				if (*oBestValueGreater >= *oBestValueLess)
					return false;
				}
			else if (oBestValueGreaterEqual)
				{
				if (*oBestValueGreaterEqual >= *oBestValueLess)
					return false;
				}
			}
		else if (oBestValueLessEqual)
			{
			if (oBestValueGreater)
				{
				ZAssert(!oBestValueGreaterEqual);
				if (*oBestValueLessEqual <= *oBestValueGreater)
					return false;
				}
			else if (oBestValueGreaterEqual)
				{
				if (*oBestValueLessEqual < *oBestValueGreaterEqual)
					return false;
				}
			}

		// Convert a lessEqual/greaterEqual pair into an equals,
		// if the boundary value is the same for both.
		if (oBestValueLessEqual
			&& oBestValueGreaterEqual
			&& *oBestValueLessEqual == *oBestValueGreaterEqual)
			{
			oValueEqual = oBestValueLessEqual;
			oBestValueLessEqual = nullptr;
			oBestValueGreaterEqual = nullptr;
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory

ZTupleIndexFactory::~ZTupleIndexFactory()
	{}

} // namespace ZooLib
