/* ------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZStreamR_Boundary.h"
#include "ZMemory.h"

using std::min;
using std::max;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Boundary

/**
\class ZStreamR_Boundary
\ingroup stream
We use Boyer-Moore to efficiently search for the boundary sequence in the source stream. However,
instead of discarding data that fails to match it is returned as the result of calls to Imp_Read.
*/

ZStreamR_Boundary::ZStreamR_Boundary(const string& iBoundary, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fBoundary(iBoundary)
	{
	this->Internal_Init();
	}

ZStreamR_Boundary::ZStreamR_Boundary(const void* iBoundary, size_t iBoundarySize,
	const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fBoundary(reinterpret_cast<const char*>(iBoundary), iBoundarySize)
	{
	this->Internal_Init();
	}

ZStreamR_Boundary::~ZStreamR_Boundary()
	{
	delete[] fBuffer;
	}

void ZStreamR_Boundary::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (!fBuffer)
		{
		fStreamSource.Read(iDest, iCount, oCountRead);
		}
	else
		{
		const size_t boundarySize = fBoundary.size();
		uint8* localDest = reinterpret_cast<uint8*>(iDest);
		while (iCount)
			{
			if (fDataEnd > fDataStart)
				{
				size_t countToMove = min(fDataEnd - fDataStart, iCount);
				ZBlockCopy(fBuffer + fDataStart, localDest, countToMove);
				fDataStart += countToMove;
				localDest += countToMove;
				iCount -= countToMove;
				}
			else
				{
				if (fHitBoundary)
					break;

				// Shuffle existing stuff to beginning of buffer.
				ZBlockMove(fBuffer + fDataEnd, fBuffer, boundarySize - fDataEnd);

				// Top up the tail.
				size_t countRead;
				fStreamSource.Read(fBuffer + boundarySize - fDataEnd, fDataEnd, &countRead);

				if (countRead < fDataEnd)
					{
					// The source stream has gone empty without our having already seen the
					// boundary. Shuffle the data we do have so the last byte aligns with
					// the end of the buffer.
					ZBlockMove(fBuffer, fBuffer + fDataEnd - countRead,
						boundarySize - (fDataEnd - countRead));

					// The first returnable byte is at fDataStart, and the last is at
					// the end of the buffer.
					fDataStart = fDataEnd - countRead;
					fDataEnd = boundarySize;
					if (countRead == 0)
						{
						// We read nothing at all, so fDataStart will equal fDataEnd,
						// which will be the end of the buffer. And we should bail.
						break;
						}
					}
				else
					{
					// Do Boyer-Moore search on the full buffer.
					int x;
					for (x = boundarySize - 1; x >= 0 && fBuffer[x] == fBoundary[x]; --x)
						{}

					if (x < 0)
						{
						// We found the boundary.
						fHitBoundary = true;
						fDataStart = boundarySize;
						fDataEnd = boundarySize;
						}
					else
						{
						// We didn't find the boundary. The shift distance is precisely the number
						// of bytes at the start of the buffer that *cannot* match the boundary,
						// which are bytes that can be returned as our output.
						fDataStart = 0;
						fDataEnd = fDistance[fBuffer[boundarySize - 1]];
						}
					}
				}
			}
		if (oCountRead)
			*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
		}
	}

size_t ZStreamR_Boundary::Imp_CountReadable()
	{
	return fDataEnd - fDataStart;
	}

/**
This method distinguishes between a stream that's gone empty because the boundary
was encountered and one that's gone empty because the source stream is empty.
*/
bool ZStreamR_Boundary::HitBoundary() const
	{
	return fHitBoundary;
	}

/**
After the boundary has been encountered the stream appears empty. Calling ZStreamR_Boundary::Reset
will allow data to be read again until the next boundary or the real end of stream.
*/
void ZStreamR_Boundary::Reset()
	{
	fHitBoundary = false;
	}

void ZStreamR_Boundary::Internal_Init()
	{
	fBuffer = nil;
	try
		{
		if (const size_t boundarySize = fBoundary.size())
			{
			fBuffer = new uint8[boundarySize];
			fDataStart = boundarySize;
			fDataEnd = boundarySize;

			// Initialize the skip vector entries to the boundary size
			for (size_t x = 0; x < 256; ++x)
				fDistance[x] = boundarySize;

			// For each byte in the search boundary, initialize the appropriate skip to
			// the distance from the last occurence of that byte to the end of the boundary.
			for (size_t x = 0; x < boundarySize - 1; ++x)
				fDistance[uint8(fBoundary[x])] = boundarySize - x - 1;
			}
		else
			{
			fBuffer = nil;
			fDataStart = 0;
			fDataEnd = 0;
			}
		fHitBoundary = false;
		}
	catch (...)
		{
		delete[] fBuffer;
		throw;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Boundary

ZStreamerR_Boundary::ZStreamerR_Boundary(const string& iBoundary, ZRef<ZStreamerR> iStreamerSource)
:	fStreamerSource(iStreamerSource),
	fStream(iBoundary, iStreamerSource->GetStreamR())
	{}

ZStreamerR_Boundary::ZStreamerR_Boundary(const void* iBoundary, size_t iBoundarySize,
	ZRef<ZStreamerR> iStreamerSource)
:	fStreamerSource(iStreamerSource),
	fStream(iBoundary, iBoundarySize, iStreamerSource->GetStreamR())
	{}

ZStreamerR_Boundary::~ZStreamerR_Boundary()
	{}

const ZStreamR& ZStreamerR_Boundary::GetStreamR()
	{ return fStream; }
