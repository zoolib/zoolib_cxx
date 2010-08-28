/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTBRep_Client_StreamerRWFactory.h"
#include "zoolib/ZLog.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client_StreamerRWFactory

ZTBRep_Client_StreamerRWFactory::ZTBRep_Client_StreamerRWFactory(
	ZRef<ZStreamerRWFactory> iStreamerRWFactory)
:	fStreamerRWFactory(iStreamerRWFactory)
	{}

ZTBRep_Client_StreamerRWFactory::~ZTBRep_Client_StreamerRWFactory()
	{
	// If we've been Start-ed, force reader and write to exit
	// before we destroy our ZTBRep_Client_StreamerRWFactory vtbl.
	this->Stop();
	}

ZRef<ZStreamerRW> ZTBRep_Client_StreamerRWFactory::EstablishConnection()
	{
	if (fStreamerRWFactory)
		return fStreamerRWFactory->MakeStreamerRW();
	return null;
	}

bool ZTBRep_Client_StreamerRWFactory::Reader(const ZStreamR& iStreamR)
	{ return ZTBRep_Client::Reader(iStreamR); }

bool ZTBRep_Client_StreamerRWFactory::Writer(const ZStreamW& iStreamW)
	{ return ZTBRep_Client::Writer(iStreamW); }

void ZTBRep_Client_StreamerRWFactory::Notify_Established()
	{
	if (ZLOG(s, eDebug, "ZTBRep_Client_StreamerRWFactory"))
		s << "Notify_Established";

	ZClientAsync::Notify_Established();
	ZTBRep_Client::Live();
	}

void ZTBRep_Client_StreamerRWFactory::Notify_Failed()
	{
	if (ZLOG(s, eDebug, "ZTBRep_Client_StreamerRWFactory"))
		s << "Notify_Failed";

	ZTBRep_Client::Fail();
	ZClientAsync::Notify_Failed();
	}

} // namespace ZooLib
