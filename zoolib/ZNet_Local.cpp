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

#include "zoolib/ZNet_Local.h"
#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZString.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Factories

ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetNameLookup>, ZNetName_Local::LookupParam_t);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetListener_Local>, string);
ZOOLIB_FACTORYCHAIN_HEAD(ZRef<ZNetEndpoint_Local>, string);

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress_Local

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
	{ return ZNetEndpoint_Local::sCreateConnectedEndpoint(fPath); }

const string& ZNetAddress_Local::GetPath() const
	{ return fPath ;}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName_Local

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
	return ZFactoryChain_T<ZRef<ZNetNameLookup>, LookupParam_t>
		::sMake(LookupParam_t(fPath, iMaxAddresses));
	}

const string& ZNetName_Local::GetPath() const
	{ return fPath; }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener_Local

ZRef<ZNetListener_Local> ZNetListener_Local::sCreateListener(
	const string& iPath, size_t iListenQueueSize)
	{
	return ZFactoryChain_T<ZRef<ZNetListener_Local>, string>
		::sMake(iPath);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint_Local

ZRef<ZNetEndpoint_Local> ZNetEndpoint_Local::sCreateConnectedEndpoint(
	const string& iPath)
	{
	return ZFactoryChain_T<ZRef<ZNetEndpoint_Local>, string>
		::sMake(iPath);
	}
