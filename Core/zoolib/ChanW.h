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

#include <stdexcept> // For range_error
#include <string> // because range_error may require it

namespace ZooLib {

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
void sEWrite(const ChanW<EE>& iChan, const EE* iSource, size_t iCount)
	{ iCount == sWriteFully<EE>(iChan, iSource, iCount) || sThrow_ExhaustedW(); }

// =================================================================================================
#pragma mark -

/// A write Chan that accepts no data.

template <class EE>
class ChanW_XX_Null
:	public ChanW<EE>
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
:	public ChanW<EE>
	{
public:
	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return iCount; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_h__
