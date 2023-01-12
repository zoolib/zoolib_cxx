// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRU_XX_Unreader_h__
#define __ZooLib_ChanRU_XX_Unreader_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_XX_Unreader

template <class EE>
class ChanRU_XX_Unreader
:	public virtual ChanRU<EE>
	{
public:
	ChanRU_XX_Unreader(const ChanR<EE>& iChanR)
	:	fChanR(iChanR)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		for (EE* const localDestEnd = oDest + iCount; localDest < localDestEnd; /*no inc*/)
			{
			if (fStack.empty())
				{
				localDest += sRead(fChanR, localDest, localDestEnd - localDest);
				break;
				}
			else
				{
				*localDest++ = fStack.back();
				fStack.pop_back();
				}
			}

		return localDest - oDest;
		}

	virtual size_t Readable()
		{ return fStack.size() + sReadable(fChanR); }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		for (size_t theCount = iCount; theCount; --theCount)
			fStack.push_back(*iSource++);
		return iCount;
		}

protected:
	const ChanR<EE>& fChanR;
	std::vector<EE> fStack;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRU_XX_Unreader_h__
