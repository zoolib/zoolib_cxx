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

#include "zoolib/CtorDtor.h"

#include "zoolib/ZName.h"

namespace ZooLib {

// These are in a separate file so they're part of a separate compilation unit from
// the main ZName code. When optimizing the compiler will likely inline these, and
// unnecessarily taint their callers with Unwind_SjLj_Register/Unwind_SjLj_Unregister calls,
// which are a noticeable cost on iOS.

void ZName::spRetain(const CountedString* iCounted)
	{
	try { const_cast<CountedString*>(iCounted)->Retain(); }
	catch (...) {}
	}

void ZName::spRelease(const CountedString* iCounted)
	{
	try { const_cast<CountedString*>(iCounted)->Release(); }
	catch (...) {}
	}

} // namespace ZooLib
