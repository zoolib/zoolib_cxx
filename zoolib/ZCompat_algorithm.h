/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCompat_algorithm_h__
#define __ZCompat_algorithm_h__
#include "zconfig.h"

#if !defined(__MWERKS__) && defined(_MSC_VER) && (_MSC_VER < 1200)
	#include <algorithm.h>
#else
	#include <algorithm>
#endif

namespace ZooLib {

template <class Min_p, class T, class Max_p>
const T sMinMax(const Min_p& iMin, const T& iVal, const Max_p& iMax)
	{ return std::min(std::max(T(iMin), iVal), T(iMax)); }	

template <class T, class Other_p>
const T sMin(const T& iVal, const Other_p& iOther)
	{ return std::min(iVal, T(iOther)); }

template <class T, class Other_p>
const T sMax(const T& iVal, const Other_p& iOther)
	{ return std::max(iVal, T(iOther)); }

template <class T>
inline T sGetSet(T& ioLoc, T iVal)
	{
	std::swap(iVal, ioLoc);
	return iVal;
	}

template <class S, class T>
inline S sGetSet(S& ioLoc, T iVal)
	{
	std::swap(iVal, ioLoc);
	return iVal;
	}

// =================================================================================================
// MARK: - ZSaveRestore_T

template <class T>
class ZSaveRestore_T
	{
public:
	ZSaveRestore_T(T& ioRef)
	:	fRef(ioRef)
	,	fValPrior(ioRef)
		{}

	~ZSaveRestore_T()
		{ std::swap(fRef, fValPrior); }

	const T& GetPrior() const
		{ return fValPrior; }

private:
	T& fRef;
	T fValPrior;
	};

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

#endif // __ZCompat_algorithm_h__
