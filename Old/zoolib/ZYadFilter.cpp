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

class YadMapR_Filtered
:	public YadMapR
	{
public:
	YadMapR_Filtered(
		const ZRef<Callable_YadFilter>& iCallable, const ZRef<YadMapR>& iYadMapR)
	:	fCallable(iCallable)
	,	fYadMapR(iYadMapR)
		{}
	
//	virtual bool IsSimple(const YadOptions& iOptions)
//		{ return fYadMapR->IsSimple(iOptions); }

	virtual ZRef<YadR> ReadInc(Name& oName)
		{ return sYadFilter(fCallable, sCall(fCallable, fYadMapR->ReadInc(oName))); }

	const ZRef<Callable_YadFilter> fCallable;
	const ZRef<YadMapR> fYadMapR;
	};

class YadSeqR_Filtered
:	public YadSeqR
	{
public:
	YadSeqR_Filtered(
		const ZRef<Callable_YadFilter>& iCallable, const ZRef<YadSeqR>& iYadSeqR)
	:	fCallable(iCallable)
	,	fYadSeqR(iYadSeqR)
		{}
	
//	virtual bool IsSimple(const YadOptions& iOptions)
//		{ return fYadSeqR->IsSimple(iOptions); }

	virtual ZRef<YadR> ReadInc()
		{ return sYadFilter(fCallable, sCall(fCallable, fYadSeqR->ReadInc())); }

	const ZRef<Callable_YadFilter> fCallable;
	const ZRef<YadSeqR> fYadSeqR;
	};

} // anonymous namespace

// =================================================================================================
// MARK: -

ZRef<YadR> sYadFilter(
	const ZRef<Callable_YadFilter>& iCallable_YadFilter, const ZRef<YadR>& iYadR)
	{
	if (iCallable_YadFilter)
		{
		if (ZRef<YadMapR> theYadMapR = iYadR.DynamicCast<YadMapR>())
			return new YadMapR_Filtered(iCallable_YadFilter, theYadMapR);

		if (ZRef<YadSeqR> theYadSeqR = iYadR.DynamicCast<YadSeqR>())
			return new YadSeqR_Filtered(iCallable_YadFilter, theYadSeqR);
		}

	return iYadR;
	}

} // namespace ZooLib
