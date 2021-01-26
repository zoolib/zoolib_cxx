// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRPos_XX_ChanR_h__
#define __ZooLib_ChanRPos_XX_ChanR_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRPos_XX_ChanR

template <class EE>
class ChanRPos_XX_ChanR
:	public ChanRPos<EE>
	{
public:
	ChanRPos_XX_ChanR(const ChanR<EE>& iSource, const ChanRWPos<EE>& iBuffer)
	:	fSource(iSource)
	,	fBuffer(iBuffer)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		while (iCount)
			{
			if (size_t countRead = sReadFully(fBuffer, localDest, iCount))
				{
				localDest += countRead;
				iCount -= countRead;
				}
			else
				{
				// Our buffer is empty.
// 				if (fCommitted)
// 					{
// 					// We're committed, so read direct from fSource.
// 					fSource.Read(localDest, iCount, &countRead);
// 					if (countRead == 0)
// 						break;
// 					localDest += countRead;
// 					iCount -= countRead;
// 					}
// 				else
					{
					// Try topping up our buffer.
					std::pair<uint64,uint64> result = sCopyFully(fSource, fBuffer, iCount);
					if (result.second == 0)
						break;
					// if (result.second < result.first) ???
					sPosSet(fBuffer, sPos(fBuffer) - result.second);
					}
				}
			}
		return localDest - oDest;
		}

	virtual size_t Readable()
		{
		if (size_t theReadable = sReadable(fBuffer))
			return theReadable;
		return sReadable(fSource);
		}

// From ChanPos
	virtual uint64 Pos()
		{
		// ZAssertStop(2, not fCommitted);
		return sPos(fBuffer);
		}

	virtual void PosSet(uint64 iPos)
		{
		// ZAssertStop(2, not fCommitted);
		uint64 curSize = sSize(fBuffer);
		if (iPos > curSize)
			{
			// Position the buffer at its end
			sPosSet(fBuffer, curSize);

			// And suck in enough data from fSource so its size (and position) is iPosition bytes
			sECopyFully(fSource, fBuffer, iPos - curSize);
			}
		sPosSet(fBuffer, iPos);
		}

// From ChanSize
	virtual uint64 Size()
		{
		// In order to know how much data is available we have
		// to suck it all in from fSource. Another reason to avoid
		// using GetSize if at all possible.
		// ZAssertStop(2, not fCommitted);

		uint64 curPosition = sPos(fBuffer);
		sPosSet(fBuffer, sSize(fBuffer));
		sECopyAll(fSource, fBuffer);
		sPosSet(fBuffer, curPosition);
		return sSize(fBuffer);
		}

	// From ChanAspect_Unread<EE>
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		return sUnread(fBuffer, iSource, iCount);
		}

protected:
	const ChanR<EE>& fSource;
	const ChanRWPos<EE>& fBuffer;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRPos_XX_ChanR_h__
