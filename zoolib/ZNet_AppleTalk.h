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

#ifndef __ZNet_AppleTalk_h__
#define __ZNet_AppleTalk_h__ 1
#include "zconfig.h"

#include "zoolib/ZNet.h"
#include "zoolib/ZTypes.h"

namespace ZooLib {

// ==================================================
// MARK: - ZNetAddress_AppleTalk

class ZNetAddress_AppleTalk : public ZNetAddress
	{
	friend class ZNetName_AppleTalk;
public:
	ZNetAddress_AppleTalk();
	//FIXME -- int16 or uint16? and what about iSocket?
	ZNetAddress_AppleTalk(int16 iNet, uint8 iNode, int8 iSocket);
	virtual ~ZNetAddress_AppleTalk();

// From ZNetAddress
	virtual ZRef<ZNetEndpoint> Connect() const;

private:
	int16 fNet;
	uint8 fNode;
	uint8 fSocket;
	};

// ==================================================
// MARK: - ZNetName_AppleTalk

class ZNetNameRegistered_AppleTalk;

class ZNetName_AppleTalk : public ZNetName
	{
	ZNetName_AppleTalk();
public:
	ZNetName_AppleTalk(const std::string& iName, const std::string& iType);
	ZNetName_AppleTalk(
		const std::string& iName, const std::string& iType, const std::string& iZone);
	virtual ~ZNetName_AppleTalk();

// From ZNetName
	virtual std::string AsString() const;
	virtual ZRef<ZNetNameLookup> CreateLookup(size_t iMaxAddresses) const;

// Our protocol
	std::string GetType() const;
	std::string GetZone() const;

	ZRef<ZNetNameRegistered_AppleTalk> RegisterSocket(uint8 iSocket) const;

private:
	std::string fName;
	std::string fType;
	std::string fZone;
	};

// ==================================================
// MARK: - ZNetNameRegistered_AppleTalk

class ZNetNameRegistered_AppleTalk : public ZCounted
	{
protected:
	ZNetNameRegistered_AppleTalk();

public:
	virtual ~ZNetNameRegistered_AppleTalk();

	virtual std::string GetName() const = 0;
	virtual std::string GetType() const = 0;
	virtual std::string GetZone() const = 0;
	virtual uint8 GetSocket() const = 0;
	};

// ==================================================
// MARK: - ZNetListener_ADSP

class ZNetListener_ADSP : public ZNetListener
	{
public:
	ZNetListener_ADSP();
	virtual ~ZNetListener_ADSP();

	virtual uint8 GetSocket() = 0;

	static ZRef<ZNetListener_ADSP> sCreateListener();
	};

// ==================================================
// MARK: - ZNetEndpoint_ADSP

class ZNetEndpoint_ADSP : public ZNetEndpoint
	{
public:
	static ZRef<ZNetEndpoint_ADSP> sCreateConnectedEndpoint(int16 iNet, uint8 iNode, int8 iSocket);
	};

} // namespace ZooLib

#endif // __ZNet_AppleTalk_h__
