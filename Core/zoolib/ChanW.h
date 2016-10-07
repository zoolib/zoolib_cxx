/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_ChanW_h__
#define __ZooLib_ChanW_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include "zoolib/ZTypes.h" // For sNonConst

#include <stdexcept> // for range_error
#include <string> // because range_error may require it

namespace ZooLib {

inline bool sThrow_ExhaustedW()
	{
	throw std::range_error("ChanW Exhausted");
	return false;
	}

// =================================================================================================
#pragma mark -

template <class EE>
bool sQWrite(const EE& iElmt, const ChanW<EE>& iChan)
	{ return 1 == sNonConst(iChan).QWrite(&iElmt, 1); }

template <class EE>
void sEWrite(const EE& iElmt, const ChanW<EE>& iChan)
	{
	if (1 != sQWrite(&iElmt, 1, iChan))
		sThrow_ExhaustedW();
	}

template <class EE>
size_t sQWriteFully(const EE* iSource, size_t iCount, const ChanW<EE>& iChan)
	{
	const EE* localSource = iSource;
	while (const size_t countWritten = sQWrite(localSource, iCount, iChan))
		{
		iCount -= countWritten;
		localSource += countWritten;
		}
	return localSource - iSource;
	}

template <class EE>
void sEWrite(const EE* iSource, size_t iCount, const ChanW<EE>& iChan)
	{
	if (iCount != sQWriteFully<EE>(iSource, iCount, iChan))
		sThrow_ExhaustedW();
	}

// =================================================================================================
#pragma mark -

/// A write Chan that accepts no data.

template <class EE>
class ChanW_XX_Null
:	public ChanW<EE>
	{
public:
	virtual size_t QWrite(const EE* iSource, size_t iCount)
		{ return 0; }
	};

// =================================================================================================
#pragma mark -

/// A write Chan that accepts and discards all data

template <class EE>
class ChanW_XX_Discard
:	public ChanW<EE>
	{
public:
	virtual size_t QWrite(const EE* iSource, size_t iCount)
		{ return iCount; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_h__
