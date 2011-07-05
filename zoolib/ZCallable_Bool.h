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

#ifndef __ZCallable_Bool__
#define __ZCallable_Bool__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Makers

ZRef<ZCallable_Bool> MakeCallable_True();

ZRef<ZCallable_Bool> MakeCallable_False();

ZRef<ZCallable_Bool> MakeCallable_Not(const ZRef<ZCallable_Bool>& iCallable);

ZRef<ZCallable_Bool>
MakeCallable_And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>
MakeCallable_Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>
MakeCallable_Xor(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * More concise Makers

ZRef<ZCallable_Bool> Callable_True();

ZRef<ZCallable_Bool> Callable_False();

ZRef<ZCallable_Bool> Not(const ZRef<ZCallable_Bool>& iCallable);

ZRef<ZCallable_Bool> And(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> Or(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> Xor(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * Infix notation

ZRef<ZCallable_Bool> operator~(const ZRef<ZCallable_Bool>& iCallable);

ZRef<ZCallable_Bool> operator&(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>& operator&=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> operator|(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>& operator|=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool> operator^(const ZRef<ZCallable_Bool>& i0, const ZRef<ZCallable_Bool>& i1);

ZRef<ZCallable_Bool>& operator^=(ZRef<ZCallable_Bool>& io0, const ZRef<ZCallable_Bool>& i1);

// =================================================================================================
#pragma mark -
#pragma mark * sCall

bool sCall(const ZRef<ZCallable_Bool>& iCallable);

} // namespace ZooLib

#endif // __ZCallable_Bool__
