// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZNet_Local_h__
#define __ZNet_Local_h__ 1
#include "zconfig.h"

#include "zoolib/FunctionChain.h"
#include "zoolib/Multi.h"

#include "zoolib/ZNet.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZNetAddress_Local

class ZNetAddress_Local : public ZNetAddress
	{
public:
	ZNetAddress_Local();
	ZNetAddress_Local(const ZNetAddress_Local& other);
	ZNetAddress_Local(const std::string& iPath);
	virtual ~ZNetAddress_Local();

// From ZNetAddress
	virtual ZRef<ZNetEndpoint> Connect() const;

// Our protocol
	const std::string& GetPath() const;

private:
	std::string fPath;
	};

// =================================================================================================
#pragma mark - ZNetName_Local

class ZNetName_Local : public ZNetName
	{
public:
	typedef std::string LookupParam_t;

	ZNetName_Local();
	ZNetName_Local(const ZNetName_Local& other);
	ZNetName_Local(const std::string& iPath);
	virtual ~ZNetName_Local();

// From ZNetName
	virtual std::string AsString() const;
	virtual ZRef<ZNetNameLookup> CreateLookup(size_t iMaxAddresses) const;

// Our protocol
	const std::string& GetPath() const;

private:
	std::string fPath;
	};

// =================================================================================================
#pragma mark - ZNetListener_TCP

class ZNetListener_Local : public virtual ZNetListener
	{
public:
	typedef Multi1<std::string> MakeParam_t;

	static ZRef<ZNetListener_Local> sCreate(
		const std::string& iPath);
	};

// =================================================================================================
#pragma mark - ZNetEndpoint_Local

class ZNetEndpoint_Local : public virtual ZNetEndpoint
	{
public:
	typedef Multi1<std::string> MakeParam_t;

	static ZRef<ZNetEndpoint_Local> sCreateConnected(const std::string& iPath);
	};

} // namespace ZooLib

#endif // __ZNet_Local_h__
