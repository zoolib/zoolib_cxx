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

#ifndef __ZooLib_ChanU_h__
#define __ZooLib_ChanU_h__ 1
#include "zconfig.h"

#include "zoolib/size_t.h" // For size_t

namespace ZooLib {

// =================================================================================================
// MARK: - ChanU

template <class Elmt_p>
class ChanU
	{
protected:
	ChanU() {}
	virtual ~ChanU() {}
	ChanU(const ChanU&) {}
	ChanU& operator=(const ChanU&) { return *this; }

public:
	typedef Elmt_p Elmt_t;

	virtual size_t Unread(const Elmt_t* iSource, size_t iCount) = 0;

	virtual size_t UnreadableLimit() // ??
		{ return 0; }
	};

// =================================================================================================
// MARK: -

template <class Elmt_p>
size_t sUnread(const Elmt_p* iSource, size_t iCount, const ChanU<Elmt_p>& iChanU)
	{ return sNonConst(iChanU).Unread(iSource, iCount); }

template <class Elmt_p>
bool sUnread(const Elmt_p& iElmt, const ChanU<Elmt_p>& iChanU)
	{ return 1 == sUnread(&iElmt, 1, iChanU); }

template <class Elmt_p>
size_t sUnreadableLimit(const ChanU<Elmt_p>& iChanU)
	{ return sNonConst(iChanU).UnreadableLimit(); }

// =================================================================================================
// MARK: -

template <class Elmt_p>
bool sQUnread(const Elmt_p& iElmt, const ChanU<Elmt_p>& iChanU)
	{ return 1 == sNonConst(iChanU).Unread(&iElmt, 1); }

} // namespace ZooLib

#endif // __ZooLib_ChanU_h__
