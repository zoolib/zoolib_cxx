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

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamerFromStreamR

class ZStreamerFromStreamR::StreamerR
:	public ZStreamerR
,	public ZStreamR
	{
public:
	StreamerR(const ZStreamR& r)
	:	fStreamR(&r)
		{}
	
	virtual ~StreamerR()
		{}

// From ZStreamerR
	virtual const ZStreamR& GetStreamR()
		{ return *this; }

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
		{
		if (fStreamR)
			fStreamR->Read(oDest, iCount, oCountRead);
		else if (oCountRead)
			*oCountRead = 0;
		}

	virtual size_t Imp_CountReadable()
		{
		if (fStreamR)
			return fStreamR->CountReadable();
		return 0;
		}

	virtual bool Imp_WaitReadable(double iTimeout)
		{
		if (fStreamR)
			return fStreamR->WaitReadable(iTimeout);
		return true;
		}

// Our protocol
	void pDetach()
		{
		ZAssert(fStreamR);
		fStreamR = nullptr;
		}

private:
	const ZStreamR* fStreamR;
	};

ZStreamerFromStreamR::ZStreamerFromStreamR(const ZStreamR& r)
:	fStreamerR(new StreamerR(r))
	{}

ZStreamerFromStreamR::~ZStreamerFromStreamR()
	{ fStreamerR->pDetach(); }

ZStreamerFromStreamR::operator ZRef<ZStreamerR>()
	{ return fStreamerR; }

} // namespace ZooLib
