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

#include "zoolib/ZTupleIndex_FullText.h"

#if ZCONFIG_SPI_Enabled(ICU)

#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZUtil_STL.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_FullText::CPChunk

ZTupleIndex_FullText::CPChunk::CPChunk(UTF32 iCPs[3])
:	fCP0(iCPs[0]),
	fCP1(iCPs[1]),
	fCP2(iCPs[2])
	{}

ZTupleIndex_FullText::CPChunk::CPChunk(UTF32 iCP0, UTF32 iCP1, UTF32 iCP2)
:	fCP0(iCP0),
	fCP1(iCP1),
	fCP2(iCP2)
	{}

bool ZTupleIndex_FullText::CPChunk::operator<(const CPChunk& iOther) const
	{
	if (fCP0 < iOther.fCP0)
		return true;
	else if (fCP0 > iOther.fCP0)
		return false;
	else if (fCP1 < iOther.fCP1)
		return true;
	else if (fCP1 > iOther.fCP1)
		return false;
	else
		return fCP2 < iOther.fCP2;
	}

bool ZTupleIndex_FullText::CPChunk::operator==(const CPChunk& iOther) const
	{
	return fCP0 == iOther.fCP0
		&& fCP1 == iOther.fCP1
		&& fCP2 == iOther.fCP2;
	}

bool ZTupleIndex_FullText::CPChunk::operator!=(const CPChunk& iOther) const
	{
	return fCP0 != iOther.fCP0
		|| fCP1 != iOther.fCP1
		|| fCP2 != iOther.fCP2;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_FullText

ZTupleIndex_FullText::ZTupleIndex_FullText(const ZTName& iPropName)
:	fCountTuples(0),
	fPropName(iPropName)
	{}

void ZTupleIndex_FullText::Add(uint64 iID, const ZTuple* iTuple)
	{
	ZTuple::const_iterator propIter = iTuple->IteratorOf(fPropName);
	if (propIter == iTuple->end())
		return;

	if (iTuple->TypeOf(propIter) != eZType_String)
		return;

	++fCountTuples;
	set<uint64> tupleIDSet;
	tupleIDSet.insert(iID);

	set<CPChunk> theCPChunks;
	sGetCPChunks(iTuple->GetString(propIter), theCPChunks);

	for (set<CPChunk>::iterator i = theCPChunks.begin(); i != theCPChunks.end(); ++i)
		{
		const CPChunk& curCPChunk = *i;
		map<CPChunk, set<uint64> >::iterator iterMap = fMap.lower_bound(curCPChunk);
		if (iterMap == fMap.end() || iterMap->first != curCPChunk)
			{
			fMap.insert(iterMap, pair<CPChunk, set<uint64> >(curCPChunk, tupleIDSet));
			}
		else
			{
			iterMap->second.insert(iID);
			}
		}
	}

void ZTupleIndex_FullText::Remove(uint64 iID, const ZTuple* iTuple)
	{
	ZTuple::const_iterator propIter = iTuple->IteratorOf(fPropName);
	if (propIter == iTuple->end())
		return;

	if (iTuple->TypeOf(propIter) != eZType_String)
		return;

	--fCountTuples;

	set<CPChunk> theCPChunks;
	sGetCPChunks(iTuple->GetString(propIter), theCPChunks);

	for (set<CPChunk>::iterator i = theCPChunks.begin(); i != theCPChunks.end(); ++i)
		{
		map<CPChunk, set<uint64> >::iterator iterMap = fMap.find(*i);
		ZAssert(iterMap != fMap.end());

		ZUtil_STL::sEraseMustContain(1, iterMap->second, iID);
		}
	}

static void sGetPattern(const ZTName& iPropName,
	const vector<const ZTBSpec::Criterion*>& iCriteria, string& oPattern)
	{
	for (vector<const ZTBSpec::Criterion*>::const_iterator
		critIter = iCriteria.begin(), theEnd = iCriteria.end();
		critIter != theEnd;
		++critIter)
		{
		// It's a criteria on the current property.
		if (ZTBSpec::eRel_StringContains == (*critIter)->GetComparator().fRel
			&& eZType_String == (*critIter)->GetTValue().TypeOf()
			&& (*critIter)->GetPropName().Equals(iPropName))
			{
			oPattern = (*critIter)->GetTValue().GetString();
			return;
			}
		}
	// Can't get here.
	ZUnimplemented();
	}

static bool sCheckForConstraint(
	const ZTName& iPropName, const ZTBSpec::CriterionSect& iCriterionSect)
	{
	for (ZTBSpec::CriterionSect::const_iterator critIter = iCriterionSect.begin(),
		theEnd = iCriterionSect.end();
		critIter != theEnd; ++critIter)
		{
		// It's a criteria on the current property.
		if (ZTBSpec::eRel_StringContains == critIter->GetComparator().fRel
			&& eZType_String == critIter->GetTValue().TypeOf()
			&& critIter->GetPropName().Equals(iPropName))
			{
			const string& thePattern = critIter->GetTValue().GetString();
			string::const_iterator iter = thePattern.begin();
			string::const_iterator theEnd = thePattern.end();
			if (ZUnicode::sInc(iter, theEnd)
				&& ZUnicode::sInc(iter, theEnd)
				&& ZUnicode::sInc(iter, theEnd))
				{
				// It's a string-containment search on our property name,
				// of a string that's three code points or more, so we can handle it.
				return true;
				}
			}
		}
	// We didn't find a string containment search on our property name.
	return false;
	}

extern bool sDumpQuery;

void ZTupleIndex_FullText::Find(const set<uint64>& iSkipIDs,
		vector<const ZTBSpec::Criterion*>& ioCriteria, vector<uint64>& oIDs)
	{
	string thePattern;
	sGetPattern(fPropName, ioCriteria, thePattern);

	set<CPChunk> theCPChunk_Pattern;
	sGetCPChunks(thePattern, theCPChunk_Pattern);

	set<uint64> results;
	bool isFirst = true;
	for (set<CPChunk>::const_iterator iterTCP = theCPChunk_Pattern.begin();
		iterTCP != theCPChunk_Pattern.end(); ++iterTCP)
		{
		map<CPChunk, set<uint64> >::const_iterator iterMap = fMap.find(*iterTCP);
		if (fMap.end() == iterMap)
			{
			results.clear();
			}
		else
			{
			if (iSkipIDs.empty())
				{
				if (isFirst)
					{
					isFirst = false;
					results = iterMap->second;
					}
				else
					{
					set<uint64> intersection;
					set_intersection(results.begin(), results.end(),
						iterMap->second.begin(), iterMap->second.end(),
						inserter(intersection, intersection.begin()));
					results.swap(intersection);
					}
				}
			else
				{
				set<uint64> candidate;
				for (set<uint64>::const_iterator i = iterMap->second.begin(),
					theEnd = iterMap->second.end();
					i != theEnd; ++i)
					{
					if (iSkipIDs.end() == iSkipIDs.find(*i))
						candidate.insert(*i);
					}

				if (isFirst)
					{
					isFirst = false;
					results = candidate;
					}
				else
					{
					set<uint64> intersection;
					set_intersection(results.begin(), results.end(),
						candidate.begin(), candidate.end(),
						inserter(intersection, intersection.begin()));
					results.swap(intersection);
					}
				}
			}
		if (results.empty())
			break;
		}

	if (const ZLog::S& s = ZLog::S(ZLog::eInfo, "ZTupleIndex_FullText"))
		{
		s.Writef("%d results of %d in map, %d chunks in pattern",
			results.size(), fMap.size(), theCPChunk_Pattern.size());
		}

	copy(results.begin(), results.end(), back_inserter(oIDs));
	sDumpQuery = true;
	}

size_t ZTupleIndex_FullText::CanHandle(const ZTBSpec::CriterionSect& iCriterionSect)
	{
	if (!sCheckForConstraint(fPropName, iCriterionSect))
		return 0;

	return (fCountTuples / 10) + 1;
	}

void ZTupleIndex_FullText::WriteDescription(const ZStrimW& s)
	{
	s << "ZTupleIndex_FullText. ";
	s.Writef("%d entries, ", fMap.size());
	s << "indexing on " << fPropName.AsString();
	}

void ZTupleIndex_FullText::sGetCPChunks(const string& iPattern, set<CPChunk>& oCPChunks)
	{
	UTF32 buffer[3];
	string::const_iterator iter = iPattern.begin();
	string::const_iterator iterEnd = iPattern.end();

	if (!ZUnicode::sReadInc(iter, iterEnd, buffer[0]))
		return;

	if (!ZUnicode::sReadInc(iter, iterEnd, buffer[1]))
		return;

	for (;;)
		{
		if (!ZUnicode::sReadInc(iter, iterEnd, buffer[2]))
			break;

		CPChunk theCPChunk(buffer);
		oCPChunks.insert(theCPChunk);
		buffer[0] = buffer[1];
		buffer[1] = buffer[2];
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_FullText

ZTupleIndexFactory_FullText::ZTupleIndexFactory_FullText(const std::string& iPropName)
:	fPropName(iPropName)
	{}

ZTupleIndex* ZTupleIndexFactory_FullText::Make()
	{ return new ZTupleIndex_FullText(fPropName); }

#endif // ZCONFIG_SPI_Enabled(ICU)
