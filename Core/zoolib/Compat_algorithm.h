// Copyright (c) 2000-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2021 Andrew Green.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compat_algorithm_h__
#define __ZooLib_Compat_algorithm_h__ 1
#include "zconfig.h"

#include <algorithm> // For std::min, std::max and std::swap

namespace ZooLib {

// =================================================================================================
#pragma mark - sMin, sMax and sMinMax

template <class T>
inline
const T sMin(const T& ll, const T& rr)
	{ return std::min<T>(ll, rr); }

template <class T>
inline
const T sMax(const T& ll, const T& rr)
	{ return std::max<T>(ll, rr); }

template <class T>
inline
const T sMinMax(const T& iMin, const T& iVal, const T& iMax)
	{ return iVal < iMin ? iMin : iMax < iVal ? iMax : iVal; }

template <class T>
inline
const T sMedian(const T& aa, const T& bb, const T& cc)
	{ return std::max(std::min(aa,bb), std::min(std::max(aa,bb), cc)); }

// =================================================================================================
#pragma mark - sGetSet

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
#pragma mark - sQSet

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
#pragma mark - sCAS (Check And Set)

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
#pragma mark - SaveSetRestore

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
