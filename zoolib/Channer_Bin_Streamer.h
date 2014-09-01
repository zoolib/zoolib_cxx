/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Channer_Bin_Stream_h__
#define __ZooLib_Channer_Bin_Stream_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_Bin_Stream.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChannerR_Bin_Streamer

class ChannerR_Bin_Streamer
:	public ChannerR_Bin
,	public ChanR_Bin_Stream
	{
public:
	ChannerR_Bin_Streamer(const ZRef<ZStreamerR>& iStreamerR)
	:	ChanR_Bin_Stream(iStreamerR->GetStreamR())
	,	fStreamerR(iStreamerR)
		{}

// From ChannerR_Bin
	virtual const ChanR_Bin_Stream& GetChanR()
		{ return *this; }

private:
	const ZRef<ZStreamerR> fStreamerR;
	};

// =================================================================================================
// MARK: - ChannerW_Bin_Streamer

class ChannerW_Bin_Streamer
:	public ChannerW_Bin
,	public ChanW_Bin_Stream
	{
public:
	ChannerW_Bin_Streamer(const ZRef<ZStreamerW>& iStreamerW)
	:	ChanW_Bin_Stream(iStreamerW->GetStreamW())
	,	fStreamerW(iStreamerW)
		{}

// From ChannerW_Bin
	virtual const ChanW_Bin_Stream& GetChanW()
		{ return *this; }

private:
	const ZRef<ZStreamerW> fStreamerW;
	};

} // namespace ZooLib

#endif // __ZooLib_Channer_Bin_Stream_h__
