/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZNet__
#define __ZNet__ 1
#include "zconfig.h"

#include "ZCompat_NonCopyable.h"
#include "ZStreamer.h"

#include <string>
#include <stdexcept> // For runtime_error

class ZNetAddressLookup;
class ZNetEndpoint;
class ZNetName;
class ZNetNameLookup;

// =================================================================================================
#pragma mark -
#pragma mark * ZNet

namespace ZNet
	{
	enum Error
		{
		errorNone,
		errorGeneric,
		errorBadState,
	
		errorCouldntConnect,
		errorLocalPortInUse,
		errorCantGetAmountUnread,
		errorBufferTooSmall
		};
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEx

class ZNetEx : public std::runtime_error
	{
public:
	ZNetEx(ZNet::Error iError);
	ZNet::Error GetNetError() const
		{ return fError; }

private:
	ZNet::Error fError;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress

/// Represents the physical address of a particular endpoint on a particular host.
class ZNetAddress : public ZStreamerRWConFactory
	{
public:
	virtual ~ZNetAddress();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();

// Our protocol
	virtual ZRef<ZNetAddressLookup> CreateLookup(size_t iMaxNames) const;

	virtual ZRef<ZNetEndpoint> Connect() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddressLookup

/// Subclasses of this are returned by ZNetAddress instances when
/// ZNetAddress::CreateLookup is called. It's an iterator that returns zero or
/// more names that are considered to map to the address in question.
class ZNetAddressLookup : public ZRefCountedWithFinalization
	{
public:
	virtual ~ZNetAddressLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZRef<ZNetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName

/// Represents the abstract name of a port or service on a host or hosts.
class ZNetName : public ZStreamerRWConFactory
	{
public:
	virtual ~ZNetName();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();

// Our protocol
	virtual std::string AsString() const = 0;

	virtual ZRef<ZNetNameLookup> CreateLookup(size_t iMaxAddresses) const;

	ZRef<ZNetEndpoint> Connect() const;

	ZRef<ZNetAddress> Lookup() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup

/// Subclasses of this are returned by ZNetName instances when
/// ZNetName::CreateLookup is called. It's an iterator that returns zero or
/// more addresses that the name maps to.
class ZNetNameLookup : public ZRefCountedWithFinalization
	{
public:
	virtual ~ZNetNameLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZRef<ZNetAddress> CurrentAddress() = 0;
	virtual ZRef<ZNetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener

/// Subclasses of this return ZNetEndpoint instances as connections arrive.
class ZNetListener : public ZStreamerRWConFactory, ZooLib::NonCopyable
	{
public:
	virtual ~ZNetListener();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();
	virtual void Cancel();

// Our protocol
	virtual ZRef<ZNetEndpoint> Listen() = 0;
	virtual void CancelListen() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint

/// Subclasses of this provide the interface to network endpoints.
class ZNetEndpoint : public ZStreamerRWCon, ZooLib::NonCopyable
	{
protected:
	ZNetEndpoint();

public:
	virtual ~ZNetEndpoint();

// Our protocol
	virtual ZRef<ZNetAddress> GetLocalAddress();
	virtual ZRef<ZNetAddress> GetRemoteAddress() = 0;

// Compatibility protocol
	bool WaitTillReadable(int iMilliseconds);
	bool ReceiveDisconnect(int iMilliseconds);
	void SendDisconnect();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpointDG

class ZNetEndpointDG : public ZRefCountedWithFinalization
	{
protected:
	ZNetEndpointDG();

public:
	virtual ~ZNetEndpointDG();

	virtual ZNet::Error Receive(void* iBuffer, size_t iBufferSize, size_t& oCountReceived, ZRef<ZNetAddress>& oSourceAddress) = 0;
	virtual ZNet::Error Send(const void* iBuffer, size_t iCount, ZRef<ZNetAddress> iDestAddress) = 0;
	};

#endif // __ZNet__
