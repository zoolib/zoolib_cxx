/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZSetRestore_T_h__
#define __ZSetRestore_T_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // For std::swap

namespace ZooLib {

// =================================================================================================
// MARK: - ZSetRestore_T

template <class T>
class ZSetRestore_T
	{
public:
	ZSetRestore_T(T& ioRef)
	:	fRef(ioRef)
		{ std::swap(fRef, fValPrior); }

	ZSetRestore_T(T& ioRef, const T& iVal)
	:	fRef(ioRef)
	,	fValPrior(iVal)
		{ std::swap(fRef, fValPrior); }

	~ZSetRestore_T()
		{ std::swap(fRef, fValPrior); }

	const T& GetPrior() const
		{ return fValPrior; }

private:
	T& fRef;
	T fValPrior;
	};

} // namespace ZooLib

#endif // __ZSetRestore_T_h__
