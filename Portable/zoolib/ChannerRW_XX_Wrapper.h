// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChannerRW_XX_Wrapper_h__
#define __ZooLib_ChannerRW_XX_Wrapper_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChannerRW_XX_Wrapper

template <class EE>
class ChannerRW_XX_Wrapper
:	public ChannerRW<EE>
	{
public:
	ChannerRW_XX_Wrapper(
		const ZP<Channer<ChanR<EE>>>& iChannerR,
		const ZP<Channer<ChanW<EE>>>& iChannerW)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
		{}

// From ChanAspect_Read
	virtual size_t Read(byte* oDest, size_t iCount)
		{ return sRead(*fChannerR, oDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return sSkip(*fChannerR, iCount); }

// From ChanAspect_Write
	virtual size_t Write(const byte* iSource, size_t iCount)
		{ return sWrite(*fChannerW, iSource, iCount); }

	virtual void Flush()
		{ sFlush(*fChannerW); }

private:
	const ZP<Channer<ChanR<EE>>> fChannerR;
	const ZP<Channer<ChanW<EE>>> fChannerW;
	};

template <class EE>
ZP<ChannerRW<EE>> sChannerRW_XX_Wrapper(
	const ZP<Channer<ChanR<EE>>>& iChannerR,
	const ZP<Channer<ChanW<EE>>>& iChannerW)
	{
	if (iChannerR && iChannerW)
		return new ChannerRW_XX_Wrapper<EE>(iChannerR, iChannerW);
	return null;
	}

} // namespace ZooLib

#endif // __ZooLib_ChannerRW_XX_Wrapper_h__
