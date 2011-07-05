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

#include "zoolib/ZCallable_Compound.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Callables (anonymous)

namespace { // anonymous

class Callable_While
:	public ZCallable<void(void)>
	{
public:
	Callable_While(const ZRef<ZCallable_Bool >& iCallable)
	:	fCallable(iCallable)
		{}

// From ZCallable
	virtual void Call()
		{
		while (fCallable->Call())
			{}
		}

private:
	const ZRef<ZCallable_Bool > fCallable;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Makers

ZRef<ZCallable<void(void)> > MakeCallable_While(const ZRef<ZCallable_Bool >& iCallable)
	{ return new Callable_While(iCallable); }

ZRef<ZCallable<void(void)> > While(const ZRef<ZCallable_Bool >& iCallable)
	{ return new Callable_While(iCallable); }

} // namespace ZooLib
