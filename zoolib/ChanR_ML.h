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

#ifndef __ZooLib_ChanR_ML_h__
#define __ZooLib_ChanR_ML_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

class MLRep
:	public ZCounted
	{};

class ML
	{
public:
	bool IsBegin();
	bool IsEnd();
	bool IsEmpty();
	bool IsTag();
	string8 GetName();
	};

typedef ChanR<ML> ChanR_ML;

// =================================================================================================
// MARK: -

template <class Val, bool Swapped>
ZQ<Val> sQReadSwapped(const ChanR_ML& iChanR);

template <class Val>
ZQ<Val> sQReadSwapped<Val,true>(const ChanR_ML& iChanR)
	{
	Val buf;
	if (sizeof Val != sReadAll(iChanR, &buf, sizeof Val))
		return null;

	sByteSwap<sizeof Val>(&buf);

	return buf;
	}

template <class Val>
ZQ<Val> sQReadSwapped<Val,false>(const ChanR_ML& iChanR)
	{
	Val buf;
	if (sizeof Val != sReadAll(iChanR, &buf, sizeof Val))
		return null;
	return buf;
	}

template <class Val, bool BigEndian=true>
ZQ<Val> sQRead(const ChanR_ML& iChanR)
	{ return sQReadSwapped<Val,BigEndian != (ZCONFIG_Endian == ZCONFIG_Endian_Big)>(iChanR); }

} // namespace ZooLib

#endif // __ZooLib_ChanR_ML_h__
