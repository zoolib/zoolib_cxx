// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZNet_Local.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark - ZNetAddress_Local

ZNetAddress_Local::ZNetAddress_Local()
	{}

ZNetAddress_Local::ZNetAddress_Local(const ZNetAddress_Local& other)
:	fPath(other.fPath)
	{}

ZNetAddress_Local::ZNetAddress_Local(const string& iPath)
:	fPath(iPath)
	{}

ZNetAddress_Local::~ZNetAddress_Local()
	{}

ZRef<ZNetEndpoint> ZNetAddress_Local::Connect() const
	{ return ZNetEndpoint_Local::sCreateConnected(fPath); }

const string& ZNetAddress_Local::GetPath() const
	{ return fPath ;}

// =================================================================================================
#pragma mark - ZNetName_Local

ZNetName_Local::ZNetName_Local()
	{}

ZNetName_Local::ZNetName_Local(const ZNetName_Local& other)
:	fPath(other.fPath)
	{}

ZNetName_Local::ZNetName_Local(const string& iPath)
:	fPath(iPath)
	{}

ZNetName_Local::~ZNetName_Local()
	{}

string ZNetName_Local::AsString() const
	{ return fPath; }

ZRef<ZNetNameLookup> ZNetName_Local::CreateLookup(size_t iMaxAddresses) const
	{
	return FunctionChain<ZRef<ZNetNameLookup>, LookupParam_t>
		::sInvoke(LookupParam_t(fPath, iMaxAddresses));
	}

const string& ZNetName_Local::GetPath() const
	{ return fPath; }

// =================================================================================================
#pragma mark - ZNetListener_Local

ZRef<ZNetListener_Local> ZNetListener_Local::sCreate(const string& iPath)
	{
	return FunctionChain<ZRef<ZNetListener_Local>, MakeParam_t>
		::sInvoke(MakeParam_t(iPath));
	}

// =================================================================================================
#pragma mark - ZNetEndpoint_Local

ZRef<ZNetEndpoint_Local> ZNetEndpoint_Local::sCreateConnected(const string& iPath)
	{
	return FunctionChain<ZRef<ZNetEndpoint_Local>, MakeParam_t>
		::sInvoke(MakeParam_t(iPath));
	}

} // namespace ZooLib
