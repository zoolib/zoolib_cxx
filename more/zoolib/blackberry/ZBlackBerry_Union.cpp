/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/blackberry/ZBlackBerry_Union.h"

#include "zoolib/ZUtil_STL_vector.h"

using std::pair;
using std::vector;

namespace ZooLib {
namespace ZBlackBerry {

// =================================================================================================
// MARK: - ZBlackBerry::Manager_Union

Manager_Union::Manager_Union(const vector<ZRef<Manager> >& iManagers)
	{
	for (vector<ZRef<Manager> >::const_iterator i = iManagers.begin(); i != iManagers.end(); ++i)
		{
		ZRef<Manager> theManager = *i;
		Entry_t anEntry;
		anEntry.fManager = theManager;
		anEntry.fValid = false;
		fEntries.push_back(anEntry);
		}
	}

void Manager_Union::Initialize()
	{
	Manager::Initialize();
	for (vector<Entry_t>::const_iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		(*i).fManager->SetCallable(this);
	}

void Manager_Union::Finalize()
	{
	for (vector<Entry_t>::const_iterator i = fEntries.begin(); i != fEntries.end(); ++i)
		(*i).fManager->SetCallable(null);
	Manager::Finalize();
	}

Manager_Union::~Manager_Union()
	{}

ZQ<void> Manager_Union::QCall(ZRef<ZBlackBerry::Manager> iManager)
	{
	ZGuardRMtxR locker(fMutex);
	for (vector<Entry_t>::iterator iterEntries = fEntries.begin();
		iterEntries != fEntries.end(); ++iterEntries)
		{
		if (iterEntries->fManager == iManager)
			iterEntries->fValid = false;
		}

	locker.Release();

	Manager::pChanged();

	return notnull;
	}

void Manager_Union::GetDeviceIDs(vector<uint64>& oDeviceIDs)
	{
	ZGuardRMtxR locker(fMutex);
	this->pValidate();

	for (vector<Entry_t>::iterator iterEntries = fEntries.begin();
		iterEntries != fEntries.end(); ++iterEntries)
		{
		for (vector<pair<uint64, uint64> >::iterator i = iterEntries->fIDs.begin();
			i != iterEntries->fIDs.end(); ++i)
			{
			oDeviceIDs.push_back(i->first);
			}
		}
	}

void Manager_Union::pValidate()
	{
	for (vector<Entry_t>::iterator iterEntries = fEntries.begin();
		iterEntries != fEntries.end(); ++iterEntries)
		{
		Entry_t& curEntry = *iterEntries;
		if (!curEntry.fValid)
			{
			curEntry.fValid = true;
			vector<uint64> deviceIDs;
			curEntry.fManager->GetDeviceIDs(deviceIDs);

			for (vector<uint64>::iterator i = deviceIDs.begin();
				i != deviceIDs.end(); ++i)
				{
				bool gotIt = false;
				const uint64 deviceID = *i;
				for (vector<pair<uint64, uint64> >::iterator j = curEntry.fIDs.begin();
					!gotIt && j != curEntry.fIDs.end(); ++j)
					{
					if (deviceID == j->second)
						gotIt = true;
					}
				if (!gotIt)
					curEntry.fIDs.push_back(pair<uint64, uint64>(fNextID++, deviceID));
				}

			for (vector<pair<uint64, uint64> >::iterator j = curEntry.fIDs.begin();
				j != curEntry.fIDs.end(); /*no inc*/)
				{
				if (ZUtil_STL::sContains(deviceIDs, j->second))
					++j;
				else
					j = curEntry.fIDs.erase(j);
				}
			}
		}
	}

ZRef<Device> Manager_Union::Open(uint64 iDeviceID)
	{
	ZGuardRMtxR locker(fMutex);
	for (vector<Entry_t>::iterator iterEntries = fEntries.begin();
		iterEntries != fEntries.end(); ++iterEntries)
		{
		Entry_t& curEntry = *iterEntries;
		for (vector<pair<uint64, uint64> >::iterator j = curEntry.fIDs.begin();
			j != curEntry.fIDs.end(); /*no inc*/)
			{
			if (j->first == iDeviceID)
				{
				ZRef<Manager> foundManager = iterEntries->fManager;
				uint64 foundDeviceID = j->second;
				locker.Release();
				return foundManager->Open(foundDeviceID);
				}
			}
		}
	return null;
	}

} // namespace ZBlackBerry
} // namespace ZooLib
