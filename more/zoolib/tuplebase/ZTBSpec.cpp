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

#include "zoolib/tuplebase/ZTBSpec.h"

#include "zoolib/ZCompare_vector.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZTextCollator.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_STL.h"

#include <list>
#include <vector>

using std::list;
using std::set;
using std::string;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Utility functions

static string spStringFromRel(ZTBSpec::ERel iRel)
	{
	switch (iRel)
		{
		case ZTBSpec::eRel_Less: return "<";
		case ZTBSpec::eRel_LessEqual: return "<=";
		case ZTBSpec::eRel_Equal: return "==";
		case ZTBSpec::eRel_GreaterEqual: return ">=";
		case ZTBSpec::eRel_Greater: return ">";
		case ZTBSpec::eRel_Has: return "Has";
		case ZTBSpec::eRel_HasOfType: return "HasOfType";
		case ZTBSpec::eRel_Lacks: return "Lacks";
		case ZTBSpec::eRel_LacksOfType: return "LacksOfType";
		case ZTBSpec::eRel_VectorContains: return "VectorContains";
		case ZTBSpec::eRel_StringContains: return "StringContains";
		case ZTBSpec::eRel_Regex: return "re";
		default:
			break;
		}
	return "???";
	}

static ZTBSpec::ERel spRelFromString(const std::string& iString)
	{
	if (iString == "<") return ZTBSpec::eRel_Less;
	else if (iString == "<=") return ZTBSpec::eRel_LessEqual;
	else if (iString == "==") return ZTBSpec::eRel_Equal;
	else if (iString == ">=") return ZTBSpec::eRel_GreaterEqual;
	else if (iString == ">") return ZTBSpec::eRel_Greater;
	else if (iString == "Has") return ZTBSpec::eRel_Has;
	else if (iString == "HasOfType") return ZTBSpec::eRel_HasOfType;
	else if (iString == "Lacks") return ZTBSpec::eRel_Lacks;
	else if (iString == "LacksOfType") return ZTBSpec::eRel_LacksOfType;
	else if (iString == "VectorContains") return ZTBSpec::eRel_VectorContains;
	else if (iString == "StringContains") return ZTBSpec::eRel_StringContains;
	else if (iString == "re") return ZTBSpec::eRel_Regex;

	return ZTBSpec::eRel_Invalid;
	}

static void spCriterionUnionFromTuple(
	const ZTuple& iTuple, ZTBSpec::CriterionUnion& ioCriterionUnion)
	{
	const vector<ZTValue>& outerVector = iTuple.Get("Criteria").GetSeq().GetVector();

	ioCriterionUnion.resize(outerVector.size());
	ZTBSpec::CriterionUnion::iterator critListIter = ioCriterionUnion.begin();

	for (vector<ZTValue>::const_iterator outerIter = outerVector.begin();
		outerIter != outerVector.end(); ++outerIter, ++critListIter)
		{
		const ZSeq_ZooLib theSeq = (*outerIter).GetSeq();
		const vector<ZTValue>& innerVector = theSeq.GetVector();
		for (vector<ZTValue>::const_iterator inner = innerVector.begin();
			inner != innerVector.end(); ++inner)
			{
			(*critListIter).push_back(ZTBSpec::Criterion((*inner).GetTuple()));
			}
		}
	}

static ZTuple spCriterionUnionToTuple(const ZTBSpec::CriterionUnion& iCriterionUnion)
	{
	ZTuple result;

	vector<ZTValue>& outerVector = result.SetMutableVector("Criteria");
	outerVector.reserve(iCriterionUnion.size());
	outerVector.resize(iCriterionUnion.size());
	vector<ZTValue>::iterator outerIter = outerVector.begin();

	for (ZTBSpec::CriterionUnion::const_iterator critListIter = iCriterionUnion.begin(),
		critListEnd = iCriterionUnion.end();
		critListIter != critListEnd;
		++critListIter, ++outerIter)
		{
		vector<ZTValue>& innerVector = (*outerIter).MutableSeq().MutableVector();
		for (ZTBSpec::CriterionSect::const_iterator i = (*critListIter).begin(),
			theEnd = (*critListIter).end();
			i != theEnd; ++i)
			{
			ZTuple aTuple = (*i).AsTuple();
			innerVector.push_back(aTuple);
			}
		}
	return result;
	}

static inline void spWriteCount(const ZStreamW& iStreamW, uint32 iCount)
	{ iStreamW.WriteCount(iCount); }

static inline uint32 spReadCount(const ZStreamR& iStreamR)
	{ return iStreamR.ReadCount(); }

static void spReadString(const ZStreamR& iStreamR, string& oString)
	{
	uint32 theCount = spReadCount(iStreamR);
	oString.resize(theCount);
	if (theCount)
		iStreamR.Read(const_cast<char*>(oString.data()), theCount);
	}

static void spWriteString(const ZStreamW& iStreamW, const string& iString)
	{
	size_t theSize = iString.size();
	spWriteCount(iStreamW, theSize);
	if (theSize)
		iStreamW.Write(iString.data(), theSize);
	}

static void spCriterionUnionFromStream(const ZStreamR& iStreamR,
	ZTBSpec::CriterionUnion& ioCriterionUnion)
	{
	if (uint32 outerCount = spReadCount(iStreamR))
		{
		ioCriterionUnion.reserve(outerCount);
		ioCriterionUnion.resize(outerCount);
		for (ZTBSpec::CriterionUnion::iterator outer = ioCriterionUnion.begin();
			outer != ioCriterionUnion.end(); ++outer)
			{
			if (uint32 innerCount = spReadCount(iStreamR))
				{
				(*outer).reserve(innerCount);
				while (innerCount--)
					(*outer).push_back(ZTBSpec::Criterion(iStreamR));
				}
			}
		}
	}

static void spCriterionUnionToStream(const ZStreamW& iStreamW,
	const ZTBSpec::CriterionUnion& iCriterionUnion)
	{
	spWriteCount(iStreamW, iCriterionUnion.size());
	for (ZTBSpec::CriterionUnion::const_iterator outer = iCriterionUnion.begin();
		outer != iCriterionUnion.end(); ++outer)
		{
		spWriteCount(iStreamW, (*outer).size());
		for (ZTBSpec::CriterionSect::const_iterator inner = (*outer).begin();
			inner != (*outer).end(); ++inner)
			{
			(*inner).ToStream(iStreamW);
			}
		}
	}

/** Remove tautologies, collapse overlapping ranges. If there are any contradictions return
false. If ioCrit is a tautology empty ioCrit and return true. Otherwise return true. */
static bool spSimplify(ZTBSpec::CriterionSect& ioCrit)
	{
	// AG2005-10-02. For the moment I'm disabling the simplification of criteria. The
	// precise technique I'm using causes a lot of allocations/deallocations and
	// we end up spending oodles of time in free/malloc, far more than we save when using
	// the simplified form. I have in mind a more complicated and less memory manager-intensive
	// technique, but that can wait a bit.
	return true;
	#if 0
	// Look at each Criterion in turn.
	for (vector<ZTBSpec::Criterion>::iterator i = ioCrit.begin();
		i != ioCrit.end(); /*no inc*/)
		{
		ZTValue smallestLess, smallestLessEqual, largestGreater, largestGreaterEqual;
		bool gotSmallestLess = false;
		bool gotSmallestLessEqual = false;
		bool gotLargestGreater = false;
		bool gotLargestGreaterEqual = false;

		// For simplicity we simplify only if strength is zero
		if ((*i).fComparator.fStrength != 0)
			{
			++i;
			continue;
			}

		switch ((*i).fComparator.fRel)
			{
			case ZTBSpec::eRel_Less:
				smallestLess = (*i).fTValue;
				gotSmallestLess = true;
				break;
			case ZTBSpec::eRel_LessEqual:
				smallestLessEqual = (*i).fTValue;
				gotSmallestLessEqual = true;
				break;
			case ZTBSpec::eRel_Equal:
				smallestLessEqual = (*i).fTValue;
				gotSmallestLessEqual = true;
				largestGreaterEqual = (*i).fTValue;
				gotLargestGreaterEqual = true;
				break;
			case ZTBSpec::eRel_GreaterEqual:
				largestGreaterEqual = (*i).fTValue;
				gotLargestGreaterEqual = true;
				break;
			case ZTBSpec::eRel_Greater:
				largestGreater = (*i).fTValue;
				gotLargestGreater = true;
				break;
			default:
				++i;
				continue;
			}

		string propName = (*i).fPropName;

		// The criteria is one for which we can easily spot contradictions,
		// so erase the criteria (we'll reinstate equivalents later).
		i = ioCrit.erase(i);

		// Find subsequent criteria that could be simplified with this one.
		bool isContradiction = false;
		bool isTautology = false;
		for (vector<ZTBSpec::Criterion>::iterator j = i; j != ioCrit.end(); /*no inc*/)
			{
			if (propName != (*j).fPropName || 0 != (*j).fComparator.fStrength)
				{
				++j;
				continue;
				}

			switch ((*j).fComparator.fRel)
				{
				case ZTBSpec::eRel_Less:
					{
					if (gotSmallestLess)
						{
						if (smallestLess > (*j).fTValue)
							smallestLess = (*j).fTValue;
						}
					else if (gotSmallestLessEqual)
						{
						if (smallestLessEqual >= (*j).fTValue)
							{
							smallestLess = (*j).fTValue;
							gotSmallestLessEqual = false;
							gotSmallestLess = true;
							}
						}
					else
						{
						smallestLess = (*j).fTValue;
						gotSmallestLess = true;
						}
					break;
					}
				case ZTBSpec::eRel_Greater:
					{
					if (gotLargestGreater)
						{
						if (largestGreater < (*j).fTValue)
							largestGreater = (*j).fTValue;
						}
					else if (gotLargestGreaterEqual)
						{
						if (largestGreaterEqual <= (*j).fTValue)
							{
							largestGreater = (*j).fTValue;
							gotLargestGreaterEqual = false;
							gotLargestGreater = true;
							}
						}
					else
						{
						largestGreater = (*j).fTValue;
						gotLargestGreater = true;
						}
					}
					break;
				case ZTBSpec::eRel_LessEqual:
				case ZTBSpec::eRel_Equal:
				case ZTBSpec::eRel_GreaterEqual:
					break;
				default:
					{
					++j;
					continue;
					}
				}

			if ((*j).fComparator.fRel == ZTBSpec::eRel_Equal
				|| (*j).fComparator.fRel == ZTBSpec::eRel_LessEqual)
				{
				if (gotSmallestLess)
					{
					if (smallestLess > (*j).fTValue)
						{
						gotSmallestLess = false;
						gotSmallestLessEqual = true;
						smallestLessEqual = (*j).fTValue;
						}
					}
				else if (gotSmallestLessEqual)
					{
					if (smallestLessEqual > (*j).fTValue)
						smallestLessEqual = (*j).fTValue;
					}
				else
					{
					gotSmallestLessEqual = true;
					smallestLessEqual = (*j).fTValue;
					}
				}

			if ((*j).fComparator.fRel == ZTBSpec::eRel_Equal
				|| (*j).fComparator.fRel == ZTBSpec::eRel_GreaterEqual)
				{
				if (gotLargestGreater)
					{
					if (largestGreater < (*j).fTValue)
						{
						gotLargestGreater = false;
						gotLargestGreaterEqual = true;
						largestGreaterEqual = (*j).fTValue;
						}
					}
				else if (gotLargestGreaterEqual)
					{
					if (largestGreaterEqual < (*j).fTValue)
						largestGreaterEqual = (*j).fTValue;
					}
				else
					{
					gotLargestGreaterEqual = true;
					largestGreaterEqual = (*j).fTValue;
					}
				}
			// Erase this criteria, need to update i too, in case the vector is reallocated.
			size_t offset = i - ioCrit.begin();
			j = ioCrit.erase(j);
			i = offset + ioCrit.begin();
			}

		// Make sure we haven't messed up.
		ZAssertStop(2, (!gotSmallestLess || !gotSmallestLessEqual)
			&& (!gotLargestGreater || !gotLargestGreaterEqual));

		if (gotSmallestLess)
			{
			if (gotLargestGreater)
				{
				if (!(smallestLess > largestGreater))
					isContradiction = true;
				}
			else if (gotLargestGreaterEqual)
				{
				if (!(smallestLess > largestGreaterEqual))
					isContradiction = true;
				}
			}
		else if (gotSmallestLessEqual)
			{
			if (gotLargestGreater)
				{
				if (!(smallestLessEqual > largestGreater))
					isContradiction = true;
				}
			else if (gotLargestGreaterEqual)
				{
				if (!(smallestLessEqual >= largestGreaterEqual))
					isContradiction = true;
				}
			}

		if (isContradiction)
			return false;

		if (isTautology)
			{
			// We don't actually recognize tautologies yet, so isTautology
			// never gets set true and we won't execute this.
			ioCrit.clear();
			return true;
			}

		// It's neither a contradiction nor a tautology. So reinstate the
		// one or two entries we need to represent the criteria.
		if (gotSmallestLessEqual && gotLargestGreaterEqual
			&& smallestLessEqual == largestGreaterEqual)
			{
			i = ioCrit.insert(i,
				ZTBSpec::Criterion(propName, ZTBSpec::eRel_Equal, smallestLessEqual));
			}
		else
			{
			if (gotSmallestLess)
				{
				i = ioCrit.insert(i,
					ZTBSpec::Criterion(propName, ZTBSpec::eRel_Less, smallestLess));
				}
			else if (gotSmallestLessEqual)
				{
				i = ioCrit.insert(i,
					ZTBSpec::Criterion(propName, ZTBSpec::eRel_LessEqual, smallestLessEqual));
				}

			if (gotLargestGreater)
				{
				i = ioCrit.insert(i,
					ZTBSpec::Criterion(propName, ZTBSpec::eRel_Greater, largestGreater));
				}
			else if (gotLargestGreaterEqual)
				{
				i = ioCrit.insert(i,
					ZTBSpec::Criterion(propName, ZTBSpec::eRel_GreaterEqual, largestGreaterEqual));
				}
			}

		// Move on to the next Criterion.
		++i;
		}
	return true;
#endif
	}

static void spSimplify(ZTBSpec::CriterionUnion& ioCritList)
	{
#if 0
	for (vector<vector<ZTBSpec::Criterion> >::iterator outer = ioCritVector.begin();
		outer != ioCritVector.end(); /*no inc*/)
		{
		if (!spSimplify(*outer))
			{
			// *outer represents a contradiction, so drop it.
			outer = ioCritVector.erase(outer);
			}
		else if ((*outer).empty())
			{
			// *outer is a tautolgy, so reconfigure ioCritVector to indicate a
			// ZTBSpec::sAny and return.
			ioCritVector.erase(ioCritVector.begin() + 1, ioCritVector.end());
			ioCritVector.front().clear();
			return;
			}
		else
			{
			++outer;
			}
		}
#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Comparator

/** \class ZTBSpec::Comparator

Comparator describes the comparison to be made between a property of a candidate tuple and
a ZTValue. The actual comparison is specified by a ZTBSpec::ERel, further qualified by
an integer strength. The strength is used when comparing one string against another, for
the relationships \c eRel_Less, \c eRel_LessEqual, \c eRel_Equal, \c eRel_GreaterEqual,
\c eRel_Greater, \c eRel_StringContains and \c eRel_Regex.

The strength is used to initialize a ZTextCollator. Generally you'll be using strength 0,
for exact string matching, or strength 1, for case and diacritic-insensitive matching.
*/

ZTBSpec::Comparator::Comparator(const ZStreamR& iStreamR)
	{
	fRel = (ERel)iStreamR.ReadUInt8();
	fStrength = iStreamR.ReadUInt8();
	}

ZTBSpec::Comparator::Comparator()
:	fRel(eRel_Invalid),
	fStrength(0)
	{}

ZTBSpec::Comparator::Comparator(ERel iRel, int iStrength)
:	fRel(iRel),
	fStrength(iStrength)
	{}

ZTBSpec::Comparator::Comparator(ERel iRel)
:	fRel(iRel),
	fStrength(0)
	{}

void ZTBSpec::Comparator::ToStream(const ZStreamW& iStreamW) const
	{
	iStreamW.WriteUInt8(fRel);
	iStreamW.WriteUInt8(fStrength);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Criterion::Rep

ZTBSpec::Criterion::Rep::Rep(const ZStreamR& iStreamR)
:	fPropName(iStreamR),
	fComparator(iStreamR),
	fTValue(iStreamR)
	{}

ZTBSpec::Criterion::Rep::Rep(const ZTuple& iTuple)
:	fPropName(iTuple.GetString("PropName")),
	fComparator(spRelFromString(iTuple.GetString("Rel")), iTuple.GetInt32("Strength")),
	fTValue(iTuple.Get("Value"))
	{}

ZTBSpec::Criterion::Rep::Rep(
	const ZTName& iPropName, const Comparator& iComparator, const ZTValue& iTV)
:	fPropName(iPropName),
	fComparator(iComparator),
	fTValue(iTV)
	{}

ZTBSpec::Criterion::Rep::Rep(const ZTName& iPropName)
:	fPropName(iPropName),
	fComparator(eRel_Has)
	{}

ZTBSpec::Criterion::Rep::Rep(const ZTName& iPropName, ZType iType)
:	fPropName(iPropName),
	fComparator(eRel_HasOfType),
	fTValue(iType)
	{}

ZTBSpec::Criterion::Rep::Rep(const ZTName& iPropName, ERel iRel, const ZTValue& iTV)
:	fPropName(iPropName),
	fComparator(iRel),
	fTValue(iTV)
	{}

ZTBSpec::Criterion::Rep::Rep(
	const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV)
:	fPropName(iPropName),
	fComparator(iRel, iStrength),
	fTValue(iTV)
	{}

ZTuple ZTBSpec::Criterion::Rep::AsTuple() const
	{
	ZTuple result;
	result.SetString("PropName", fPropName.AsString());
	result.SetString("Rel", spStringFromRel(fComparator.fRel));
	if (fComparator.fStrength)
		result.SetInt32("Strength", fComparator.fStrength);
	if (fTValue)
		result.Set("Value", fTValue);
	return result;
	}

void ZTBSpec::Criterion::Rep::ToStream(const ZStreamW& iStreamW) const
	{
	fPropName.ToStream(iStreamW);
	fComparator.ToStream(iStreamW);
	fTValue.ToStream(iStreamW);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Criterion

/** \class ZTBSpec::Criterion

Criterion specifies a the name of a property to be examined, and the ZTBSpec::Comparator
to be used to make that examination.
*/

ZTBSpec::Criterion::Criterion(const ZStreamR& iStreamR)
:	fRep(new Rep(iStreamR))
	{}

ZTBSpec::Criterion::Criterion(const ZTuple& iTuple)
:	fRep(new Rep(iTuple))
	{}

ZTBSpec::Criterion::Criterion(
	const ZTName& iPropName, const Comparator& iComparator, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iComparator, iTV))
	{}

ZTBSpec::Criterion::Criterion(const ZTName& iPropName)
:	fRep(new Rep(iPropName))
	{}

ZTBSpec::Criterion::Criterion(const ZTName& iPropName, ZType iType)
:	fRep(new Rep(iPropName, iType))
	{}

ZTBSpec::Criterion::Criterion(const ZTName& iPropName, ERel iRel, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iRel, iTV))
	{}

ZTBSpec::Criterion::Criterion(
	const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iRel, iStrength, iTV))
	{}

ZTuple ZTBSpec::Criterion::AsTuple() const
	{ return fRep->AsTuple(); }

void ZTBSpec::Criterion::ToStream(const ZStreamW& iStreamW) const
	{ fRep->ToStream(iStreamW); }

bool ZTBSpec::Criterion::Matches(const ZTuple& iTuple) const
	{
	ZTuple::const_iterator propIter = iTuple.IteratorOf(this->GetPropName());
	if (propIter == iTuple.end())
		{
		// iTuple does not have a property named fPropName. For
		// 'Lacks' and 'LacksOfType' relationships that means
		// we match iTuple, for all other relationships we don't.
		return this->GetComparator().fRel == eRel_Lacks
			|| this->GetComparator().fRel == eRel_LacksOfType;
		}

	switch (this->GetComparator().fRel)
		{
		case eRel_Has:
			{
			return true;
			}
		case eRel_HasOfType:
			{
			return iTuple.PGet(propIter)->TypeOf() == this->GetTValue().GetType();
			}
		case eRel_Lacks:
			{
			// iTuple has a property named fPropName,
			// so it doesn't lack a property named fPropName.
			return false;
			}
		case eRel_LacksOfType:
			{
			return iTuple.PGet(propIter)->TypeOf() != this->GetTValue().GetType();
			}
		case eRel_VectorContains:
			{
			ZSeq_ZooLib theSeq;
			if (iTuple.Get(propIter).QGetSeq(theSeq))
				{
				const vector<ZTValue>& theVector = theSeq.GetVector();
				for (vector<ZTValue>::const_iterator i = theVector.begin();
					i != theVector.end(); ++i)
					{
					if (*i == this->GetTValue())
						return true;
					}
				}
			return false;
			}
		case eRel_StringContains:
			{
			string pattern;
			if (!this->GetTValue().QGetString(pattern) || pattern.empty())
				return true;

			string target;
			if (not iTuple.QGetString(propIter, target) || target.empty())
				return false;

			if (!fRep->fTextCollator)
				{
				fRep->fTextCollator =
					ZTextCollator(this->GetComparator().fStrength);
				}
			// Assume our collator is NativeUTF16 and do the conversion here
			// to isolate its effects for profiling.
			const string16 pattern16 = ZUnicode::sAsUTF16(pattern);
			const string16 target16 = ZUnicode::sAsUTF16(target);
			return fRep->fTextCollator.Contains(pattern16.data(), pattern16.size(),
				target16.data(), target16.size());
			}
		case eRel_Regex:
			{
			#if ZCONFIG_API_Enabled(Regex)
				string target;
				if (not iTuple.GetString(propIter, target) || target.empty())
					return false;

				if (!fRep->fRegex)
					{
					fRep->fRegex =
						ZRegex(this->GetTValue().GetString(), this->GetComparator().fStrength);
					}

				if (!fRep->fRegex)
					return false;

				return fRep->fRegex.Matches(target);
			#else
				return false;
			#endif
			}
		default:
			{
			const ZTValue& leftValue = iTuple.Get(propIter);
			ZType leftType = leftValue.TypeOf();
			ZType rightType = this->GetTValue().TypeOf();
			if (this->GetComparator().fRel == eRel_Equal)
				{
				if (leftType != rightType)
					{
					return false;
					}
				else
					{
					if (leftType == eZType_String && this->GetComparator().fStrength != 0)
						{
						// We're doing string compares where the strength is non zero and
						// so we need to use a collator to do the comparison.
						if (!fRep->fTextCollator)
							{
							fRep->fTextCollator =
								ZTextCollator(this->GetComparator().fStrength);
							}

						return fRep->fTextCollator.Equals(
							leftValue.GetString(), this->GetTValue().GetString());
						}
					else
						{
						// The type matches, so do the appropriate comparison.
						// return leftValue.pUncheckedEqual(this->GetTValue());
						return leftValue == this->GetTValue();
						}
					}
				}
			else
				{
				int compare;
				if (leftType != rightType)
					{
					compare = int(leftType) - int(rightType);
					}
				else
					{
					if (leftType == eZType_String && this->GetComparator().fStrength != 0)
						{
						// We're doing string compares where the strength is non zero and
						// so we need to use a collator to do the comparison.
						if (!fRep->fTextCollator)
							{
							fRep->fTextCollator =
								ZTextCollator(this->GetComparator().fStrength);
							}

						compare = fRep->fTextCollator.Compare(
							leftValue.GetString(), this->GetTValue().GetString());
						}
					else
						{
						// The type matches, so do the appropriate comparison.
						// compare = leftValue.pUncheckedCompare(this->GetTValue());
						compare = leftValue.Compare(this->GetTValue());
						}
					}
				switch (this->GetComparator().fRel)
					{
					case eRel_Less: return compare < 0;
					case eRel_LessEqual: return compare <= 0;
					case eRel_GreaterEqual: return compare >= 0;
					case eRel_Greater: return compare > 0;
					}
				}
			}
		}
	// Can't get here.
	ZUnimplemented();
	return false;
	}

int ZTBSpec::Criterion::Compare(const Criterion& iOther) const
	{
	Rep* myRep = fRep.Get();
	Rep* otherRep = iOther.fRep.Get();

	if (int compare = myRep->fComparator.Compare(otherRep->fComparator))
		return compare;

	if (int compare = myRep->fPropName.Compare(otherRep->fPropName))
		return compare;

	return myRep->fTValue.Compare(otherRep->fTValue);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Rep

class ZTBSpec::Rep : public ZCountedWithoutFinalize
	{
public:
	Rep();
	Rep(const Rep& iOther);
	Rep(bool iAny);
	Rep(const ZTName& iPropName, ERel iRel, const ZTValue& iTV);
	Rep(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV);
	Rep(const ZTName& iPropName, Comparator iComparator, const ZTValue& iTV);
	Rep(const Criterion& iCriterion);
	Rep(const CriterionSect& iCriterionSect);

	CriterionUnion fCriterionUnion;
	};

ZTBSpec::Rep::Rep()
	{}

ZTBSpec::Rep::Rep(const Rep& iOther)
:	fCriterionUnion(iOther.fCriterionUnion)
	{}

ZTBSpec::Rep::Rep(bool iAny)
:	fCriterionUnion(iAny ? 1 : 0)
	{}

ZTBSpec::Rep::Rep(const ZTName& iPropName, ERel iRel, const ZTValue& iTV)
:	fCriterionUnion(1)
	{
	fCriterionUnion.back().push_back(Criterion(iPropName, iRel, iTV));
	}

ZTBSpec::Rep::Rep(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV)
:	fCriterionUnion(1)
	{
	fCriterionUnion.back().push_back(Criterion(iPropName, iRel, iStrength, iTV));
	}

ZTBSpec::Rep::Rep(const ZTName& iPropName, Comparator iComparator, const ZTValue& iTV)
:	fCriterionUnion(1)
	{
	fCriterionUnion.back().push_back(Criterion(iPropName, iComparator, iTV));
	}

ZTBSpec::Rep::Rep(const Criterion& iCriterion)
:	fCriterionUnion(1)
	{
	fCriterionUnion.back().push_back(iCriterion);
	}

ZTBSpec::Rep::Rep(const CriterionSect& iCriterionSect)
:	fCriterionUnion(1, iCriterionSect)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec

/**
\class ZTBSpec
\nosubgrouping
\ingroup Tuplebase
\ingroup Tuplesoup
\ingroup Tuplestore
\sa Tuplebase
\sa Tuplesoup
\sa Tuplestore

\note Despite its name ZTBSpec, like ZTBQuery, knows nothing about
tuplebases, tuplesoups or tuplestores. It's simply a way to create and manage a suite
of criteria against which a tuple can be checked to determine if they're satisfied. As such
it probably ought to be called ZTupleSpec or ZTSpec.

A ZTBSpec is constructed by providing a property name and a Criterion against which
the named property of a candidate tuple should be tested. A ZTBSpec can also be constructed
by intersecting two ZTBSpecs, or by unioning two ZTBSpecs.

A Criterion holds a property name, a comparator, and for binary comparators a value to be passed
to it. The simplest comparators are \c Has and \c Lacks. They are unary, and simply check that the
candidate tuple has, or lacks, a property with that name. All other comparators are binary, and
check a candidate tuple's named property against a particular value.

The group documentation \ref Tuplebase discusses in more detail how to construct ZTBSpecs from
primitive values and comparators, and then to combine them using intersection (operator&)
and union (operator|).

The underlying representation is a single ZTBSpec::CriterionUnion, which itself holds a
list of ZTBSpec::CriterionSect, which holds a list of Criterion; it's the union of
intersections of criteria.

A ZTBSpec::CriterionSect matches a tuple if all of its Criterion match the tuple, so
if its list is empty then it matches any tuple.

A ZTBSpec::CriterionUnion matches a tuple if at least one of its ZTBSpec::CriterionSect matches
the tuple. So a ZTBSpec::CriterionUnion containing no ZTBSpec::CriterionSect will match no tuples.
A ZTBSpec::CriterionUnion containing a single ZTBSpec::CriterionSect which itself contains no
Criterion will match any tuple.

The default ZTBSpec constructor creates a 'none' ZTBSpec, as does ZTBSpec::sNone, and
ZTBSpec(bool iAny) with \c iAny = \c false.

An 'any' ZTBSpec can be constructed by calling ZTBSpec::sAny, or ZTBSpec(bool iAny)
with \c iAny = \c true.

Some useful identities:
	- someSpec & ZTBSpec::sAny() == someSpec
	- someSpec & ZTBSpec::sNone() == ZTBSpec::sNone()
	- someSpec | ZTBSpec::sAny() == ZTBSpec::sAny()
	- someSpec | ZTBSpec::sNone() == someSpec
*/

static const ZTBSpec::CriterionUnion spCriterionUnion_Empty;
static const ZRef<ZTBSpec::Rep> spRepAny = new ZTBSpec::Rep(true);

ZTBSpec::ZTBSpec()
	{}

ZTBSpec::ZTBSpec(const ZTBSpec& iOther)
:	fRep(iOther.fRep)
	{}

ZTBSpec::ZTBSpec(bool iAny)
	{
	if (iAny)
		fRep = spRepAny;
	}

ZTBSpec& ZTBSpec::operator=(const ZTBSpec& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

// ZTBSpec's destructor needs to be defined explicitly because ZTBSpec::Rep is not declared
// in ZTBSpec.h and so an automatically generated destructor isn't usable by other code.
ZTBSpec::~ZTBSpec()
	{}


/**
Creates a spec which returns true from ZTBSpec::Matches when that candidate tuple's
satisfies Criterion \a iCriterion.
*/
ZTBSpec::ZTBSpec(const Criterion& iCriterion)
:	fRep(new Rep(iCriterion))
	{}


/**
Creates a spec which returns true from ZTBSpec::Matches when the candidate
tuple's property named \a iPropName satisfies relationship \a iRel with respect
to \a iTV.
*/
ZTBSpec::ZTBSpec(const ZTName& iPropName, ERel iRel, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iRel, iTV))
	{}


/**
Creates a spec which returns true from ZTBSpec::Matches when the candidate tuple's
property named \a iPropName satisfies relationship \a iRel with strength
\a iStrength with respect to \a iTV. */
ZTBSpec::ZTBSpec(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iRel, iStrength, iTV))
	{}


/**
Creates a spec which returns true from ZTBSpec::Matches when the candidate tuple's
property named \a iPropName satisfies Comparator \a iComparator with respect to \a iTV.
*/
ZTBSpec::ZTBSpec(const ZTName& iPropName, Comparator iComparator, const ZTValue& iTV)
:	fRep(new Rep(iPropName, iComparator, iTV))
	{}


/**
Creates a spec which returns true from ZTBSpec::Matches when that candidate tuple's
satisfies CriterionSect \a iCriterionSect.
*/
ZTBSpec::ZTBSpec(const CriterionSect& iCriterionSect)
:	fRep(new Rep(iCriterionSect))
	{}

ZTBSpec::ZTBSpec(const ZTuple& iTuple)
:	fRep(new Rep)
	{
	spCriterionUnionFromTuple(iTuple, fRep->fCriterionUnion);
	spSimplify(fRep->fCriterionUnion);
	}

ZTBSpec::ZTBSpec(const ZStreamR& iStreamR)
:	fRep(new Rep)
	{
	spCriterionUnionFromStream(iStreamR, fRep->fCriterionUnion);
	spSimplify(fRep->fCriterionUnion);
	}

ZTuple ZTBSpec::AsTuple() const
	{
	if (fRep)
		return spCriterionUnionToTuple(fRep->fCriterionUnion);
	else
		return spCriterionUnionToTuple(spCriterionUnion_Empty);
	}

void ZTBSpec::ToStream(const ZStreamW& iStreamW) const
	{
	if (fRep)
		spCriterionUnionToStream(iStreamW, fRep->fCriterionUnion);
	else
		spCriterionUnionToStream(iStreamW, spCriterionUnion_Empty);
	}

int ZTBSpec::Compare(const ZTBSpec& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We share a rep, and are thus equal.
		return 0;
		}
	else if (!fRep)
		{
		// We don't have a rep, but iOther must have.
		return -1;
		}
	else if (not iOther.fRep)
		{
		// We do have a rep, but iOther doesn't.
		return 1;
		}
	else
		{
		return sCompare_T(fRep->fCriterionUnion, iOther.fRep->fCriterionUnion);
		}
	}

static inline void spReserve(vector<ZTBSpec::Criterion>& ioVector, size_t iSize)
	{ ioVector.reserve(iSize); }

static inline void spReserve(list<ZTBSpec::Criterion>& ioList, size_t iSize)
	{}

static void spCrossProduct(const ZTBSpec::CriterionUnion& iSourceA,
	const ZTBSpec::CriterionUnion& iSourceB,
	ZTBSpec::CriterionUnion& oDest)
	{
	for (ZTBSpec::CriterionUnion::const_iterator iterSourceA = iSourceA.begin();
		iterSourceA != iSourceA.end(); ++iterSourceA)
		{
		for (ZTBSpec::CriterionUnion::const_iterator iterSourceB = iSourceB.begin();
			iterSourceB != iSourceB.end(); ++iterSourceB)
			{
			oDest.push_back(*iterSourceA);
			ZTBSpec::CriterionSect& temp = oDest.back();
			spReserve(temp, temp.size() + (*iterSourceB).size());
			temp.insert(temp.end(), (*iterSourceB).begin(), (*iterSourceB).end());
			}
		}
	}

ZTBSpec ZTBSpec::operator&(const ZTBSpec& iOther) const
	{
	if (this->IsNone() || iOther.IsAny())
		return *this;

	if (this->IsAny() || iOther.IsNone())
		return iOther;

	CriterionUnion crossProduct;
	spCrossProduct(fRep->fCriterionUnion, iOther.fRep->fCriterionUnion, crossProduct);
	spSimplify(crossProduct);
	// Here we use our special constructor that swaps its empty list
	// with the passed in list.
	return ZTBSpec(crossProduct, true);
	}

ZTBSpec& ZTBSpec::operator&=(const ZTBSpec& iOther)
	{
	if (this->IsNone() || iOther.IsAny())
		return *this;

	if (this->IsAny() || iOther.IsNone())
		{
		fRep = iOther.fRep;
		return *this;
		}

	this->MakeFresh();
	CriterionUnion sourceA;
	fRep->fCriterionUnion.swap(sourceA);
	// sourceA now holds what fCriterionUnion held, and fCriterionUnion is empty.
	spCrossProduct(sourceA, iOther.fRep->fCriterionUnion, fRep->fCriterionUnion);
	spSimplify(fRep->fCriterionUnion);
	return *this;
	}

ZTBSpec ZTBSpec::operator|(const ZTBSpec& iOther) const
	{
	if (this->IsAny() || iOther.IsNone())
		return *this;

	if (this->IsNone() || iOther.IsAny())
		return iOther;

	CriterionUnion theCriterionUnion = fRep->fCriterionUnion;
	theCriterionUnion.insert(theCriterionUnion.end(),
		iOther.fRep->fCriterionUnion.begin(), iOther.fRep->fCriterionUnion.end());
	spSimplify(theCriterionUnion);
	return ZTBSpec(theCriterionUnion, true);
	}

ZTBSpec& ZTBSpec::operator|=(const ZTBSpec& iOther)
	{
	if (this->IsAny() || iOther.IsNone())
		return *this;

	if (this->IsNone() || iOther.IsAny())
		{
		fRep = iOther.fRep;
		return *this;
		}

	this->MakeFresh();
	fRep->fCriterionUnion.insert(fRep->fCriterionUnion.end(),
		iOther.fRep->fCriterionUnion.begin(), iOther.fRep->fCriterionUnion.end());
	spSimplify(fRep->fCriterionUnion);
	return *this;
	}

static inline bool spIsCheap(ZTBSpec::ERel iRel)
	{ return iRel < ZTBSpec::eRel_VectorContains; }

bool ZTBSpec::Matches(const ZTuple& iTuple) const
	{
	if (fRep)
		{
		for (CriterionUnion::const_iterator outer = fRep->fCriterionUnion.begin(),
			outerEnd = fRep->fCriterionUnion.end();
			outer != outerEnd; ++outer)
			{
			bool allOkay = true;
			for (CriterionSect::const_iterator inner = (*outer).begin(), innerEnd = (*outer).end();
				inner != innerEnd; ++inner)
				{
				if (spIsCheap(inner->GetComparator().fRel))
					{
					if (not inner->Matches(iTuple))
						{
						allOkay = false;
						break;
						}
					}
				}

			if (allOkay)
				{
				for (CriterionSect::const_iterator inner = (*outer).begin(), innerEnd = (*outer).end();
					inner != innerEnd; ++inner)
					{
					if (!spIsCheap(inner->GetComparator().fRel))
						{
						if (not inner->Matches(iTuple))
							{
							allOkay = false;
							break;
							}
						}
					}
				}

			if (allOkay)
				return true;
			}
		}
	return false;
	}

bool ZTBSpec::IsAny() const
	{ return fRep && fRep->fCriterionUnion.size() == 1 && fRep->fCriterionUnion.front().empty(); }

bool ZTBSpec::IsNone() const
	{ return !fRep || fRep->fCriterionUnion.empty(); }

const ZTBSpec::CriterionUnion& ZTBSpec::GetCriterionUnion() const
	{
	if (fRep)
		return fRep->fCriterionUnion;
	return spCriterionUnion_Empty;
	}

void ZTBSpec::GetPropNames(std::set<std::string>& oPropNames) const
	{
	oPropNames.clear();
	if (!fRep)
		return;

	for (CriterionUnion::const_iterator outer = fRep->fCriterionUnion.begin(),
		outerEnd = fRep->fCriterionUnion.end();
		outer != outerEnd; ++outer)
		{
		for (CriterionSect::const_iterator inner = (*outer).begin(), innerEnd = (*outer).end();
			inner != innerEnd;
			++inner)
			{
			oPropNames.insert((*inner).GetPropName().AsString());
			}
		}
	}

void ZTBSpec::GetPropNames(std::set<ZTName>& oPropNames) const
	{
	oPropNames.clear();
	if (!fRep)
		return;

	for (CriterionUnion::const_iterator outer = fRep->fCriterionUnion.begin(),
		outerEnd = fRep->fCriterionUnion.end();
		outer != outerEnd; ++outer)
		{
		for (CriterionSect::const_iterator inner = outer->begin(), innerEnd = outer->end();
			inner != innerEnd;
			++inner)
			{
			oPropNames.insert(inner->GetPropName());
			}
		}
	}

void ZTBSpec::MakeFresh()
	{
	if (!fRep)
		fRep = new Rep;
	else if (fRep->IsShared())
		fRep = new Rep(*fRep.Get());
	}

/**
This constructor is used by operator& and operator|. Note that after executing
\a ioCriterionUnion will be empty.
*/
ZTBSpec::ZTBSpec(CriterionUnion& ioCriterionUnion, bool iKnowWhatIAmDoing)
:	fRep(new Rep)
	{
	ZAssertStop(1, iKnowWhatIAmDoing);
	fRep->fCriterionUnion.swap(ioCriterionUnion);
	}

} // namespace ZooLib
