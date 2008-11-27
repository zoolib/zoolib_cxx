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

#include "zoolib/ZBlackBerry.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL.h"

using namespace ZooLib;

using std::set;
using std::string;

/**
These are some brief notes to get you started. If anything is unclear, drop me a line at ag@em.net.

The BlackBerry code is all in the namespace ZBlackBerry.

Most entities in this API suite are ultimately derived from ZRefCountedWithFinalization, and thus
we use refcounted pointers of the form ZRef<XXX> rather than using unadorned XXX*.

There are three entities with which you'll be working: Manager, Device and Channel.

An instance of Manager provides access to a roster of BlackBerrys. In the common case that will
be one BlackBerry connected by USB. Registering with the Manager as an Observer will cause your
ManagerChanged method to be invoked whenever the roster has or may have changed.

Device represents a single connected BlackBerry. Register with the Device as an Observer to
be told when it has been disconnected. Device's other job is to open channels to software
running on the BlackBerry.

Channel represents the connection to a channel on a BlackBerry. As happened with ZNetEndpoint,
we may need additional entry points and controllable attributes, so Channel is a distinct class,
but currently it adds nothing to its parent class ZStreamerRWCon. For detailed information on
ZStreamerRWCon see ZStreamer.cpp.

You can see an example of real code in ZBlackBerryServer -- it's complicated by the
details of safely handling multiple connections simultaneously. The sample application
zoolib_samples/BlackBerry/BBDaemon provides access to a ZBlackBerryServer over a TCP
connection, and thus allows multiple apps to talk to multiple BlackBerrys even on OSX.

The essentials of working with ZBlackBerry are demonstrated in this code:

void Sample()
	{
	using namespace ZBlackBerry;

	// Instantiate the Manager. On Windows you would create a Manager_BBDevMgr.
	// If you're using BBDaemon you'd create a Manager_Client, passing a
	// ZStreamerRWConFactory that will open connections to the
	// appropriate TCP address.
	ZRef<Manager> theManager = new Manager_OSXUSB;

	// Get the IDs of all BlackBerrys handled by this Manager.
	// The ID of a Device is non-zero, opaque and unique within a Manager.
	vector<uint64> theDeviceIDs;
	theManager->GetDeviceIDs(theDeviceIDs);

	if (theDeviceIDs.size())
		{
		// We found at least one. Open the first.
		const uint64 theDeviceID = theDeviceIDs[0];
		if (ZRef<Device> theDevice = theManager->Open(theDeviceID))
			{
			// The device opened okay. Just because you have the ID for a device
			// that is no guarantee that the device is still there -- it can
			// get unplugged at any time. When a device is unplugged its ID
			// is retired and will never again be used by the Manager that allocated it.

			// Now open a Channel to "MyAppChannel". We're passing nil for the
			// password hash parameter, indicating that we do not know the device's
			// password. We're passing nil for the oError output parameter, indicating
			// that we don't care to know the precise reason for a failure.
			if (ZRef<Channel> theChannel = theDevice->Open("MyAppChannel", nil, nil))
				{
				// We can now send and receive data over theChannel. As with any other
				// streamer we can take a local reference to the stream(s) it encapsulates.				
				const ZStreamWCon& w = theChannel->GetStreamWCon();
				const ZStreamRCon& r = theChannel->GetStreamRCon();

				// Send a string
				w.WriteString("Hello MyAppChannel");
				// And a terminating zero byte.
				w.WriteUInt8(0);
				
				// Just in case there's anything funky happening (buffers, filters etc)
				// it is advisable to flush a write stream before reading from an
				// associated read stream.
				w.Flush();

				// Read the response, whatever it might be.
				bool result = r.ReadBool();

				// Close our send direction.
				w.SendDisconnect();

				// Suck up and discard any outstanding data until the receive direction closes.
				r.ReceiveDisconnect(-1);
				}
			}
		}
	// And by the magic of refcounting, Channels, Devices and Managers will cleanly
	// disconnect and dispose when they are no longer in use.
	}
*/

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager

Manager::Manager()
:	fMutex("ZBlackBerry::Manager::fMutex"),
	fStartCount(0)
	{}

Manager::~Manager()
	{
	ZAssert(fObservers.empty());
	ZAssert(fStartCount == 0);
	}

void Manager::Initialize()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
		s << "Initialize, enter";

	ZMutexLocker locker(fMutex);
	if (this->GetRefCount() == 1)
		{
		if (fStartCount == 0)
			{
			locker.Release();
			if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
				s << "Initialize, calling Start";
				
			this->Start();
			}
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
		s << "Initialize, exit";
	}

void Manager::Finalize()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
		s << "Finalize, enter";

	ZMutexLocker locker(fMutex);

	ZAssert(fObservers.empty());

	if (this->GetRefCount() != 1)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
			s << "Finalize, busy";
		this->FinalizationComplete();
		return;
		}

	if (fStartCount)
		{
		locker.Release();

		this->Stop();

		locker.Acquire();

		while (fStartCount)
			fCondition.Wait(fMutex);
		}

	if (this->GetRefCount() != 1)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
			s << "Finalize, post stop, busy";
		this->FinalizationComplete();
		return;
		}

	this->FinalizationComplete();
	locker.Release();
	delete this;

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Manager"))
		s << "Finalize, deleted";
	}

void Manager::Start()
	{
	this->pStarted();
	}

void Manager::Stop()
	{
	this->pStopped();
	}

void Manager::ObserverAdd(Observer* iObserver)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sInsertMustNotContain(1, fObservers, iObserver);
	}

void Manager::ObserverRemove(Observer* iObserver)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fObservers, iObserver);
	}

void Manager::pNotifyObservers()
	{
	ZMutexLocker locker(fMutex);
	if (!fObservers.empty())
		{
		ZRef<Manager> localThis = this;

		set<Observer*> localObservers = fObservers;
		for (set<Observer*>::iterator i = localObservers.begin(); i != localObservers.end(); ++i)
			(*i)->ManagerChanged(localThis);

		locker.Release();
		}
	}

void Manager::pStarted()
	{
	ZMutexLocker locker(fMutex);
	++fStartCount;
	fCondition.Broadcast();
	}

void Manager::pStopped()
	{
	ZMutexLocker locker(fMutex);
	--fStartCount;
	fCondition.Broadcast();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device

Device::Device()
:	fMutex("ZBlackBerry::Device::fMutex"),
	fStartCount(0)
	{}

Device::~Device()
	{
	ZAssert(fObservers.empty());
	ZAssert(fStartCount == 0);
	}

void Device::Initialize()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
		s << "Initialize, enter";

	ZMutexLocker locker(fMutex);
	if (this->GetRefCount() == 1)
		{
		if (fStartCount == 0)
			{
			locker.Release();
			if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
				s << "Initialize, calling Start";
			this->Start();
			}
		}

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
		s << "Initialize, exit";
	}

void Device::Finalize()
	{
	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
		s << "Finalize, enter";

	ZMutexLocker locker(fMutex);

	ZAssert(fObservers.empty());

	if (this->GetRefCount() != 1)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
			s << "Finalize, busy";
		this->FinalizationComplete();
		return;
		}

	if (fStartCount)
		{
		locker.Release();

		this->Stop();

		locker.Acquire();

		while (fStartCount)
			fCondition.Wait(fMutex);
		}

	if (this->GetRefCount() != 1)
		{
		if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
			s << "Finalize, post stop, busy";
		this->FinalizationComplete();
		return;
		}

	this->FinalizationComplete();
	locker.Release();
	delete this;

	if (ZLOG(s, eDebug + 2, "ZBlackBerry::Device"))
		s << "Finalize, deleted";
	}

void Device::Start()
	{
	ZMutexLocker locker(fMutex);
	++fStartCount;
	fCondition.Broadcast();
	}

void Device::Stop()
	{
	this->pFinished();
	}

ZRef<Channel> Device::Open(
	const string& iName, const PasswordHash* iPasswordHash, Error* oError)
	{ return this->Open(false, iName, iPasswordHash, oError); }

uint32 Device::GetPIN()
	{
	return 0;
	}

void Device::ObserverAdd(Observer* iObserver)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sInsertMustNotContain(1, fObservers, iObserver);
	if (fStartCount == 0)
		iObserver->Finished(this);
	}

void Device::ObserverRemove(Observer* iObserver)
	{
	ZMutexLocker locker(fMutex);
	ZUtil_STL::sEraseMustContain(1, fObservers, iObserver);
	}

void Device::pFinished()
	{
	ZMutexLocker locker(fMutex);

	--fStartCount;
	fCondition.Broadcast();

	if (!fObservers.empty())
		{
		ZRef<Device> localThis = this;

		set<Observer*> localObservers = fObservers;
		for (set<Observer*>::iterator i = localObservers.begin(); i != localObservers.end(); ++i)
			(*i)->Finished(localThis);

		// Observers may be the only thing with a reference to us, so release
		// the locker now in case we get finalized when localThis goes out of scope.
		locker.Release();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Channel

Channel::Channel()
	{}

Channel::~Channel()
	{}

size_t Channel::GetIdealSize_Read()
	{
	return 4096;
	}

size_t Channel::GetIdealSize_Write()
	{
	return 4096;
	}

} // namespace ZBlackBerry
