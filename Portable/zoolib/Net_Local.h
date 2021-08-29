// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Net_Local_h__
#define __ZooLib_Net_Local_h__ 1
#include "zconfig.h"

#include "zoolib/FunctionChain.h"
#include "zoolib/Multi.h"

#include "zoolib/Net.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - NetAddress_Local

class NetAddress_Local
:	public NetAddress
	{
public:
	NetAddress_Local();
	NetAddress_Local(const NetAddress_Local& other);
	NetAddress_Local(const std::string& iPath);
	virtual ~NetAddress_Local();

// From NetAddress
	virtual ZP<ChannerRWCon_Bin> Connect();

// Our protocol
	const std::string& GetPath();

private:
	const std::string fPath;
	};

// =================================================================================================
#pragma mark - NetName_Local

class NetName_Local
:	public NetName
	{
public:
	typedef std::string LookupParam_t;

	NetName_Local();
	NetName_Local(const NetName_Local& other);
	NetName_Local(const std::string& iPath);
	virtual ~NetName_Local();

// From NetName
	virtual std::string AsString();
	virtual ZP<NetNameLookup> MakeLookup(size_t iMaxAddresses);

// Our protocol
	const std::string& GetPath();

private:
	const std::string fPath;
	};

// =================================================================================================
#pragma mark - NetListener_Local

class NetListener_Local
:	public virtual NetListener
	{
public:
	};

ZP<NetListener_Local> sNetListener_Local(const std::string& iPath);

// =================================================================================================
#pragma mark - sConnect_Local

ZP<ChannerRWCon_Bin> sConnect_Local(const std::string& iPath);

} // namespace ZooLib

#endif // __ZooLib_Net_Local_h__
