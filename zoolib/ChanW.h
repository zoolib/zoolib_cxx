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

#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
// MARK: -

template <class Elmt_p>
class ChanW
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
	typedef Elmt_p Elmt;

	virtual size_t Write(const Elmt* iSource, size_t iCount) = 0;

	virtual void Flush()
		{}
	};

// =================================================================================================
// MARK: -

template <class Elmt_p>
size_t sWrite(const Elmt_p* iSource, size_t iCount, const ChanW<Elmt_p>& iChanW)
	{ return sNonConst(iChanW).Write(iSource, iCount); }

template <class Elmt_p>
void sFlush(const ChanW<Elmt_p>& iChanW)
	{ sNonConst(iChanW).Flush(); }

// =================================================================================================
// MARK: -

template <class Elmt_p>
size_t sWriteFully(const Elmt_p* iSource, size_t iCount, const ChanW<Elmt_p>& iChanW)
	{
	const Elmt_p* localSource = iSource;
	while (iCount)
		{
		if (const size_t countWritten = sWrite(localSource, iCount, iChanW))
			{
			iCount -= countWritten;
			localSource += countWritten;
			}
		else
			{ break; }
		}
	return localSource - iSource;
	}

} // namespace ZooLib

#endif // __ZooLib_ChanW_h__
