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

#ifndef __ZooLib_Compat_algorithm_h__
#define __ZooLib_Compat_algorithm_h__ 1
#include "zconfig.h"

#if !defined(__MWERKS__) && defined(_MSC_VER) && (_MSC_VER < 1200)
	#include <algorithm.h>
#else
	#include <algorithm> // For std::min, std::max and std::swap
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark sMin, sMax and sMinMax

template <class T>
inline
const T sMin(const T& i0, const T& i1)
	{ return std::min<T>(i0, i1); }

template <class T>
inline
const T sMax(const T& i0, const T& i1)
	{ return std::max<T>(i0, i1); }

template <class T>
inline
const T sMinMax(const T& iMin, const T& iVal, const T& iMax)
	{ return iVal < iMin ? iMin : iMax < iVal ? iMax : iVal; }

// =================================================================================================
#pragma mark -
#pragma mark sGetSet

template <class T>
inline
T sGetSet(T& ioLoc, T iVal)
	{
	using std::swap;
	swap(iVal, ioLoc);
	return iVal;
	}

template <class T, class S>
inline
T sGetSet(T& ioLoc, const S& iVal)
	{
	T local = iVal;
	using std::swap;
	swap(ioLoc, local);
	return local;
	}

// =================================================================================================
#pragma mark -
#pragma mark sQSet

template <class T, class S>
inline
bool sQSet(T& ioLoc, const S& iVal)
	{
	if (ioLoc == iVal)
		return false;

	ioLoc = iVal;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark sCAS (Check And Set)

template <class T, class S, class R>
inline
bool sCAS(T& ioLoc, const S& iOldVal, const R& iNewVal)
	{
	if (not (ioLoc == iOldVal))
		return false;

	ioLoc = iNewVal;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark SaveSetRestore

template <class T>
class SaveSetRestore
	{
public:
	SaveSetRestore(T& ioRef)
	:	fRef(ioRef)
	,	fValPrior(ioRef)
		{}

	SaveSetRestore(T& ioRef, const T& iVal)
	:	fRef(ioRef)
	,	fValPrior(iVal)
		{
		using std::swap;
		swap(fRef, fValPrior);
		}

	~SaveSetRestore()
		{
		using std::swap;
		swap(fRef, fValPrior);
		}

	const T& Prior() const
		{ return fValPrior; }

	const T& Current() const
		{ return fRef; }

	T& Current()
		{ return fRef; }

protected:
	T& fRef;
	T fValPrior;
	};

template <class T>
struct SaveRestore : public SaveSetRestore<T>
	{ SaveRestore(T& ioRef) : SaveSetRestore<T>(ioRef) {} };

} // namespace ZooLib

#endif // __ZooLib_Compat_algorithm_h__
