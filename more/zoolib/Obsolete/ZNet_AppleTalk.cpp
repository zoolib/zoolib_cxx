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

#include "zoolib/ZNet_AppleTalk.h"

using std::string;

namespace ZooLib {

// ==================================================
// MARK: - ZNetAddress_AppleTalk

ZNetAddress_AppleTalk::ZNetAddress_AppleTalk()
:	fNet(0),
	fNode(0),
	fSocket(0)
	{}

ZNetAddress_AppleTalk::ZNetAddress_AppleTalk(int16 iNet, uint8 iNode, int8 iSocket)
:	fNet(iNet),
	fNode(iNode),
	fSocket(iSocket)
	{}

ZNetAddress_AppleTalk::~ZNetAddress_AppleTalk()
	{}

ZRef<ZNetEndpoint> ZNetAddress_AppleTalk::Connect() const
	{ return ZNetEndpoint_ADSP::sCreateConnectedEndpoint(fNet, fNode, fSocket); }

// ==================================================
// MARK: - ZNetName_AppleTalk

ZNetName_AppleTalk::ZNetName_AppleTalk()
	{}

ZNetName_AppleTalk::ZNetName_AppleTalk(const string& iName, const string& iType)
:	fName(iName),
	fType(iType)
	{}

ZNetName_AppleTalk::ZNetName_AppleTalk(
	const string& iName, const string& iType, const string& iZone)
:	fName(iName),
	fType(iType),
	fZone(iZone)
	{}

ZNetName_AppleTalk::~ZNetName_AppleTalk()
	{}

string ZNetName_AppleTalk::AsString() const
	{ return fName; }

ZRef<ZNetNameLookup> ZNetName_AppleTalk::CreateLookup(size_t iMaxAddresses) const
	{
#if ZCONFIG(API_Net, MacClassic)
	return new ZNetNameLookup_AppleTalk_MacClassic(fName, fType, fZone, iMaxAddresses);
#endif // ZCONFIG(API_Net, MacClassic)

	return null;
	}

string ZNetName_AppleTalk::GetType() const
	{ return fType; }

string ZNetName_AppleTalk::GetZone() const
	{ return fZone; }

ZRef<ZNetNameRegistered_AppleTalk> ZNetName_AppleTalk::RegisterSocket(uint8 iSocket) const
	{
#if ZCONFIG(API_Net, MacClassic)
	ZRef<ZNetNameRegistered_AppleTalk_MacClassic> theNN =
		new ZNetNameRegistered_AppleTalk_MacClassic(fName, fType, fZone, iSocket);
	if (theNN->IsRegistrationGood())
		return theNN;
#endif // ZCONFIG(API_Net, MacClassic)
	return null;
	}

// ==================================================
// MARK: - ZNetNameRegistered_AppleTalk

ZNetNameRegistered_AppleTalk::ZNetNameRegistered_AppleTalk()
	{}

ZNetNameRegistered_AppleTalk::~ZNetNameRegistered_AppleTalk()
	{}

// ==================================================
// MARK: - ZNetListener_ADSP

ZNetListener_ADSP::ZNetListener_ADSP()
	{}

ZNetListener_ADSP::~ZNetListener_ADSP()
	{}

ZRef<ZNetListener_ADSP> ZNetListener_ADSP::sCreateListener()
	{
#if ZCONFIG(API_Net, MacClassic)
	return new ZNetListener_ADSP_MacClassic;
#endif // ZCONFIG(API_Net, MacClassic)

	return null;
	}

// ==================================================
// MARK: - ZNetEndpoint_ADSP

ZRef<ZNetEndpoint_ADSP> ZNetEndpoint_ADSP::sCreateConnectedEndpoint(
	int16 iNet, uint8 iNode, int8 iSocket)
	{
	try
		{
#if ZCONFIG(API_Net, MacClassic)
		return new ZNetEndpoint_ADSP_MacClassic(iNet, iNode, iSocket);
#endif // ZCONFIG(API_Net, MacClassic)
		}
	catch (...)
		{}
	return null;
	}

} // namespace ZooLib
