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

#ifndef __ZStrimmer_Streamer_h__
#define __ZStrimmer_Streamer_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrimmer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimmerR_Streamer_T

template <class StrimR_p>
class ZStrimmerR_Streamer_T : public ZStrimmerR
	{
protected:
	ZStrimmerR_Streamer_T() {}

public:
	virtual ~ZStrimmerR_Streamer_T() {}

	template <class P>
	ZStrimmerR_Streamer_T(P& iParam, const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iParam, iStreamerR->GetStreamR())
		{}

	template <class P>
	ZStrimmerR_Streamer_T(const P& iParam, const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iParam, iStreamerR->GetStreamR())
		{}

	ZStrimmerR_Streamer_T(const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimR(iStreamerR->GetStreamR())
		{}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR() { return fStrimR; }

// Our protocol
	StrimR_p& GetStrim() { return fStrimR; }

protected:
	ZRef<ZStreamerR> fStreamerR;
	StrimR_p fStrimR;
	};

template <class StrimR_p, class Streamer_p>
ZRef<ZStrimmerR> sStrimmerR_Streamer_T(const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerR_Streamer_T<StrimR_p>(iStreamer);
	return null;
	}

template <class StrimR_p, class Param_p, class Streamer_p>
ZRef<ZStrimmerR> sStrimmerR_Streamer_T(const Param_p& iParam, const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerR_Streamer_T<StrimR_p>(iParam, iStreamer);
	return null;
	}

// =================================================================================================
// MARK: - ZStrimmerU_Streamer_T

template <class StrimU_p>
class ZStrimmerU_Streamer_T : public ZStrimmerU
	{
protected:
	ZStrimmerU_Streamer_T() {}

public:
	virtual ~ZStrimmerU_Streamer_T() {}

	template <class P>
	ZStrimmerU_Streamer_T(P& iParam, const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iParam, iStreamerR->GetStreamR())
		{}

	template <class P>
	ZStrimmerU_Streamer_T(const P& iParam, const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iParam, iStreamerR->GetStreamR())
		{}

	ZStrimmerU_Streamer_T(const ZRef<ZStreamerR>& iStreamerR)
	:	fStreamerR(iStreamerR),
		fStrimU(iStreamerR->GetStreamR())
		{}

// From ZStrimmerU
	virtual const ZStrimU& GetStrimU() { return fStrimU; }

// Our protocol
	StrimU_p& GetStrim() { return fStrimU; }

protected:
	ZRef<ZStreamerR> fStreamerR;
	StrimU_p fStrimU;
	};

template <class StrimU_p, class Streamer_p>
ZRef<ZStrimmerU> sStrimmerU_Streamer_T(const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerU_Streamer_T<StrimU_p>(iStreamer);
	return null;
	}

template <class StrimU_p, class Param_p, class Streamer_p>
ZRef<ZStrimmerU> sStrimmerU_Streamer_T(const Param_p& iParam, const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerU_Streamer_T<StrimU_p>(iParam, iStreamer);
	return null;
	}

// =================================================================================================
// MARK: - ZStrimmerW_Streamer_T

template <class StrimW_p>
class ZStrimmerW_Streamer_T : public ZStrimmerW
	{
protected:
	ZStrimmerW_Streamer_T() {}

public:
	virtual ~ZStrimmerW_Streamer_T() {}

	template <class P>
	ZStrimmerW_Streamer_T(P& iParam, const ZRef<ZStreamerW>& iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iParam, iStreamerW->GetStreamW())
		{}

	template <class P>
	ZStrimmerW_Streamer_T(const P& iParam, const ZRef<ZStreamerW>& iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iParam, iStreamerW->GetStreamW())
		{}

	ZStrimmerW_Streamer_T(const ZRef<ZStreamerW>& iStreamerW)
	:	fStreamerW(iStreamerW),
		fStrimW(iStreamerW->GetStreamW())
		{}

// From ZStrimmerW
	virtual const ZStrimW& GetStrimW() { return fStrimW; }

// Our protocol
	StrimW_p& GetStrim() { return fStrimW; }

protected:
	ZRef<ZStreamerW> fStreamerW;
	StrimW_p fStrimW;
	};


template <class StrimW_p, class Streamer_p>
ZRef<ZStrimmerW> sStrimmerW_Streamer_T(const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerW_Streamer_T<StrimW_p>(iStreamer);
	return null;
	}

template <class StrimW_p, class Param_p, class Streamer_p>
ZRef<ZStrimmerW> sStrimmerW_Streamer_T(const Param_p& iParam, const ZRef<Streamer_p>& iStreamer)
	{
	if (iStreamer)
		return new ZStrimmerW_Streamer_T<StrimW_p>(iParam, iStreamer);
	return null;
	}

} // namespace ZooLib

#endif // __ZStrimmer_Streamer_h__
