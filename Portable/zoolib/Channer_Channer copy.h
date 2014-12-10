/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Channer_Channer_h__
#define __ZooLib_Channer_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Channer_Channer

template <class ChanOther_p, class Chan_p>
class Channer_Channer
:	public Channer<typename Chan_p::Chan_Base>
	{
protected:
	Channer_Channer() {}

public:
	virtual ~Channer_Channer() {}

	template <class P>
	Channer_Channer(P& iParam, const ZRef<Channer<ChanOther_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(iParam, sGetChan(iChannerOther))
		{}

	template <class P>
	Channer_Channer(const P& iParam, const ZRef<Channer<ChanOther_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(iParam, sGetChan(iChannerOther))
		{}

	Channer_Channer(const ZRef<Channer<ChanOther_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(sGetChan(iChannerOther))
		{}

// From Channer
	virtual void GetChan(const typename Chan_p::Chan_Base*& oChanPtr)
		{ oChanPtr = &fChan; }

protected:
	const ZRef<Channer<ChanOther_p> > fChannerOther;
	Chan_p fChan;
	};

} // namespace ZooLib

#endif // __ZooLib_Channer_Channer_h__
