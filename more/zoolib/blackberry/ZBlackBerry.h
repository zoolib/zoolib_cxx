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

#include "zoolib/ZCallback_T.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

#include <set>
#include <vector>

namespace ZooLib {

namespace ZBlackBerry {

typedef ZData_Any Data;

struct PasswordHash
	{
	uint8 fData[20];
	};

class Device;
class Channel;

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::StartStop

class StartStop : public ZCounted
	{
protected:
	StartStop();

public:
// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	virtual void Started();
	virtual void Stopped();

private:
	ZMtxR fMutex;
	ZCnd fCondition;
	int fStartCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Manager

class Manager : public ZCounted
	{
protected:
	Manager();

public:
	virtual ~Manager();

// Our protocol
	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs) = 0;
	virtual ZRef<Device> Open(uint64 iDeviceID) = 0;

	typedef ZCallback_T<ZRef<Manager> > CB_ManagerChanged;
	void RegisterManagerChanged(ZRef<CB_ManagerChanged> iCallback);
	void UnregisterManagerChanged(ZRef<CB_ManagerChanged> iCallback);

protected:
	void pChanged();

private:
	ZCallbackSet_T<ZRef<Manager> > fCallbacks;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZBlackBerry::Device

class Device : public ZCounted
	{
protected:
	Device();

public:
	virtual ~Device();

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

	virtual Data GetAttribute(uint16 iObject, uint16 iAttribute) = 0;
	virtual uint32 GetPIN();

	typedef ZCallback_T<ZRef<Device> > CB_DeviceFinished;
	void RegisterDeviceFinished(ZRef<CB_DeviceFinished> iCallback);
	void UnregisterDeviceFinished(ZRef<CB_DeviceFinished> iCallback);

protected:
	void pFinished();

private:
	ZCallbackSet_T<ZRef<Device> > fCallbacks;
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

} // namespace ZooLib

#endif // __ZBlackBerry__
