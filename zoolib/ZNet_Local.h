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

#ifndef __ZNet_Local_h__
#define __ZNet_Local_h__ 1
#include "zconfig.h"

#include "zoolib/ZMulti_T.h"
#include "zoolib/ZNet.h"

#include <string>

namespace ZooLib {

// =================================================================================================
// MARK: - ZNetAddress_Local

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
// MARK: - ZNetName_Local

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
// MARK: - ZNetListener_TCP

class ZNetListener_Local : public virtual ZNetListener
	{
public:
	typedef ZMulti_T1<std::string> MakeParam_t;

	static ZRef<ZNetListener_Local> sCreate(
		const std::string& iPath);
	};

// =================================================================================================
// MARK: - ZNetEndpoint_Local

class ZNetEndpoint_Local : public virtual ZNetEndpoint
	{
public:
	typedef ZMulti_T1<std::string> MakeParam_t;

	static ZRef<ZNetEndpoint_Local> sCreateConnected(const std::string& iPath);
	};

} // namespace ZooLib

#endif // __ZNet_Local_h__
