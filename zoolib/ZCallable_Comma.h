/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCallable_Comma__
#define __ZCallable_Comma__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCallable_Comma

template <class R0, class R1>
class ZCallable_Comma
:	public ZCallable<R1(void)>
	{
public:
	ZCallable_Comma(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	:	f0(i0)
	,	f1(i1)
		{}

// From ZCallable
	virtual R1 Call()
		{
		f0->Call();
		return f1->Call();
		}

private:
	const ZRef<ZCallable<R0(void)> > f0;
	const ZRef<ZCallable<R1(void)> > f1;
	};

template <class R0, class R1>
ZRef<ZCallable<R1(void)> >
MakeCallable_Comma(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	{ return new ZCallable_Comma<R0,R1>(i0, i1); }

template <class R0, class R1>
ZRef<ZCallable<R1(void)> >
Comma(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	{ return MakeCallable_Comma(i0, i1); }

template <class R0, class R1>
ZRef<ZCallable<R1(void)> >
operator,(const ZRef<ZCallable<R0(void)> >& i0, const ZRef<ZCallable<R1(void)> >& i1)
	{ return MakeCallable_Comma(i0, i1); }

} // namespace ZooLib

#endif // __ZCallable_Comma__
