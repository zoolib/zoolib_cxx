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

#include "zoolib/blackberry/ZBlackBerry.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL.h"

using std::set;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

namespace ZBlackBerry {

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager

/**
\class Manager
\ingroup BlackBerry
*/

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

/**
\class Device
\ingroup BlackBerry
*/

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

/**
\class Channel
\ingroup BlackBerry
*/

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

NAMESPACE_ZOOLIB_END
