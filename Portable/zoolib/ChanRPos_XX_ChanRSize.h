// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRPos_XX_ChanRSize_h__
#define __ZooLib_ChanRPos_XX_ChanRSize_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/Util_Chan.h" // For sCopyFully.

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRPos_XX_ChanR

template <class EE>
class ChanRPos_XX_ChanRSize
:	public ChanRPos<EE>
	{
public:
	ChanRPos_XX_ChanRSize(const ChanRSize<EE>& iSource, const ChanRWPos<EE>& iBuffer)
	:	fSource(iSource)
	,	fBuffer(iBuffer)
	,	fOffset(sSize(iBuffer))
		{}

// From ChanPos
	virtual uint64 Pos()
		{ return sPos(fBuffer); }

// From ChanPosSet
	virtual void PosSet(uint64 iPos)
		{ sPosSet(fBuffer, iPos); }

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const uint64 thePotentialSize = sSize(fSource) + fOffset;
		const uint64 theBufferPos = sPos(fBuffer);
		if (thePotentialSize <= theBufferPos)
			return 0;

		const uint64 theBufferSize = sSize(fBuffer);
		if (theBufferPos >= theBufferSize)
			{
			sPosSet(fBuffer, theBufferSize);
			std::pair<uint64,uint64> result =
				sCopyFully(fSource, fBuffer,theBufferPos - theBufferSize + iCount);
			sPosSet(fBuffer, theBufferPos);

			// We absolutely depend on having copied all data into fBuffer.
			if (result.first != result.second)
				sThrow_ExhaustedW();
			}

		return sRead(fBuffer, oDest, iCount);
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const uint64 thePotentialSize = sSize(fSource) + fOffset;
		const uint64 theBufferPos = sPos(fBuffer);
		if (thePotentialSize <= theBufferPos)
			return 0;
		const uint64 countSkipped = std::min<uint64>(thePotentialSize - theBufferPos, iCount);
		sPosSet(fBuffer, theBufferPos + countSkipped);
		return countSkipped;
		}

	virtual size_t Readable()
		{
		uint64 thePotentialSize = sSize(fSource) + fOffset;
		uint64 theBufferPos = sPos(fBuffer);
		if (thePotentialSize <= theBufferPos)
			return 0;
		return sClamped(thePotentialSize - theBufferPos);
		}

// From ChanSize
	virtual uint64 Size()
		{ return sSize(fSource) + fOffset; }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{ return sUnread(fBuffer, iSource, iCount); }

protected:
	const ChanRSize<EE>& fSource;
	const ChanRWPos<EE>& fBuffer;
	const uint64 fOffset;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRPos_XX_ChanRSize_h__
