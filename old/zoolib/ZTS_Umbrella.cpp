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

#include "zoolib/ZTS_Umbrella.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

using std::map;
using std::pair;
using std::set;
using std::vector;

#define kDebug_TS_Umbrella 0

// =================================================================================================
// MARK: - ZTS_Umbrella

ZTS_Umbrella::ZTS_Umbrella(const vector<ZRef<ZTS> >& iTSes)
	{
	for (vector<ZRef<ZTS> >::const_iterator i = iTSes.begin(), theEnd = iTSes.end();
		i != theEnd; ++i)
		{
		Child theChild;
		theChild.fTS = *i;
		fChildren.push_back(theChild);
		fReadComposite.Add((*i)->GetReadLock());
		}
	}

ZTS_Umbrella::~ZTS_Umbrella()
	{}

void ZTS_Umbrella::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount)
	{ fChildren.front().fTS->AllocateIDs(iCount, oBaseID, oCount); }

void ZTS_Umbrella::SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	while (--iCount)
		{
		uint64 theGlobalID = *iIDs++;
		GlobalToLocal_t::iterator i = fGlobalToLocal.find(theGlobalID);
		if (i != fGlobalToLocal.end())
			{
			size_t theChildIndex = (*i).second.first;
			uint64 theLocalID = (*i).second.second;
			ZTuple temp = *iTuples;
			this->pTranslate_GlobalToLocal(theChildIndex, temp);
			fChildren[theChildIndex].fTS->SetTuples(1, &theLocalID, &temp);
			}
		else
			{
			// It's in the prime TS, and thus its ID and contents are not translated.
			fChildren.front().fTS->SetTuples(1, &theGlobalID, iTuples);
			}
		++iTuples;
		}
	// Any changes
	fChildren.front().fTS->SetTuples(iCount, iIDs, iTuples);
	}

void ZTS_Umbrella::GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{
	// A real implementation would walk iIDs and determine which TS owns each ID,
	// then push the fetch off to a pre-running thread that synchronously
	// calls the appropriate TS, and possibly lock only those TSes. Alternatively
	// we would make the TS API asynchronous.
	while (--iCount)
		{
		uint64 theGlobalID = *iIDs++;
		GlobalToLocal_t::iterator i = fGlobalToLocal.find(theGlobalID);
		if (i != fGlobalToLocal.end())
			{
			size_t theChildIndex = (*i).second.first;
			uint64 theLocalID = (*i).second.second;
			fChildren[theChildIndex].fTS->GetTuples(1, &theLocalID, oTuples);
			this->pTranslate_LocalToGlobal(theChildIndex, *oTuples);
			}
		else
			{
			// It's in the prime TS, and thus its ID and contents are not translated.
			fChildren.front().fTS->GetTuples(1, &theGlobalID, oTuples);
			}
		++oTuples;
		}
	}

void ZTS_Umbrella::Search(const ZTBSpec& iSpec, const set<uint64>& iSkipIDs, set<uint64>& ioIDs)
	{
	vector<Child>::iterator i = fChildren.begin();
	(*i).fTS->Search(iSpec, iSkipIDs, ioIDs);

	vector<set<uint64> > skipIDs(fChildren.size());

	for (set<uint64>::const_iterator i = iSkipIDs.begin(), theEnd = iSkipIDs.end();
		i != theEnd; ++i)
		{
		GlobalToLocal_t::iterator j = fGlobalToLocal.find(*i);
		if (j != fGlobalToLocal.end())
			{
			size_t theChildIndex = (*j).second.first;
			ZAssertStop(kDebug_TS_Umbrella, theChildIndex < fChildren.size());
			skipIDs[theChildIndex].insert((*j).second.second);
			}
		}

	for (size_t theChildIndex = 1; theChildIndex < fChildren.size(); ++theChildIndex)
		{
		set<uint64> results;
		fChildren[theChildIndex].fTS->Search(iSpec, skipIDs[theChildIndex], results);

		for (set<uint64>::iterator i = results.begin(), theEnd = results.end();
			i != theEnd; ++i)
			{
			uint64 globalID = this->pLocalToGlobal(theChildIndex, *i);
			ioIDs.insert(globalID);
			}
		}
	}

ZMutexBase& ZTS_Umbrella::GetReadLock()
	{ return fReadComposite; }

ZMutexBase& ZTS_Umbrella::GetWriteLock()
	{ return fChildren.front().fTS->GetWriteLock(); }

uint64 ZTS_Umbrella::pAllocateID()
	{ return this->pAllocateID(fChildren.front().fTS); }

uint64 ZTS_Umbrella::pAllocateID(ZRef<ZTS> iTS)
	{
	for (;;)
		{
		uint64 theID;
		size_t countIssued;
		iTS->AllocateIDs(1, theID, countIssued);
		if (countIssued)
			return theID;
		}
	}

void ZTS_Umbrella::pTranslate_GlobalToLocal(size_t iChildIndex, ZMap_ZooLib& ioTuple)
	{
	for (ZTuple::const_iterator i = ioTuple.begin(), theEnd = ioTuple.end();
		i != theEnd; ++i)
		{
		switch (ioTuple.RGet(i).TypeOf())
			{
			case eZType_ID:
				{
				uint64 localID = this->pGlobalToLocal(iChildIndex, ioTuple.Get(i).GetID());
				ioTuple.Set(i, localID);
				break;
				}
			case eZType_Tuple:
				{
				this->pTranslate_GlobalToLocal(iChildIndex, ioTuple.Mutable(i).MutableMap());
				break;
				}
			case eZType_Vector:
				{
				vector<ZTValue>& theVector = ioTuple.Mutable(i).MutableSeq().MutableVector();
				for (vector<ZTValue>::iterator j = theVector.begin(); j != theVector.end(); ++j)
					{
					if (eZType_ID == (*j).TypeOf())
						{
						uint64 localID = this->pGlobalToLocal(iChildIndex, ioTuple.Get(i).GetID());
						(*j).SetID(localID);
						}
					}
				}
			}
		}
	}

void ZTS_Umbrella::pTranslate_LocalToGlobal(size_t iChildIndex, ZMap_ZooLib& ioTuple)
	{
	for (ZTuple::const_iterator i = ioTuple.begin(), theEnd = ioTuple.end();
		i != theEnd; ++i)
		{
		switch (ioTuple.RGet(i).TypeOf())
			{
			case eZType_ID:
				{
				uint64 globalID = this->pLocalToGlobal(iChildIndex, ioTuple.Get(i).GetID());
				ioTuple.Set(i, globalID);
				break;
				}
			case eZType_Tuple:
				{
				this->pTranslate_LocalToGlobal(iChildIndex, ioTuple.Mutable(i).MutableMap());
				break;
				}
			case eZType_Vector:
				{
				vector<ZTValue>& theVector = ioTuple.Mutable(i).MutableSeq().MutableVector();
				for (vector<ZTValue>::iterator j = theVector.begin(); j != theVector.end(); ++j)
					{
					if (eZType_ID == (*j).TypeOf())
						{
						uint64 globalID = this->pLocalToGlobal(iChildIndex, ioTuple.Get(i).GetID());
						(*j).SetID(globalID);
						}
					}
				}
			}
		}
	}

uint64 ZTS_Umbrella::pGlobalToLocal(size_t iChildIndex, uint64 iGlobalID)
	{
	Child& theChild = fChildren[iChildIndex];
	GlobalToLocal_t::iterator i = fGlobalToLocal.find(iGlobalID);
	if (i != fGlobalToLocal.end())
		{
		ZAssertStop(kDebug_TS_Umbrella, (*i).second.first == iChildIndex);
		return (*i).second.second;
		}
	else
		{
		uint64 localID = this->pAllocateID(theChild.fTS);
		theChild.fLocalToGlobal.insert(LocalToGlobal_t::value_type(localID, iGlobalID));
		fGlobalToLocal.insert(GlobalToLocal_t::value_type(iGlobalID, pair<size_t, uint64>(iChildIndex, localID)));
		return localID;
		}
	}

uint64 ZTS_Umbrella::pLocalToGlobal(size_t iChildIndex, uint64 iLocalID)
	{
	Child& theChild = fChildren[iChildIndex];
	LocalToGlobal_t::iterator i = theChild.fLocalToGlobal.find(iLocalID);
	if (i != theChild.fLocalToGlobal.end())
		{
		return (*i).second;
		}
	else
		{
		// No mapping found, create one.
		uint64 globalID = this->pAllocateID();
		theChild.fLocalToGlobal.insert(LocalToGlobal_t::value_type(iLocalID, globalID));
		fGlobalToLocal.insert(GlobalToLocal_t::value_type(globalID, pair<size_t, uint64>(iChildIndex, iLocalID)));
		return globalID;
		}
	}

} // namespace ZooLib

