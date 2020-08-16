// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Net_h__
#define __ZooLib_Net_h__ 1
#include "zconfig.h"

#include "zoolib/Cancellable.h"
#include "zoolib/Connection.h" // For Factory_ChannerRWClose_Bin

#include <string>
#include <stdexcept> // For runtime_error

namespace ZooLib {

class NetAddressLookup;
class NetName;
class NetNameLookup;

// =================================================================================================
#pragma mark - Net

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
#pragma mark - NetEx

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
#pragma mark - NetAddress

/// Represents the physical address of a particular endpoint on a particular host.
class NetAddress
:	public virtual Factory_ChannerRWClose_Bin
	{
protected:
	NetAddress();

public:
	virtual ~NetAddress();

// From Factory_ChannerRWClose_Bin
	virtual ZP<ChannerRWClose_Bin> QCall();

// Our protocol
	virtual ZP<ChannerRWClose_Bin> Connect() = 0;

	virtual ZP<NetAddressLookup> MakeLookup(size_t iMaxNames);
	};

// =================================================================================================
#pragma mark - NetAddressLookup

/// Subclasses of this are returned by NetAddress instances when
/// NetAddress::MakeLookup is called. It's an iterator that returns zero or
/// more names that are considered to map to the address in question.
class NetAddressLookup
:	public Counted
	{
protected:
	NetAddressLookup();

public:
	virtual ~NetAddressLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZP<NetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark - NetName

/// Represents the abstract name of a port or service on a host or hosts.
class NetName
:	public virtual Factory_ChannerRWClose_Bin
	{
protected:
	NetName();

public:
	virtual ~NetName();

// From Factory_ChannerRWClose_Bin
	virtual ZP<ChannerRWClose_Bin> QCall();

// Our protocol
	virtual ZP<ChannerRWClose_Bin> Connect();

	virtual std::string AsString() = 0;

	virtual ZP<NetNameLookup> MakeLookup(size_t iMaxAddresses) = 0;
	};

// =================================================================================================
#pragma mark - NetNameLookup

/// Subclasses of this are returned by NetName instances when
/// NetName::MakeLookup is called. It's an iterator that returns zero or
/// more addresses that the name maps to.
class NetNameLookup
:	public Counted
	{
protected:
	NetNameLookup();

public:
	virtual ~NetNameLookup();

	virtual void Start() = 0;
	virtual bool Finished() = 0;
	virtual void Advance() = 0;

	virtual ZP<NetAddress> CurrentAddress() = 0;
	virtual ZP<NetName> CurrentName() = 0;
	};

// =================================================================================================
#pragma mark - NetListener

/// Subclasses of this return NetEndpoint instances as connections arrive.
class NetListener
:	public virtual Factory_ChannerRWClose_Bin
,	public virtual Cancellable
	{
protected:
	NetListener();

public:
	virtual ~NetListener();

// From Factory_ChannerRWClose_Bin
	virtual ZP<ChannerRWClose_Bin> QCall();

// Our protocol
	virtual ZP<ChannerRWClose_Bin> Listen() = 0;

	virtual ZP<NetAddress> GetAddress();
	};

} // namespace ZooLib

#endif // __ZooLib_Net_h__
