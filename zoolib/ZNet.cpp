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

#include "zoolib/ZNet.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEx

ZNetEx::ZNetEx(ZNet::Error iError)
:	fError(iError),
	runtime_error("ZNetEx")
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddress

ZNetAddress::~ZNetAddress()
	{}

ZRef<ZStreamerRWCon> ZNetAddress::MakeStreamerRWCon()
	{ return this->Connect(); }

ZRef<ZNetAddressLookup> ZNetAddress::CreateLookup(size_t iMaxNames) const
	{ return ZRef<ZNetAddressLookup>(); };

ZRef<ZNetEndpoint> ZNetAddress::Connect() const
	{ return ZRef<ZNetEndpoint>(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetAddressLookup

ZNetAddressLookup::~ZNetAddressLookup()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetName

ZNetName::~ZNetName()
	{}

ZRef<ZStreamerRWCon> ZNetName::MakeStreamerRWCon()
	{ return this->Connect(); }

ZRef<ZNetNameLookup> ZNetName::CreateLookup(size_t iMaxAddresses) const
	{ return ZRef<ZNetNameLookup>(); };

ZRef<ZNetEndpoint> ZNetName::Connect() const
	{ return this->Connect(1); }

ZRef<ZNetEndpoint> ZNetName::Connect(size_t iMaxAddresses) const
	{
	if (ZRef<ZNetNameLookup> theLookup = this->CreateLookup(iMaxAddresses))
		{
		theLookup->Start();
		while (!theLookup->Finished())
			{
			if (ZRef<ZNetEndpoint> theEP = theLookup->CurrentAddress()->Connect())
				return theEP;
			theLookup->Advance();
			}
		}
	return ZRef<ZNetEndpoint>();
	}

ZRef<ZNetAddress> ZNetName::Lookup() const
	{
	if (ZRef<ZNetNameLookup> theLookup = this->CreateLookup(1))
		{
		theLookup->Start();
		if (!theLookup->Finished())
			return theLookup->CurrentAddress();
		}
	return ZRef<ZNetAddress>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetNameLookup

ZNetNameLookup::~ZNetNameLookup()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZNetListener

ZNetListener::~ZNetListener()
	{}

void ZNetListener::Cancel()
	{ this->CancelListen(); }

ZRef<ZStreamerRWCon> ZNetListener::MakeStreamerRWCon()
	{ return this->Listen(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpoint

ZNetEndpoint::ZNetEndpoint()
	{}

ZNetEndpoint::~ZNetEndpoint()
	{}

ZRef<ZNetAddress> ZNetEndpoint::GetLocalAddress()
	{ return ZRef<ZNetAddress>(); }

bool ZNetEndpoint::WaitTillReadable(int iMilliseconds)
	{ return this->GetStreamRCon().WaitReadable(iMilliseconds); }

bool ZNetEndpoint::ReceiveDisconnect(int iMilliseconds)
	{ return this->GetStreamRCon().ReceiveDisconnect(iMilliseconds); }

void ZNetEndpoint::SendDisconnect()
	{ this->GetStreamWCon().SendDisconnect(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZNetEndpointDG

ZNetEndpointDG::ZNetEndpointDG()
	{}

ZNetEndpointDG::~ZNetEndpointDG()
	{}

