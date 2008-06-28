/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStreamR_Boundary__
#define __ZStreamR_Boundary__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

#include <string>

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Boundary

/// A read filter stream that reads from another stream until a boundary sequence is encountered.

class ZStreamR_Boundary : public ZStreamR
	{
public:
	ZStreamR_Boundary(const std::string& iBoundary, const ZStreamR& iStreamSource);
	ZStreamR_Boundary(const void* iBoundary, size_t iBoundarySize, const ZStreamR& iStreamSource);
	~ZStreamR_Boundary();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// Our protocol
	bool HitBoundary() const;
	void Reset();

protected:
	void Internal_Init();

	const ZStreamR& fStreamSource;
	std::string fBoundary;
	size_t fDistance[256];
	uint8* fBuffer;
	size_t fDataStart;
	size_t fDataEnd;
	bool fHitBoundary;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Boundary

/// A read filter streamer encapsulating a ZStreamR_Boundary.

class ZStreamerR_Boundary : public ZStreamerR
	{
public:
	ZStreamerR_Boundary(const std::string& iBoundary, ZRef<ZStreamerR> iStreamerSource);

	ZStreamerR_Boundary(const void* iBoundary, size_t iBoundarySize,
		ZRef<ZStreamerR> iStreamerSource);

	virtual ~ZStreamerR_Boundary();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZRef<ZStreamerR> fStreamerSource;
	ZStreamR_Boundary fStream;
	};

// Alternate template-based definition:
// typedef ZStreamerRFT<ZStreamR_Boundary> ZStreamerR_Boundary;

// =================================================================================================

#endif // __ZStreamR_Boundary__
