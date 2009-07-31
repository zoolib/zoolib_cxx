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

#ifndef __ZBlackBerry__
#define __ZBlackBerry__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZThreadOld.h"
#include "zoolib/ZVal_Any.h"

#include <set>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

namespace ZBlackBerry {

typedef ZValData_Any ValData;

struct PasswordHash
	{
	uint8 fData[20];
	};

class Device;
class Channel;

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager

class Manager : public ZRefCountedWithFinalize
	{
protected:
	Manager();

public:
	virtual ~Manager();

// From ZRefCountedWithFinalize
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	virtual void Start();
	virtual void Stop();

	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs) = 0;
	virtual ZRef<Device> Open(uint64 iDeviceID) = 0;

	class Observer
		{
	public:
		virtual void ManagerChanged(ZRef<Manager> iManager) = 0;
		};

	void ObserverAdd(Observer* iObserver);
	void ObserverRemove(Observer* iObserver);

protected:
	void pNotifyObservers();

	void pStarted();
	void pStopped();

private:
	ZMutex fMutex;
	ZCondition fCondition;
	int fStartCount;
	std::set<Observer*> fObservers;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device

class Device : public ZRefCountedWithFinalize
	{
protected:
	Device();

public:
	virtual ~Device();

// From ZRefCountedWithFinalize
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	virtual void Start();
	virtual void Stop();

	enum Error
		{
		error_None,
		error_DeviceClosed,
		error_UnknownChannel,
		error_PasswordNeeded,
		error_PasswordExhausted,
		error_PasswordIncorrect,
		error_Generic
		};

	virtual ZRef<Channel> Open(
		const std::string& iName, const PasswordHash* iPasswordHash, Error* oError);

	virtual ZRef<Channel> Open(bool iPreserveBoundaries,
		const std::string& iName, const PasswordHash* iPasswordHash, Error* oError) = 0;

	virtual ValData GetAttribute(uint16 iObject, uint16 iAttribute) = 0;
	virtual uint32 GetPIN();

	class Observer
		{
	public:
		virtual void Finished(ZRef<Device> iDevice) = 0;
		};

	void ObserverAdd(Observer* iObserver);
	void ObserverRemove(Observer* iObserver);

protected:
	void pFinished();

private:
	ZMutex fMutex;
	ZCondition fCondition;
	int fStartCount;
	std::set<Observer*> fObservers;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Channel

class Channel : public ZStreamerRWCon
	{
protected:
	Channel();

public:
	virtual ~Channel();

// Our protocol
	virtual size_t GetIdealSize_Read();
	virtual size_t GetIdealSize_Write();
	};

} // namespace ZBlackBerry

NAMESPACE_ZOOLIB_END

#endif // __ZBlackBerry__
