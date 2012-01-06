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

#ifndef __ZUtil_STL_vector_h__
#define __ZUtil_STL_vector_h__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // find
#include "zoolib/ZDebug.h"

#include <vector>

namespace ZooLib {
namespace ZUtil_STL {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_STL

template <class T>
T* sFirstOrNil(std::vector<T>& iVec)
	{ return iVec.empty() ? nullptr : &iVec[0]; }

template <class T>
const T* sFirstOrNil(const std::vector<T>& iVec)
	{ return iVec.empty() ? nullptr : &iVec[0]; }

// ==================================================

/** Returns true if iVector contains iElement. iVector is
assumed to be unordered, so we use find to make the determination. */
template <typename Base, typename Derived>
bool sContains(const std::vector<Base>& iVector, const Derived& iElement)
	{ return iVector.end() != std::find(iVector.begin(), iVector.end(), iElement); }


/** Appends iElement to ioVec by calling push_back. If iElement was already contained in ioVec
then false is returned and no change is made to ioVec. */
template <typename Base, typename Derived>
bool sPushBackIfNotContains(std::vector<Base>& ioVec, const Derived& iElement)
	{
	if (ioVec.end() == std::find(ioVec.begin(), ioVec.end(), iElement))
		return false;
	ioVec.push_back(iElement);
	return true;
	}


/** Appends iElement to ioVec by calling push_back. We first assert, controlled
by iDebugLevel, that iElement is not already present in ioVec. */
template <typename Base, typename Derived>
void sPushBackMustNotContain(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	ZAssertStop(iDebugLevel, ioVec.end() == std::find(ioVec.begin(), ioVec.end(), iElement));
	ioVec.push_back(iElement);
	}

template <typename Base, typename Derived>
void sPushBackMustNotContain(std::vector<Base>& ioVec, const Derived& iElement)
	{ sPushBackMustNotContain(1, ioVec, iElement); }


/** If the unordered vector ioVec contains iElement then it is
removed and true returned. Otherwise no change is made to ioVec and
false is returned. */
template <typename Base, typename Derived>
bool sEraseIfContains(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = std::find(ioVec.begin(), ioVec.end(), iElement);
	if (i == ioVec.end())
		return false;
	ioVec.erase(i);
	return true;
	}


/** Removes iElement from ioVec, asserting that it is present and
returning an iterator referencing the position at which iElement was found. */
template <typename Base, typename Derived>
typename std::vector<Base>::iterator sEraseMustContain(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = std::find(ioVec.begin(), ioVec.end(), iElement);
	ZAssertStop(iDebugLevel, i != ioVec.end());
	return ioVec.erase(i);
	}

template <typename Base, typename Derived>
typename std::vector<Base>::iterator sEraseMustContain
	(std::vector<Base>& ioVec, const Derived& iElement)
	{ return sEraseMustContain(1, ioVec, iElement); }

// ==================================================

/** The contents of iVector are assumed to be sorted by less<Base>. Returns
true if iVector contains iElement. */
template <typename Base, typename Derived>
bool sSortedContains(const std::vector<Base>& iVector, const Derived& iElement)
	{
	typename std::vector<Base>::const_iterator i =
		lower_bound(iVector.begin(), iVector.end(), iElement);
	return i != iVector.end() && *i == iElement;
	}


/** The contents of ioVec are assumed to be sorted by less<Base>. If iElement
was already contained in ioVec then false is returned and no change is made
to ioVec. Otherwise it is inserted using lower_bound and less<Base> and true
is returned. */
template <typename Base, typename Derived>
bool sSortedInsertIfNotContains(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = lower_bound(ioVec.begin(), ioVec.end(), iElement);

	if (i != ioVec.end() && *i == iElement)
		return false;

	ioVec.insert(i, iElement);
	return true;
	}


/** The contents of ioVec are assumed to be sorted by less<Base>. Returns false
if ioVec does not contain iElement. If ioVec does contain
iElement then it is removed and true returned. */
template <typename Base, typename Derived>
bool sSortedEraseIfContains(std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = lower_bound(ioVec.begin(), ioVec.end(), iElement);

	if (i == ioVec.end() || !(*i == iElement))
		return false;

	ioVec.erase(i);
	return true;
	}


/** The contents of ioVec are assumed to be sorted by less<Base>. We first
assert, under the control of iDebugLevel, that ioVec does not contain iElement.
We then insert iElement using lower_bound and less<Base>. */
template <typename Base, typename Derived>
void sSortedInsertMustNotContain(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = lower_bound(ioVec.begin(), ioVec.end(), iElement);
	ZAssertStop(iDebugLevel, i == ioVec.end() || !(*i == iElement));
	ioVec.insert(i, iElement);
	}

template <typename Base, typename Derived>
void sSortedInsertMustNotContain(std::vector<Base>& ioVec, const Derived& iElement)
	{ sSortedInsertMustNotContain(1, ioVec, iElement); } 


/** The contents of ioVec are assumed to be sorted by less<Base>. We first
assert, under the control of iDebugLevel, that ioVec contains iElement.
We then remove iElement using lower_bound and less<Base>. */
template <typename Base, typename Derived>
void sSortedEraseMustContain(const int iDebugLevel,
	std::vector<Base>& ioVec, const Derived& iElement)
	{
	typename std::vector<Base>::iterator i = lower_bound(ioVec.begin(), ioVec.end(), iElement);
	ZAssertStop(iDebugLevel, i != ioVec.end() && *i == iElement);
	ioVec.erase(i);
	}

template <typename Base, typename Derived>
void sSortedEraseMustContain(std::vector<Base>& ioVec, const Derived& iElement)
	{ return sSortedEraseMustContain(1, ioVec, iElement); }

} // namespace ZUtil_STL
} // namespace ZooLib

#endif // __ZUtil_STL_vector_h__
