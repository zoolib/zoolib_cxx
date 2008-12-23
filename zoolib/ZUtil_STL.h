/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_STL__
#define __ZUtil_STL__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"

#include <map>
#include <set>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_STL {

// ==================================================

/** Invoke delete on all elements between begin and end. */
template <class InputIterator>
void sDeleteAll(InputIterator begin, InputIterator end)
	{
	while (begin != end)
		{
		delete *begin;
		++begin;
		}
	}

template <class T>
T* sFirstOrNil(std::vector<T> iVec)
	{
	return iVec.empty() ? nil : &iVec[0];
	}

// ==================================================

/** Returns true if iVector contains iElement. iVector is
assumed to be unordered, so we use find to make the determination. */
template <typename T, typename S>
bool sContains(const std::vector<T>& iVector, S iElement)
	{ return iVector.end() != find(iVector.begin(), iVector.end(), iElement); }


/** Returns true if iSet contains iElement. */
template <typename T, typename S>
bool sContains(const std::set<T>& iSet, S iElement)
	{ return iSet.end() != iSet.find(iElement); }


/** Returns true if iMap has an element at iKey. */
template <typename K, typename V>
bool sContains(const std::map<K, V>& iMap, K iKey)
	{ return iMap.end() != iMap.find(iKey); }


/** Appends iElement to ioVector by calling push_back. If iElement
was already contained in ioVector then false is returned and no change is made
to ioVector. */
template <typename T>
bool sPushBackIfNotContains(std::vector<T>& ioVector, T iElement)
	{
	typename std::vector<T>::iterator i = find(ioVector.begin(), ioVector.end(), iElement);
	if (i != ioVector.end())
		return false;
	ioVector.push_back(iElement);
	return true;
	}


/** Appends iElement to ioVector by calling push_back. We first assert, controlled
by iDebugLevel, that iElement is not already present in ioVector. */
template <typename T>
void sPushBackMustNotContain(const int iDebugLevel, std::vector<T>& ioVector, T iElement)
	{
	ZAssertStop(iDebugLevel, ioVector.end() == find(ioVector.begin(), ioVector.end(), iElement));
	ioVector.push_back(iElement);
	}


/** Inserts iElement to ioSet. We first assert, controlled
by iDebugLevel, that iElement is not already present in ioSet. */
template <typename T>
void sInsertMustNotContain(const int iDebugLevel, std::set<T>& ioSet, T iElement)
	{
	ZAssertStop(iDebugLevel, ioSet.end() == ioSet.find(iElement));
	ioSet.insert(iElement);
	}


/** Inserts iElement in ioSet, if it's not already contained. */
template <typename T>
bool sInsertIfNotContains(std::set<T>& ioSet, T iElement)
	{
	typename std::set<T>::iterator i = ioSet.lower_bound(iElement);
	if (ioSet.end() == i || *i != iElement)
		{
		ioSet.insert(i, iElement);
		return true;
		}
	return false;
	}


/** If the unordered vector ioVector contains iElement then it is
removed and true returned. Otherwise no change is made to ioVector and
false is returned. */
template <typename T>
bool sEraseIfContains(std::vector<T>& ioVector, T iElement)
	{
	typename std::vector<T>::iterator i = find(ioVector.begin(), ioVector.end(), iElement);
	if (i == ioVector.end())
		return false;
	ioVector.erase(i);
	return true;
	}


/** If  ioSet contains iElement then it is removed and true returned.
Otherwise no change is made to ioSet and false is returned. */
template <typename T>
bool sEraseIfContains(std::set<T>& ioSet, T iElement)
	{
	typename std::set<T>::iterator i = ioSet.find(iElement);
	if (i != ioSet.end())
		{
		ioSet.erase(i);
		return true;
		}
	return false;
	}


/** Removes iElement from ioVector, asserting that it is present and
returning an iterator referencing the position at which iElement was found. */
template <typename T>
typename std::vector<T>::iterator sEraseMustContain(
	const int iDebugLevel, std::vector<T>& ioVector, T iElement)
	{
	typename std::vector<T>::iterator i = find(ioVector.begin(), ioVector.end(), iElement);
	ZAssertStop(iDebugLevel, i != ioVector.end());
	return ioVector.erase(i);
	}


/** Removes iElement from ioSet, asserting that it is present. */
template <typename T>
void sEraseMustContain(const int iDebugLevel, std::set<T>& ioSet, T iElement)
	{
	typename std::set<T>::iterator i = ioSet.find(iElement);
	ZAssertStop(iDebugLevel, i != ioSet.end());
	ioSet.erase(i);
	}

// ==================================================

/** The contents of iVector are assumed to be sorted by less<T>. Returns
true if iVector contains iElement. */
template <typename T>
bool sSortedContains(const std::vector<T>& iVector, T iElement)
	{
	typename std::vector<T>::const_iterator i
		= lower_bound(iVector.begin(), iVector.end(), iElement);
	return i != iVector.end() && *i == iElement;
	}


/** The contents of iVector are assumed to be sorted by less<T>. If iElement
was already contained in iVector then false is returned and no change is made
to iVector. Otherwise it is inserted using lower_bound and less<T> and true
is returned. */
template <typename T>
bool sSortedInsertIfNotContains(std::vector<T>& iVector, T iElement)
	{
	typename std::vector<T>::iterator i = lower_bound(iVector.begin(), iVector.end(), iElement);
	if (i != iVector.end() && *i == iElement)
		return false;
	iVector.insert(i, iElement);
	return true;
	}


/** The contents of iVector are assumed to be sorted by less<T>. Returns false
if iVector does not contain iElement. If iVector does contain
iElement then it is removed and true returned. */
template <typename T>
bool sSortedEraseIfContains(std::vector<T>& iVector, T iElement)
	{
	typename std::vector<T>::iterator i = lower_bound(iVector.begin(), iVector.end(), iElement);
	if (i == iVector.end() || *i != iElement)
		return false;
	iVector.erase(i);
	return true;
	}


/** The contents of iVector are assumed to be sorted by less<T>. We first
assert, under the control of iDebugLevel, that iVector does not contain iElement.
We then insert iElement using lower_bound and less<T>. */
template <typename T>
void sSortedInsertMustNotContain(const int iDebugLevel, std::vector<T>& iVector, T iElement)
	{
	typename std::vector<T>::iterator i = lower_bound(iVector.begin(), iVector.end(), iElement);
	ZAssertStop(iDebugLevel, i == iVector.end() || *i != iElement);
	iVector.insert(i, iElement);
	}


/** The contents of iVector are assumed to be sorted by less<T>. We first
assert, under the control of iDebugLevel, that iVector contains iElement.
We then remove iElement using lower_bound and less<T>. */
template <typename T>
void sSortedEraseMustContain(const int iDebugLevel, std::vector<T>& iVector, T iElement)
	{
	typename std::vector<T>::iterator i = lower_bound(iVector.begin(), iVector.end(), iElement);
	ZAssertStop(iDebugLevel, i != iVector.end() && *i == iElement);
	iVector.erase(i);
	}

// ==================================================

template <typename K, typename V>
bool sEraseIfContains(std::map<K, V>& ioMap, K iKey)
	{
	typename std::map<K, V>::iterator i = ioMap.find(iKey);
	if (i != ioMap.end())
		{
		ioMap.erase(i);
		return true;
		}
	return false;
	}


template <typename K, typename V>
void sEraseMustContain(const int iDebugLevel, std::map<K, V>& ioMap, K iKey)
	{
	typename std::map<K, V>::iterator i = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, i != ioMap.end());
	ioMap.erase(i);
	}


template <typename K, typename V>
V sEraseAndReturn(const int iDebugLevel, std::map<K, V>& ioMap, K iKey)
	{	
	typename std::map<K, V>::iterator iter = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != iter);
	V result = (*iter).second;
	ioMap.erase(iter);
	return result;
	}


template <typename K, typename V>
void sInsertMustNotContain(const int iDebugLevel, std::map<K, V>& ioMap, K iKey, V iValue)
	{
	ZAssertStop(iDebugLevel, ioMap.end() == ioMap.find(iKey));
	ioMap.insert(typename std::map<K, V>::value_type(iKey, iValue));
	}

} // namespace ZUtil_STL

NAMESPACE_ZOOLIB_END

#endif // __ZUtil_STL__
