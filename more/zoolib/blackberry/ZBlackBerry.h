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

#ifndef __ZBlackBerry_h__
#define __ZBlackBerry_h__ 1
#include "zconfig.h"

#include "zoolib/Safe.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

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
// MARK: - ZBlackBerry::Manager

class Manager : public ZCounted
	{
protected:
	Manager();

public:
	virtual ~Manager();

// Our protocol
	virtual void GetDeviceIDs(std::vector<uint64>& oDeviceIDs) = 0;
	virtual ZRef<Device> Open(uint64 iDeviceID) = 0;

	typedef ZRef<Manager> ZRef_Manager;
	typedef ZCallable<void(ZRef_Manager)> CB_ManagerChanged;
	void SetCallable(ZRef<CB_ManagerChanged> iCallable);

protected:
	void pChanged();

private:
	Safe<ZRef<CB_ManagerChanged> > fCallable;
	};

// =================================================================================================
// MARK: - ZBlackBerry::Device

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

	virtual ZQ<Data> GetAttribute(uint16 iObject, uint16 iAttribute) = 0;
	virtual uint32 GetPIN();

	typedef ZRef<Device> ZRef_Device;
	typedef ZCallable<void(ZRef_Device)> CB_DeviceFinished;
	void SetCallable(ZRef<CB_DeviceFinished> iCallback);

protected:
	void pFinished();

private:
	Safe<ZRef<CB_DeviceFinished> > fCallable;
	};

// =================================================================================================
// MARK: - ZBlackBerry::Channel

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

#endif // __ZBlackBerry_h__
