// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanW_h__
#define __ZooLib_ChanW_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include <stdexcept> // For range_error
#include <string> // because range_error may require it

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZMACRO_NoReturn_Pre
inline bool sThrow_ExhaustedW();
ZMACRO_NoReturn_Post

inline bool sThrow_ExhaustedW()
	{ throw std::range_error("ChanW Exhausted"); }

// =================================================================================================
#pragma mark -

template <class EE>
bool sQWrite(const ChanW<EE>& iChan, const EE& iElmt)
	{ return 1 == sNonConst(iChan).Write(&iElmt, 1); }

template <class EE>
void sEWrite(const ChanW<EE>& iChan, const EE& iElmt)
	{ 1 == sNonConst(iChan).Write(&iElmt, 1) || sThrow_ExhaustedW(); }

template <class EE>
size_t sWriteFully(const ChanW<EE>& iChan, const EE* iSource, size_t iCount)
	{
	const EE* localSource = iSource;
	while (const size_t countWritten = sWrite(iChan, localSource, iCount))
		{
		iCount -= countWritten;
		localSource += countWritten;
		}
	return localSource - iSource;
	}

template <class EE>
size_t sWriteFully(const ChanW<EE>& iChan, const PaC<const EE>& iSource)
	{ return sWriteFully(iChan, sPtr(iSource), sCount(iSource)); }

template <class EE>
void sEWrite(const ChanW<EE>& iChan, const EE* iSource, size_t iCount)
	{ iCount == sWriteFully<EE>(iChan, iSource, iCount) || sThrow_ExhaustedW(); }

template <class EE>
void sEWrite(const ChanW<EE>& iChan, const PaC<const EE>& iSource)
	{ sEWrite(iChan, sPtr(iSource), sCount(iSource)); }

// =================================================================================================
#pragma mark -

/// A write Chan that accepts no data.

template <class EE>
class ChanW_XX_Null
:	public virtual ChanW<EE>
	{
public:
	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return 0; }
	};

// =================================================================================================
#pragma mark -

/// A write Chan that accepts and discards all data

template <class EE>
class ChanW_XX_Discard
:	public virtual ChanW<EE>
	{
public:
	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return iCount; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_h__
