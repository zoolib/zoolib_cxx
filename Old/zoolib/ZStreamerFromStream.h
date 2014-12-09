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

#ifndef __ZStreamerFromStream_h__
#define __ZStreamerFromStream_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamerFromStreamR

class ZStreamerFromStreamR
	{
public:
	ZStreamerFromStreamR(const ZStreamR& iStream);
	~ZStreamerFromStreamR();

	operator ZRef<ZStreamerR>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamRCon

class ZStreamerFromStreamRCon
	{
public:
	ZStreamerFromStreamRCon(const ZStreamRCon& iStream);
	~ZStreamerFromStreamRCon();

	operator ZRef<ZStreamerRCon>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamU

class ZStreamerFromStreamU
	{
public:
	ZStreamerFromStreamU(const ZStreamU& iStream);
	~ZStreamerFromStreamU();

	operator ZRef<ZStreamerU>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamRPos

class ZStreamerFromStreamRPos
	{
public:
	ZStreamerFromStreamRPos(const ZStreamRPos& iStream);
	~ZStreamerFromStreamRPos();

	operator ZRef<ZStreamerRPos>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamW

class ZStreamerFromStreamW
	{
public:
	ZStreamerFromStreamW(const ZStreamW& iStream);
	~ZStreamerFromStreamW();

	operator ZRef<ZStreamerW>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamWCon

class ZStreamerFromStreamWCon
	{
public:
	ZStreamerFromStreamWCon(const ZStreamWCon& iStream);
	~ZStreamerFromStreamWCon();

	operator ZRef<ZStreamerWCon>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamWPos

class ZStreamerFromStreamWPos
	{
public:
	ZStreamerFromStreamWPos(const ZStreamWPos& iStream);
	~ZStreamerFromStreamWPos();

	operator ZRef<ZStreamerWPos>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamRWCon

class ZStreamerFromStreamRWCon
	{
public:
	ZStreamerFromStreamRWCon(const ZStreamRCon& r, const ZStreamWCon& w);
	~ZStreamerFromStreamRWCon();

	operator ZRef<ZStreamerRWCon>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

// =================================================================================================
// MARK: - ZStreamerFromStreamRWPos

class ZStreamerFromStreamRWPos
	{
public:
	ZStreamerFromStreamRWPos(const ZStreamRWPos& iStream);
	~ZStreamerFromStreamRWPos();

	operator ZRef<ZStreamerRWPos>();

private:
	class Streamer;
	ZRef<Streamer> fStreamer;
	};

} // namespace ZooLib

#endif // __ZStreamerFromStream_h__
