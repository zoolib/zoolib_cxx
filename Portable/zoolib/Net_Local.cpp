// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Net_Local.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark - NetAddress_Local

NetAddress_Local::NetAddress_Local()
	{}

NetAddress_Local::NetAddress_Local(const NetAddress_Local& other)
:	fPath(other.fPath)
	{}

NetAddress_Local::NetAddress_Local(const string& iPath)
:	fPath(iPath)
	{}

NetAddress_Local::~NetAddress_Local()
	{}

ZP<ChannerRWClose_Bin> NetAddress_Local::Connect()
	{ return sConnect_Local(fPath); }

const string& NetAddress_Local::GetPath()
	{ return fPath ;}

// =================================================================================================
#pragma mark - NetName_Local

NetName_Local::NetName_Local()
	{}

NetName_Local::NetName_Local(const NetName_Local& other)
:	fPath(other.fPath)
	{}

NetName_Local::NetName_Local(const string& iPath)
:	fPath(iPath)
	{}

NetName_Local::~NetName_Local()
	{}

string NetName_Local::AsString()
	{ return fPath; }

ZP<NetNameLookup> NetName_Local::MakeLookup(size_t iMaxAddresses)
	{
	return FunctionChain<ZP<NetNameLookup>, LookupParam_t>
		::sInvoke(fPath);
	}

const string& NetName_Local::GetPath()
	{ return fPath; }

// =================================================================================================
#pragma mark - NetListener_Local

ZP<NetListener_Local> sNetListener_Local(const std::string& iPath)
	{
	return FunctionChain<ZP<NetListener_Local>, std::string>
		::sInvoke(iPath);
	}

// =================================================================================================
#pragma mark - NetEndpoint_Local

ZP<ChannerRWClose_Bin> sConnect_Local(const string& iPath)
	{
	return FunctionChain<ZP<ChannerRWClose_Bin>, std::string>
		::sInvoke(iPath);
	}

} // namespace ZooLib
