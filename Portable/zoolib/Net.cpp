// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Net.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - NetEx

NetEx::NetEx(Net::Error iError)
:	runtime_error("NetEx")
,	fError(iError)
	{}

// =================================================================================================
#pragma mark - NetAddress

NetAddress::NetAddress()
	{}

NetAddress::~NetAddress()
	{}

ZP<ChannerRWClose_Bin> NetAddress::QCall()
	{
	if (ZP<ChannerRWClose_Bin> theChanner = this->Connect())
		return theChanner;
	return null;
	}

ZP<NetAddressLookup> NetAddress::MakeLookup(size_t iMaxNames)
	{ return null; }

// =================================================================================================
#pragma mark - NetAddressLookup

NetAddressLookup::NetAddressLookup()
	{}

NetAddressLookup::~NetAddressLookup()
	{}

// =================================================================================================
#pragma mark - NetName

NetName::NetName()
	{}

NetName::~NetName()
	{}

ZP<ChannerRWClose_Bin> NetName::QCall()
	{
	if (ZP<ChannerRWClose_Bin> theChanner = this->Connect())
		return theChanner;
	return null;
	}

ZP<ChannerRWClose_Bin> NetName::Connect()
	{
	if (ZP<NetNameLookup> theLookup = this->MakeLookup(10))
		{
		theLookup->Start();
		while (not theLookup->Finished())
			{
			if (ZP<ChannerRWClose_Bin> theConnection = theLookup->CurrentAddress()->Connect())
				return theConnection;
			theLookup->Advance();
			}
		}
	return null;
	}

// =================================================================================================
#pragma mark - NetNameLookup

NetNameLookup::NetNameLookup()
	{}

NetNameLookup::~NetNameLookup()
	{}

// =================================================================================================
#pragma mark - NetListener

NetListener::NetListener()
	{}

NetListener::~NetListener()
	{}

ZP<ChannerRWClose_Bin> NetListener::QCall()
	{
	if (ZP<ChannerRWClose_Bin> theChanner = this->Listen())
		return theChanner;
	return null;
	}

ZP<NetAddress> NetListener::GetAddress()
	{ return null; }

} // namespace ZooLib
