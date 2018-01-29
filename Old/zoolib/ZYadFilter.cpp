/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZYadFilter.h"

namespace ZooLib {

// =================================================================================================
// MARK: -

namespace { // anonymous

class ChanR_NameRefYad_Filtered
:	public ChanR_NameRefYad
	{
public:
	ChanR_NameRefYad_Filtered(
		const ZRef<Callable_YadFilter>& iCallable, const ZRef<YadMapR>& iYadMapR)
	:	fCallable(iCallable)
	,	fYadMapR(iYadMapR)
		{}
	
// From ChanR_NameRefYad
	virtual size_t QRead(NameRefYad* oDest, size_t iCount)
		{
		if (iCount)
			{
			NameRefYad result;
			if (1 == sRead(*fYadMapR, &result, 1))
				{
				oDest->first = result.first;
				oDest->second = sYadFilter(fCallable, sCall(fCallable, result.second));
				return 1;
				}
			}
		return 0;
		}

	const ZRef<Callable_YadFilter> fCallable;
	const ZRef<YadMapR> fYadMapR;
	};

class ChanR_RefYad_Filtered
:	public ChanR_RefYad
	{
public:
	ChanR_RefYad_Filtered(const ZRef<Callable_YadFilter>& iCallable, const ZRef<YadSeqR>& iYadSeqR)
	:	fCallable(iCallable)
	,	fYadSeqR(iYadSeqR)
		{}
	
// From ChanR_RefYad
	virtual size_t QRead(RefYad* oDest, size_t iCount)
		{
		if (iCount && (*oDest++ = sYadFilter(fCallable, sCall(fCallable, sReadInc(fYadSeqR)))))
			return 1;
		return 0;
		}

	const ZRef<Callable_YadFilter> fCallable;
	const ZRef<YadSeqR> fYadSeqR;
	};

} // anonymous namespace

// =================================================================================================
// MARK: -

RefYad sYadFilter(
	const ZRef<Callable_YadFilter>& iCallable_YadFilter, const RefYad& iYadR)
	{
	if (iCallable_YadFilter)
		{
		if (ZRef<YadMapR> theYadMapR = iYadR.DynamicCast<YadMapR>())
			return sChanner_T<ChanR_NameRefYad_Filtered>(iCallable_YadFilter, theYadMapR);

		if (ZRef<YadSeqR> theYadSeqR = iYadR.DynamicCast<YadSeqR>())
			return sChanner_T<ChanR_RefYad_Filtered>(iCallable_YadFilter, theYadSeqR);
		}

	return iYadR;
	}

} // namespace ZooLib
