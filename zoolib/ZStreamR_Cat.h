/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStreamR_Cat__
#define __ZStreamR_Cat__ 1

#include "zconfig.h"

#include "zoolib/ZStreamer.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Cat

/// A read filter stream that reads from \a iStreamR1 until it's exhausted, then from \a iStreamR2.

class ZStreamR_Cat : public ZStreamR
	{
public:
	ZStreamR_Cat(const ZStreamR& iStreamR1, const ZStreamR& iStreamR2);
	~ZStreamR_Cat();

	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

protected:
	const ZStreamR& fStreamR1;
	const ZStreamR& fStreamR2;
	bool fFirstIsLive;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Cat

/// A read filter streamer encapsulating a ZStreamR_Cat.

class ZStreamerR_Cat : public ZStreamerR
	{
public:
	ZStreamerR_Cat(ZRef<ZStreamerR> iStreamerR1, ZRef<ZStreamerR> iStreamerR2);
	virtual ~ZStreamerR_Cat();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZRef<ZStreamerR> fStreamerR1;
	ZRef<ZStreamerR> fStreamerR2;
	ZStreamR_Cat fStream;
	};


#endif // __ZStreamR_Cat__
