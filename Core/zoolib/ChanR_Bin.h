// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_Bin_h__
#define __ZooLib_ChanR_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/Chan_Bin.h"
#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

// Analogs of sRead that take void*, so a binary chan can read into any pointer.

inline
size_t sReadMem(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ return sRead(iChan, static_cast<byte*>(oDest), iCount); }

inline
bool sQReadMem(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ return iCount == sReadFully(iChan, static_cast<byte*>(oDest), iCount); }

inline
bool sQReadMem(const ChanR_Bin& iChan, const PaC<void>& iDest)
	{ return sCount(iDest) == sReadFully(iChan, sPtr<byte>(iDest), sCount(iDest)); }

inline
size_t sReadMemFully(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ return sReadFully(iChan, static_cast<byte*>(oDest), iCount); }

inline
void sEReadMem(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ sERead(iChan, static_cast<byte*>(oDest), iCount); }

inline
void sEReadMem(const ChanR_Bin& iChan, const PaC<void>& iDest)
	{ sERead(iChan, sPtr<byte>(iDest), sCount(iDest)); }

inline
void sUnreadMem(const ChanRU_Bin& iChan, const void* iSource, size_t iCount)
	{ sUnread(iChan, static_cast<const byte*>(iSource), iCount); }

// =================================================================================================
#pragma mark -

template <class T>
ZQ<T> sQReadNative(const ChanR_Bin& iChanR)
	{
	T buf;
	if (sizeof(T) != sReadMemFully(iChanR, &buf, sizeof(T)))
		return null;
	return buf;
	}

template <class T>
ZQ<T> sQReadSwapped(const ChanR_Bin& iChanR)
	{
	T buf;
	if (sizeof(T) != sReadMemFully(iChanR, &buf, sizeof(T)))
		return null;
	return sByteSwapped(buf);
	}

#if ZCONFIG_Endian == ZCONFIG_Endian_Big

	template <class T>
	ZQ<T> sQReadBE(const ChanR_Bin& iChanR)
		{ return sQReadNative<T>(iChanR); }

	template <class T>
	ZQ<T> sQReadLE(const ChanR_Bin& iChanR)
		{ return sQReadSwapped<T>(iChanR); }

	template <class T>
	ZQ<T> sQRead(bool iBE, const ChanR_Bin& iChanR)
		{
		if (iBE)
			return sQReadNative<T>(iChanR);
		return sQReadSwapped<T>(iChanR);
		}

#else

	template <class T>
	ZQ<T> sQReadBE(const ChanR_Bin& iChanR)
		{ return sQReadSwapped<T>(iChanR); }

	template <class T>
	ZQ<T> sQReadLE(const ChanR_Bin& iChanR)
		{ return sQReadNative<T>(iChanR); }

	template <class T>
	ZQ<T> sQRead(bool iBE, const ChanR_Bin& iChanR)
		{
		if (iBE)
			return sQReadSwapped<T>(iChanR);
		return sQReadNative<T>(iChanR);
		}

#endif

template <class T>
T sEReadNative(const ChanR_Bin& iChanR)
	{
	if (ZQ<T> theQ = sQReadNative<T>(iChanR))
		return *theQ;
	sThrow_ExhaustedR();
	}

template <class T>
T sEReadSwapped(const ChanR_Bin& iChanR)
	{
	if (ZQ<T> theQ = sQReadSwapped<T>(iChanR))
		return *theQ;
	sThrow_ExhaustedR();
	}

template <class T>
T sEReadBE(const ChanR_Bin& iChanR)
	{
	if (ZQ<T> theQ = sQReadBE<T>(iChanR))
		return *theQ;
	sThrow_ExhaustedR();
	}

template <class T>
T sEReadLE(const ChanR_Bin& iChanR)
	{
	if (ZQ<T> theQ = sQReadLE<T>(iChanR))
		return *theQ;
	sThrow_ExhaustedR();
	}

template <class T>
T sERead(bool iBE, const ChanR_Bin& iChanR)
	{
	if (iBE)
		return sEReadBE<T>(iChanR);
	return sEReadLE<T>(iChanR);
	}

} // namespace ZooLib

#endif // __ZooLib_ChanR_Bin_h__
