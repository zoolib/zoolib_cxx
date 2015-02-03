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

#include "zoolib/Net.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark NetEx

NetEx::NetEx(Net::Error iError)
:	runtime_error("NetEx")
,	fError(iError)
	{}

// =================================================================================================
#pragma mark -
#pragma mark NetAddress

NetAddress::NetAddress()
	{}

NetAddress::~NetAddress()
	{}

ZRef<NetAddressLookup> NetAddress::MakeLookup(size_t iMaxNames)
	{ return null; }

// =================================================================================================
#pragma mark -
#pragma mark NetAddressLookup

NetAddressLookup::NetAddressLookup()
	{}

NetAddressLookup::~NetAddressLookup()
	{}

// =================================================================================================
#pragma mark -
#pragma mark NetName

NetName::NetName()
	{}

NetName::~NetName()
	{}

ZRef<ChannerRWClose_Bin> NetName::Make()
	{
	if (ZRef<NetNameLookup> theLookup = this->MakeLookup(10))
		{
		theLookup->Start();
		while (not theLookup->Finished())
			{
			if (ZRef<ChannerRWClose_Bin> theQ = sCall(theLookup->CurrentAddress()))
				return theQ;
			theLookup->Advance();
			}
		}
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark NetNameLookup

NetNameLookup::NetNameLookup()
	{}

NetNameLookup::~NetNameLookup()
	{}

// =================================================================================================
#pragma mark -
#pragma mark NetListener

NetListener::NetListener()
	{}

NetListener::~NetListener()
	{}

ZRef<NetAddress> NetListener::GetAddress()
	{ return null; }

} // namespace ZooLib
