/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStrimR_Boundary.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimR_Boundary

/**
\class ZStrimR_Boundary
\ingroup strim
We use Boyer-Moore to efficiently search for the boundary sequence in the
source strim. However, instead of discarding data that fails to match it is
returned as the result of calls to Imp_ReadUTF32.
*/

ZStrimR_Boundary::ZStrimR_Boundary(const string8& iBoundary, const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fBoundary(ZUnicode::sAsUTF32(iBoundary))
	{
	this->pInit();
	}

ZStrimR_Boundary::ZStrimR_Boundary
	(const UTF8* iBoundary, size_t iBoundarySize, const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fBoundary(ZUnicode::sAsUTF32(iBoundary, iBoundarySize))
	{
	this->pInit();
	}

ZStrimR_Boundary::ZStrimR_Boundary(const string32& iBoundary, const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fBoundary(iBoundary)
	{
	this->pInit();
	}

ZStrimR_Boundary::ZStrimR_Boundary
	(const UTF32* iBoundary, size_t iBoundarySize, const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fBoundary(iBoundary, iBoundarySize)
	{
	this->pInit();
	}

ZStrimR_Boundary::~ZStrimR_Boundary()
	{
	delete[] fBuffer;
	}

void ZStrimR_Boundary::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (!fBuffer)
		{
		fStrimSource.Read(oDest, iCount, oCount);
		}
	else
		{
		const size_t boundarySize = fBoundary.size();

		UTF32* localDest = oDest;
		size_t countRemaining = iCount;
		while (countRemaining > 0)
			{
			if (fEnd > fStart)
				{
				size_t countToMove = std::min(fEnd - fStart, countRemaining);
				ZMemCopy(localDest, fBuffer + fStart, countToMove * sizeof(UTF32));
				fStart += countToMove;
				localDest += countToMove;
				countRemaining -= countToMove;
				}
			else
				{
				if (fHitBoundary)
					break;

				// Shuffle existing code units to the beginning of the buffer.
				ZMemMove(fBuffer, fBuffer + fEnd, (boundarySize - fEnd) * sizeof(UTF32));

				// And top up the tail.
				size_t countRead;
				fStrimSource.Read(fBuffer + boundarySize - fEnd, fEnd, &countRead);

				if (fEnd > countRead)
					{
					// The source strim has insufficient code units to fill our buffer,
					// so shuffle stuff back up so that the code units that *are* in the
					// buffer align with its end.
					ZMemMove(fBuffer + fEnd - countRead,
						fBuffer, (boundarySize - (fEnd - countRead)) * sizeof(UTF32));

					// The first code unit that can be returned to the caller is
					// at offset fStart, and the last is at the end of the buffer.
					fStart = fEnd - countRead;
					fEnd = boundarySize;
					if (countRead == 0)
						{
						// We read nothing at all, so fStart will equal fEnd,
						// and we should bail.
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
						fStart = boundarySize;
						fEnd = boundarySize;
						}
					else
						{
						// We didn't find the boundary. The skip distance is precisely the number
						// of CUs at the start of the buffer that cannot match the boundary,
						// which are CUs that can be returned as our output.
						fStart = 0;
						fEnd = fSkip[uint8(fBuffer[boundarySize - 1])];
						}
					}
				}
			}
		if (oCount)
			*oCount = localDest - oDest;
		}
	}

/**
This method distinguishes between a strim that's gone empty because the boundary
was encountered and one that's gone empty because the source strim is empty.
*/
bool ZStrimR_Boundary::HitBoundary() const
	{ return fHitBoundary; }

/**
After the boundary has been encountered the strim appears empty. Calling ZStrimR_Boundary::Reset
will allow data to be read again until the next boundary or the real end of strim.
*/
void ZStrimR_Boundary::Reset()
	{
	fHitBoundary = false;
	}

void ZStrimR_Boundary::pInit()
	{
	fBuffer = nullptr;
	try
		{
		if (const size_t boundarySize = fBoundary.size())
			{
			fBuffer = new UTF32[boundarySize];
			fStart = boundarySize;
			fEnd = boundarySize;

			// Initialize the skip vector entries to the boundary size.
			for (size_t x = 0; x < 256; ++x)
				fSkip[x] = boundarySize;

			// For each CP in the search boundary, initialize the appropriate skip
			// to the distance from the last occurence of any CP with the same low
			// 8 bits to the end of the boundary.
			for (size_t x = 0; x < boundarySize - 1; ++x)
				fSkip[uint8(fBoundary[x])] = boundarySize - x - 1;
			}
		else
			{
			fBuffer = nullptr;
			fStart = 0;
			fEnd = 0;
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
// MARK: - ZStrimmerR_Boundary

ZStrimmerR_Boundary::ZStrimmerR_Boundary(const string8& iBoundary, ZRef<ZStrimmerR> iStrimmerSource)
:	fStrimmerSource(iStrimmerSource),
	fStrim(iBoundary, iStrimmerSource->GetStrimR())
	{}

ZStrimmerR_Boundary::ZStrimmerR_Boundary
	(const UTF8* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource)
:	fStrimmerSource(iStrimmerSource),
	fStrim(iBoundary, iBoundarySize, iStrimmerSource->GetStrimR())
	{}

ZStrimmerR_Boundary::ZStrimmerR_Boundary
	(const string32& iBoundary, ZRef<ZStrimmerR> iStrimmerSource)
:	fStrimmerSource(iStrimmerSource),
	fStrim(iBoundary, iStrimmerSource->GetStrimR())
	{}

ZStrimmerR_Boundary::ZStrimmerR_Boundary
	(const UTF32* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource)
:	fStrimmerSource(iStrimmerSource),
	fStrim(iBoundary, iBoundarySize, iStrimmerSource->GetStrimR())
	{}

ZStrimmerR_Boundary::~ZStrimmerR_Boundary()
	{}

const ZStrimR& ZStrimmerR_Boundary::GetStrimR()
	{ return fStrim; }

// =================================================================================================

} // namespace ZooLib
