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
// MARK: - ChanW

template <class Elmt_p>
class ChanW
:	public virtual Chan<Elmt_p>
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ChanW reference, you must work with some derived class.
*/	//@{
	ChanW() {}
	virtual ~ChanW() {}
	ChanW(const ChanW&) {}
	ChanW& operator=(const ChanW&) { return *this; }
	//@}

public:
	typedef Elmt_p Elmt_t;
	typedef ChanW<Elmt_p> Chan_Base;

	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount) = 0;

	virtual void Flush()
		{}
	};

// =================================================================================================
// MARK: -

template <class Elmt_p>
size_t sQWrite(const Elmt_p* iSource, size_t iCount, const ChanW<Elmt_p>& iChanW)
	{ return sNonConst(iChanW).QWrite(iSource, iCount); }

template <class Elmt_p>
void sFlush(const ChanW<Elmt_p>& iChanW)
	{ sNonConst(iChanW).Flush(); }

// =================================================================================================
// MARK: -

template <class Elmt_p>
bool sQWrite(const Elmt_p& iElmt, const ChanW<Elmt_p>& iChanW)
	{ return 1 == sNonConst(iChanW).QWrite(&iElmt, 1); }

template <class Elmt_p>
void sWriteMust(const Elmt_p& iElmt, const ChanW<Elmt_p>& iChanW)
	{
	if (1 != sQWrite(&iElmt, 1, iChanW))
		sThrow_ExhaustedW();
	}

template <class Elmt_p>
size_t sQWriteFully(const Elmt_p* iSource, size_t iCount, const ChanW<Elmt_p>& iChanW)
	{
	const Elmt_p* localSource = iSource;
	while (iCount)
		{
		if (const size_t countWritten = sQWrite(localSource, iCount, iChanW))
			{
			iCount -= countWritten;
			localSource += countWritten;
			}
		else
			{ break; }
		}
	return localSource - iSource;
	}

template <class Elmt_p>
void sWriteMust(const Elmt_p* iSource, size_t iCount, const ChanW<Elmt_p>& iChanW)
	{
	if (iCount != sQWriteFully<Elmt_p>(iSource, iCount, iChanW))
		sThrow_ExhaustedW();
	}

// =================================================================================================
// MARK: -

/// A write Chan that accepts no data.

template <class XX>
class ChanW_XX_Null
:	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;
	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount)
		{ return 0; }
	};

// =================================================================================================
// MARK: -

/// A write Chan that accepts and discards all data

template <class XX>
class ChanW_XX_Discard
:	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;
	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount)
		{ return iCount; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_h__
