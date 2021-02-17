// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Channer_XX_Wrapper_h__
#define __ZooLib_Channer_XX_Wrapper_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChannerRW_Wrapper

template <class EE>
class ChannerRW_Wrapper
:	public ChannerRW<EE>
	{
public:
	ChannerRW_Wrapper(
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
ZP<ChannerRW<EE>> sChanner_Wrapper(
	const ZP<Channer<ChanR<EE>>>& iChannerR,
	const ZP<Channer<ChanW<EE>>>& iChannerW)
	{ return new ChannerRW_Wrapper<EE>(iChannerR, iChannerW); }

// =================================================================================================
#pragma mark - ChannerRWClose_Wrapper

template <class EE>
class ChannerRWClose_Wrapper
:	public ChannerConnection<EE>
	{
public:
	ChannerRWClose_Wrapper(
		const ZP<Channer<ChanR<EE>>>& iChannerR,
		const ZP<Channer<ChanW<EE>>>& iChannerW,
		const ZP<ChannerClose>& iChannerClose)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
	,	fChannerClose(iChannerClose)
		{}

// From ChanAspect_Abort
	virtual void Abort()
		{ return sAbort(*fChannerClose); }

// From ChanAspect_DisconnectRead
	virtual bool DisconnectRead(double iTimeout)
		{ return sDisconnectRead(*fChannerClose, iTimeout); }

// From ChanAspect_DisconnectWrite
	virtual void DisconnectWrite()
		{ return sDisconnectWrite(*fChannerClose); }

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
	const ZP<ChannerClose> fChannerClose;
	};

template <class EE>
ZP<ChannerRWClose<EE>> sChanner_Wrapper(
	const ZP<Channer<ChanR<EE>>>& iChannerR,
	const ZP<Channer<ChanW<EE>>>& iChannerW,
	const ZP<ChannerClose>& iChannerClose)
	{ return new ChannerRWClose_Wrapper<EE>(iChannerR, iChannerW, iChannerClose); }

} // namespace ZooLib

#endif // __ZooLib_Channer_XX_Wrapper_h__
