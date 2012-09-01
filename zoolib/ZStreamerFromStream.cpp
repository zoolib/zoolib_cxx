/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStreamerFromStream.h"
#include "zoolib/ZStream_Indirect.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamerFromStreamR

class ZStreamerFromStreamR::Streamer : public ZStreamerR, public ZStreamR_Indirect
	{
public:
	Streamer(const ZStreamR& iStream) : ZStreamR_Indirect(&iStream) {}
	virtual const ZStreamR& GetStreamR() { return *this; }
	};

ZStreamerFromStreamR::ZStreamerFromStreamR(const ZStreamR& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamR::~ZStreamerFromStreamR()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamR::operator ZRef<ZStreamerR>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamRCon

class ZStreamerFromStreamRCon::Streamer : public ZStreamerRCon, public ZStreamRCon_Indirect
	{
public:
	Streamer(const ZStreamRCon& iStream) : ZStreamRCon_Indirect(&iStream) {}
	virtual const ZStreamRCon& GetStreamRCon() { return *this; }
	};

ZStreamerFromStreamRCon::ZStreamerFromStreamRCon(const ZStreamRCon& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamRCon::~ZStreamerFromStreamRCon()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamRCon::operator ZRef<ZStreamerRCon>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamU

class ZStreamerFromStreamU::Streamer : public ZStreamerU, public ZStreamU_Indirect
	{
public:
	Streamer(const ZStreamU& iStream) : ZStreamU_Indirect(&iStream) {}
	virtual const ZStreamU& GetStreamU() { return *this; }
	};

ZStreamerFromStreamU::ZStreamerFromStreamU(const ZStreamU& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamU::~ZStreamerFromStreamU()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamU::operator ZRef<ZStreamerU>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamRPos

class ZStreamerFromStreamRPos::Streamer : public ZStreamerRPos, public ZStreamRPos_Indirect
	{
public:
	Streamer(const ZStreamRPos& iStream) : ZStreamRPos_Indirect(&iStream) {}
	virtual const ZStreamRPos& GetStreamRPos() { return *this; }
	};

ZStreamerFromStreamRPos::ZStreamerFromStreamRPos(const ZStreamRPos& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamRPos::~ZStreamerFromStreamRPos()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamRPos::operator ZRef<ZStreamerRPos>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamW

class ZStreamerFromStreamW::Streamer : public ZStreamerW, public ZStreamW_Indirect
	{
public:
	Streamer(const ZStreamW& iStream) : ZStreamW_Indirect(&iStream) {}
	virtual const ZStreamW& GetStreamW() { return *this; }
	};

ZStreamerFromStreamW::ZStreamerFromStreamW(const ZStreamW& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamW::~ZStreamerFromStreamW()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamW::operator ZRef<ZStreamerW>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamWCon

class ZStreamerFromStreamWCon::Streamer : public ZStreamerWCon, public ZStreamWCon_Indirect
	{
public:
	Streamer(const ZStreamWCon& iStream) : ZStreamWCon_Indirect(&iStream) {}
	virtual const ZStreamWCon& GetStreamWCon() { return *this; }
	};

ZStreamerFromStreamWCon::ZStreamerFromStreamWCon(const ZStreamWCon& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamWCon::~ZStreamerFromStreamWCon()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamWCon::operator ZRef<ZStreamerWCon>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamWPos

class ZStreamerFromStreamWPos::Streamer : public ZStreamerWPos, public ZStreamWPos_Indirect
	{
public:
	Streamer(const ZStreamWPos& iStream) : ZStreamWPos_Indirect(&iStream) {}
	virtual const ZStreamWPos& GetStreamWPos() { return *this; }
	};

ZStreamerFromStreamWPos::ZStreamerFromStreamWPos(const ZStreamWPos& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamWPos::~ZStreamerFromStreamWPos()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamWPos::operator ZRef<ZStreamerWPos>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamRWCon

class ZStreamerFromStreamRWCon::Streamer
:	public ZStreamerRWCon
,	public ZStreamRCon_Indirect
,	public ZStreamWCon_Indirect
	{
public:
	Streamer(const ZStreamRCon& r, const ZStreamWCon& w)
	:	ZStreamRCon_Indirect(&r)
	,	ZStreamWCon_Indirect(&w)
		{}
	virtual const ZStreamRCon& GetStreamRCon() { return *this; }
	virtual const ZStreamWCon& GetStreamWCon() { return *this; }
	};

ZStreamerFromStreamRWCon::ZStreamerFromStreamRWCon(const ZStreamRCon& r, const ZStreamWCon& w)
:	fStreamer(new Streamer(r, w))
	{}

ZStreamerFromStreamRWCon::~ZStreamerFromStreamRWCon()
	{
	fStreamer.StaticCast<ZStreamRCon_Indirect>()->Set(nullptr);
	fStreamer.StaticCast<ZStreamWCon_Indirect>()->Set(nullptr);
	}

ZStreamerFromStreamRWCon::operator ZRef<ZStreamerRWCon>()
	{ return fStreamer; }

// =================================================================================================
// MARK: - ZStreamerFromStreamRWPos

class ZStreamerFromStreamRWPos::Streamer : public ZStreamerRWPos, public ZStreamRWPos_Indirect
	{
public:
	Streamer(const ZStreamRWPos& iStream) : ZStreamRWPos_Indirect(&iStream) {}
	virtual const ZStreamRWPos& GetStreamRWPos() { return *this; }
	};

ZStreamerFromStreamRWPos::ZStreamerFromStreamRWPos(const ZStreamRWPos& iStream)
:	fStreamer(new Streamer(iStream))
	{}

ZStreamerFromStreamRWPos::~ZStreamerFromStreamRWPos()
	{ fStreamer->Set(nullptr); }

ZStreamerFromStreamRWPos::operator ZRef<ZStreamerRWPos>()
	{ return fStreamer; }

} // namespace ZooLib
