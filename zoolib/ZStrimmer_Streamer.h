/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStrimmer_Streamer__
#define __ZStrimmer_Streamer__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrimmer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerR_Streamer_T

template <class StrimR_t>
class ZStrimmerR_Streamer_T : public ZStrimmerR
	{
protected:
	ZStrimmerR_Streamer_T() {}

public:
	virtual ~ZStrimmerR_Streamer_T() {}

	template <class P>
	ZStrimmerR_Streamer_T(P& iParam, ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iParam, iStreamerR->GetStreamR())
		{}

	template <class P>
	ZStrimmerR_Streamer_T(const P& iParam, ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iParam, iStreamerR->GetStreamR())
		{}

	ZStrimmerR_Streamer_T(ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iStreamerR->GetStreamR())
		{}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR() { return fStrimR; }

// Our protocol
	StrimR_t& GetStrim() { return fStrimR; }

protected:
	ZRef<ZStreamerR> fStreamerR;
	StrimR_t fStrimR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerU_Streamer_T

template <class StrimU_t>
class ZStrimmerU_Streamer_T : public ZStrimmerU
	{
protected:
	ZStrimmerU_Streamer_T() {}

public:
	virtual ~ZStrimmerU_Streamer_T() {}

	template <class P>
	ZStrimmerU_Streamer_T(P& iParam, ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iParam, iStreamerR->GetStreamR())
		{}

	template <class P>
	ZStrimmerU_Streamer_T(const P& iParam, ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iParam, iStreamerR->GetStreamR())
		{}

	ZStrimmerU_Streamer_T(ZRef<ZStreamerR> iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iStreamerR->GetStreamR())
		{}

// From ZStrimmerU
	virtual const ZStrimU& GetStrimU() { return fStrimU; }

// Our protocol
	StrimU_t& GetStrim() { return fStrimU; }

protected:
	ZRef<ZStreamerR> fStreamerR;
	StrimU_t fStrimU;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerW_Streamer_T

template <class StrimW_t>
class ZStrimmerW_Streamer_T : public ZStrimmerW
	{
protected:
	ZStrimmerW_Streamer_T() {}

public:
	virtual ~ZStrimmerW_Streamer_T() {}

	template <class P>
	ZStrimmerW_Streamer_T(P& iParam, ZRef<ZStreamerW> iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iParam, iStreamerW->GetStreamW())
		{}

	template <class P>
	ZStrimmerW_Streamer_T(const P& iParam, ZRef<ZStreamerW> iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iParam, iStreamerW->GetStreamW())
		{}

	ZStrimmerW_Streamer_T(ZRef<ZStreamerW> iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iStreamerW->GetStreamW())
		{}

// From ZStrimmerW
	virtual const ZStrimW& GetStrimW() { return fStrimW; }

// Our protocol
	StrimW_t& GetStrim() { return fStrimW; }

protected:
	ZRef<ZStreamerW> fStreamerW;
	StrimW_t fStrimW;
	};

} // namespace ZooLib

#endif // __ZStrimmer_Streamer__
