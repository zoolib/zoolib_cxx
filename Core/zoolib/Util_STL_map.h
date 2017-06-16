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

#ifndef __ZooLib_Util_STL_map_h__
#define __ZooLib_Util_STL_map_h__ 1
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <map>

namespace ZooLib {
namespace Util_STL {

// =================================================================================================
#pragma mark -
#pragma mark Util_STL

template <typename KBase, typename Value, typename Comparator>
bool sIsEmpty(const std::map<KBase,Value,Comparator>& iMap)
	{ return iMap.empty(); }

template <typename KBase, typename Value, typename Comparator>
bool sNotEmpty(const std::map<KBase,Value,Comparator>& iMap)
	{ return not sIsEmpty(iMap); }

template <typename KBase, typename Value, typename Comparator>
void
sClear(std::map<KBase,Value,Comparator>& ioMap)
	{ ioMap.clear(); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sContains(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return iMap.end() != iMap.find(iKey); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sQErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ return ioMap.erase(iKey); }

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ ioMap.erase(iKey); }

template <typename KBase, typename Value, typename Comparator>
typename std::map<KBase,Value,Comparator>::iterator
sEraseInc(std::map<KBase,Value,Comparator>& ioMap,
	typename std::map<KBase,Value,Comparator>::iterator iter)
	{
	ZAssert(ioMap.end() != iter);
	return ioMap.erase(iter);
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value* sPGet(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::const_iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sDGet(
	const Value& iDefault, const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return iDefault;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const ZQ<Value> sQGet(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return null;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGet(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	if (const Value* theP = sPGet(iMap, iKey))
		return *theP;
	return sDefault<Value>();
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value* sPMut(std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = iMap.find(iKey);
	if (iMap.end() == ii)
		return nullptr;
	return &ii->second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value& sDMut(
	const Value& iDefault, std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii =
		ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iDefault));
	return ii->first.second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value& sMut(
	std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return iMap[iKey]; }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sSet(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ ioMap[iKey] = iValue; }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
bool sQInsert(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	return ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue)).second;
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
bool sQReplace(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return false;
	ii->second = iValue;
	return true;
	}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sQGetErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return null;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sDGetErase(
	const Value& iDefault, std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return iDefault;
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	if (ioMap.end() == ii)
		return sDefault<Value>();
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

// -----

template <typename KBase, typename Value, typename Comparator>
ZQ<std::pair<KBase,Value> > sQPopFront(std::map<KBase,Value,Comparator>& ioMap)
	{
	if (ioMap.empty())
		return null;
	const std::pair<KBase,Value> result = *ioMap.begin();
	ioMap.erase(ioMap.begin());
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark sXXXMust

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sEraseMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	const bool result = sQErase(ioMap, iKey);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sEraseMust(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ sEraseMust(1, ioMap, iKey); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGetMust(const int iDebugLevel,
	const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	const Value* theP = sPGet(iMap, iKey);
	ZAssertStop(iDebugLevel, theP);
	return *theP;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGetMust(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return sGetMust(1, iMap, iKey); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{
	const bool result = sQInsert(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iKey, iValue);}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{
	const typename std::map<KBase,Value,Comparator>::iterator result =
	ioMap.insert(
		iIter,
		typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue));

	ZAssertStop(iDebugLevel, result->second == iValue); // ??? Not sure about this
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sInsertMust(
	std::map<KBase,Value,Comparator>& ioMap,
	const typename std::map<KBase,Value,Comparator>::iterator& iIter,
	const KDerived& iKey, const VDerived& iValue)
	{ sInsertMust(1, ioMap, iIter, iKey, iValue);}

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sReplaceMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	const bool result = sQReplace(ioMap, iKey, iValue);
	ZAssertStop(iDebugLevel, result);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived, typename VDerived>
void sReplaceMust(
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const VDerived& iValue)
	{ sReplaceMust(1, ioMap, iKey, iValue); }

// -----

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetEraseMust(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator ii = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != ii);
	const Value result = ii->second;
	ioMap.erase(ii);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sGetEraseMust(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ return sGetEraseMust(1, ioMap, iKey); }

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_map_h__
