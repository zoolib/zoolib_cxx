// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRPos_XX_ChanR_h__
#define __ZooLib_ChanRPos_XX_ChanR_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/Util_Chan.h" // For sECopyAll, sCopyFully.

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRPos_XX_ChanR

template <class EE>
class ChanRPos_XX_ChanR
:	public virtual ChanRPos<EE>
	{
public:
	ChanRPos_XX_ChanR(const ChanR<EE>& iSource, const ChanRWPos<EE>& iBuffer)
	:	fSource(iSource)
	,	fBuffer(iBuffer)
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
		const uint64 theBufferPos = sPos(fBuffer);
		const uint64 theBufferSize = sSize(fBuffer);
		if (theBufferPos >= theBufferSize)
			{
			sPosSet(fBuffer, theBufferSize);
			std::pair<uint64,uint64> result =
				sCopyFully(fSource, fBuffer, theBufferPos - theBufferSize + iCount);
			sPosSet(fBuffer, theBufferPos);

			if (result.first != result.second)
				sThrow_ExhaustedW();
			}

		return sRead(fBuffer, oDest, iCount);
		}

	virtual size_t Readable()
		{
		if (size_t theReadable = sReadable(fBuffer))
			return theReadable;
		return sReadable(fSource);
		}

// From ChanSize
	virtual uint64 Size()
		{
		// In order to know how much data is available we have
		// to suck it all in from fSource. Another reason to avoid
		// using Size if at all possible.

		uint64 curPosition = sPos(fBuffer);
		sPosSet(fBuffer, sSize(fBuffer));
		sECopyAll(fSource, fBuffer);
		sPosSet(fBuffer, curPosition);
		return sSize(fBuffer);
		}

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{ return sUnread(fBuffer, iSource, iCount); }

protected:
	const ChanR<EE>& fSource;
	const ChanRWPos<EE>& fBuffer;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRPos_XX_ChanR_h__
