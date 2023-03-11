// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_XX_Boundary_h__
#define __ZooLib_ChanR_XX_Boundary_h__ 1
#include "zconfig.h"

#include <vector>

#include "zoolib/Compat_algorithm.h" // For std::copy_n
#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Boundary

template <class EE>
class ChanR_XX_Boundary
:	public virtual ChanR<EE>
	{
public:
	ChanR_XX_Boundary(const EE* iBoundary, size_t iBoundaryCount,
		const ChanR<EE>& iChanR)
	:	fChanR(iChanR)
	,	fBoundary(iBoundary, iBoundary + iBoundaryCount)
	,	fBuffer(fBoundary.size())
		{
		this->pInit();
		}

	template <class Iterator>
	ChanR_XX_Boundary(const Iterator& iBegin, const Iterator& iEnd,
		const ChanR<EE>& iChanR)
	:	fChanR(iChanR)
	,	fBoundary(iBegin, iEnd)
	,	fBuffer(fBoundary.size())
		{
		this->pInit();
		}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t boundaryCount = fBoundary.size();
		if (not boundaryCount)
			{
			return sRead(fChanR, oDest, iCount);
			}
		else
			{
			EE* localDest = oDest;
			while (iCount)
				{
				if (fEnd > fBegin)
					{
					const size_t countToCopy = std::min(fEnd - fBegin, iCount);
					std::copy_n(&fBuffer[fBegin], countToCopy, localDest);
					fBegin += countToCopy;
					localDest += countToCopy;
					iCount -= countToCopy;
					}
				else if (fHitBoundary)
					{
					break;
					}
				else
					{
					// Shuffle existing stuff to beginning of buffer (could be handled in
					// a more efficient manner.)
					std::copy_n(&fBuffer[fEnd], boundaryCount - fEnd, &fBuffer[0]);

					// Top up the tail.
					const size_t countRead = sRead(fChanR, &fBuffer[boundaryCount - fEnd], fEnd);

					if (fEnd > countRead)
						{
						// The source stream has gone empty without our having already seen the
						// boundary. Shuffle the data we do have so the last byte aligns with
						// the end of the buffer.
						std::copy_n(&fBuffer[0],
							boundaryCount - (fEnd - countRead), &fBuffer[fEnd - countRead]);

						// The first returnable byte is at fBegin, and the last is at
						// the end of the buffer.
						fBegin = fEnd - countRead;
						fEnd = boundaryCount;
						if (countRead == 0)
							{
							// We read nothing at all, so fBegin will equal fEnd,
							// which will be the end of the buffer. And we should bail.
							break;
							}
						}
					else
						{
						// Do Boyer-Moore search on the full buffer.
						int xx;
						for (xx = int(boundaryCount) - 1;
							xx >= 0 && fBuffer[xx] == fBoundary[xx];
							--xx)
							{}

						if (xx < 0)
							{
							// We found the boundary.
							fHitBoundary = true;
							fBegin = boundaryCount;
							fEnd = boundaryCount;
							}
						else
							{
							// We didn't find the boundary. The shift distance is precisely the number
							// of 'bytes' at the start of the buffer that *cannot* match the boundary,
							// which are 'bytes' that can be returned as our output.
							fBegin = 0;
							fEnd = fDistance[uint8(fBuffer[boundaryCount - 1])];
							}
						}
					}
				}
			return localDest - oDest;
			}
		}

	virtual size_t Readable()
		{
		return fEnd - fBegin;
		}

// Our protocol
	bool HitBoundary()
		{ return fHitBoundary; }

	void Reset()
		{ fHitBoundary = false; }

protected:
	void pInit()
		{
		fHitBoundary = false;
		const size_t boundaryCount = fBoundary.size();
		fBegin = boundaryCount;
		fEnd = boundaryCount;

		// Initialize the skip vector entries to the boundary count
		for (size_t xx = 0; xx < 256; ++xx)
			fDistance[xx] = boundaryCount;

		// For each 'byte' in the search boundary, initialize the appropriate skip to
		// the distance from the last occurence of that byte to the end of the boundary.
		for (size_t xx = 0; xx < boundaryCount - 1; ++xx) //?? why boundaryCount - 1 ??
			fDistance[uint8(fBoundary[xx])] = boundaryCount - xx - 1;
		}

	const ChanR<EE>& fChanR;
	const std::vector<EE> fBoundary;

	std::vector<EE> fBuffer;
	size_t fDistance[256];
	size_t fBegin;
	size_t fEnd;
	bool fHitBoundary;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Boundary_h__
