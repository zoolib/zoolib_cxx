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

#ifndef __ZooLib_Net_h__
#define __ZooLib_Net_h__ 1
#include "zconfig.h"

#include "zoolib/Connection.h" // For Factory_ChannerRWClose_Bin

#include <string>
#include <stdexcept> // For runtime_error

namespace ZooLib {

class NetAddressLookup;
class NetName;
class NetNameLookup;

// =================================================================================================
#pragma mark -
#pragma mark Net

namespace Net {

enum Error
	{
	errorNone,
	errorGeneric,
	errorBadState,

	errorCouldntConnect,
	errorLocalPortInUse,
//	errorCantGetAmountUnread,
//	errorBufferTooSmall
	};

} // namespace Net

// =================================================================================================
#pragma mark -
#pragma mark NetEx

class NetEx
:	public std::runtime_error
	{
public:
	NetEx(Net::Error iError);
	Net::Error GetNetError() const
		{ return fError; }

private:
	Net::Error fError;
	};

// =================================================================================================
#pragma mark -
#pragma mark NetAddress

/// Represents the physical address of a particular endpoint on a particular host.
class NetAddress
:	public Factory_ChannerRWClose_Bin
	{
protected:
	NetAddress();

public:
	virtual ~NetAddress();

	virtual ZRef<NetAddressLookup> MakeLookup(size_t iMaxNames);
	};

// =================================================================================================
#pragma mark -
#pragma mark NetAddressLookup

/// Subclasses of this are returned by NetAddress instances when
/// NetAddress::MakeLookup is called. It's an iterator that returns zero or
/// more names that are considered to map to the address in question.
class NetAddressLookup
:	public ZCounted
	{
protected:
	NetAddressLookup();

public:
	virtual ~NetAddressLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZRef<NetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark NetName

/// Represents the abstract name of a port or service on a host or hosts.
class NetName
:	public Factory_ChannerRWClose_Bin
	{
protected:
	NetName();

public:
	virtual ~NetName();

// From Factory_ChannerRWClose_Bin
	virtual ZRef<ChannerRWClose_Bin> Make();

// Our protocol
	virtual std::string AsString() = 0;

	virtual ZRef<NetNameLookup> MakeLookup(size_t iMaxAddresses) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark NetNameLookup

/// Subclasses of this are returned by NetName instances when
/// NetName::MakeLookup is called. It's an iterator that returns zero or
/// more addresses that the name maps to.
class NetNameLookup
:	public ZCounted
	{
protected:
	NetNameLookup();

public:
	virtual ~NetNameLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZRef<NetAddress> CurrentAddress() = 0;
	virtual ZRef<NetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark NetListener

/// Subclasses of this return NetEndpoint instances as connections arrive.
class NetListener
:	public Factory_ChannerRWClose_Bin
	{
protected:
	NetListener();

public:
	virtual ~NetListener();

	virtual ZRef<NetAddress> GetAddress();
	};

} // namespace ZooLib

#endif // __ZooLib_Net_h__
