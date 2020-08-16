// Copyright (c) 2014-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_Bin_h__
#define __ZooLib_ChanW_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin

typedef ChanW<byte> ChanW_Bin;

// =================================================================================================
#pragma mark -

inline
size_t sWriteMem(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sWrite<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

inline
size_t sWriteMemFully(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sWriteFully<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

inline
void sEWriteMem(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sEWrite<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

// =================================================================================================
#pragma mark -

template <class T>
bool sQWriteNative(const ChanW_Bin& iChanW, const T& iT)
	{
	if (sizeof(T) != sWriteMemFully(iChanW, &iT, sizeof(T)))
		return false;
	return true;
	}

template <class T>
bool sQWriteSwapped(const ChanW_Bin& iChanW, const T& iT)
	{
	const T buf = sByteSwapped(iT);
	if (sizeof(T) != sWriteMemFully(iChanW, &buf, sizeof(T)))
		return false;
	return true;
	}

#if ZCONFIG_Endian == ZCONFIG_Endian_Big

	template <class T>
	bool sQWriteBE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteNative<T>(iChanW, iT); }

	template <class T>
	bool sQWriteLE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteSwapped<T>(iChanW, iT); }

#else

	template <class T>
	bool sQWriteBE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteSwapped<T>(iChanW, iT); }

	template <class T>
	bool sQWriteLE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteNative<T>(iChanW, iT); }

#endif

template <class T>
void sEWriteNative(const ChanW_Bin& iChanW, const T& iT)
	{ sQWriteNative<T>(iChanW, iT) || sThrow_ExhaustedW(); }

template <class T>
void sEWriteBE(const ChanW_Bin& iChanW, const T& iT)
	{ sQWriteBE<T>(iChanW, iT) || sThrow_ExhaustedW(); }

template <class T>
void sEWriteLE(const ChanW_Bin& iChanW, const T& iT)
	{ sQWriteLE<T>(iChanW, iT) || sThrow_ExhaustedW(); }

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_h__
