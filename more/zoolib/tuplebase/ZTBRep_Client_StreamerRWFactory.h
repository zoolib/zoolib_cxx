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

#ifndef __ZTBRep_Client_StreamerRWFactory__
#define __ZTBRep_Client_StreamerRWFactory__ 1

#include "zoolib/ZClient.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/tuplebase/ZTBRep_Client.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBRep_Client_StreamerRWFactory

class ZTBRep_Client_StreamerRWFactory : public ZTBRep_Client, public ZClientAsync
	{
public:
	ZTBRep_Client_StreamerRWFactory(ZRef<ZStreamerRWFactory> iStreamerRWFactory);
	virtual ~ZTBRep_Client_StreamerRWFactory();

// From ZClientAsync
	virtual ZRef<ZStreamerRW> EstablishConnection();

	virtual bool Reader(const ZStreamR& iStreamR);
	virtual bool Writer(const ZStreamW& iStreamW);

	virtual void Notify_Established();
	virtual void Notify_Failed();

private:
	ZRef<ZStreamerRWFactory> fStreamerRWFactory;
	};

} // namespace ZooLib

#endif // __ZTBRep_Client_StreamerRWFactory__
