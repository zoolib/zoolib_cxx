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

#ifndef __ZUtil_STL_map__
#define __ZUtil_STL_map__
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

#include <map>

namespace ZooLib {
namespace ZUtil_STL {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_STL

/** Returns true if iMap has an element at iKey. */
template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sContains(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return iMap.end() != iMap.find(iKey); }

template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sEraseIfContains(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ return ioMap.erase(iKey); }

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sEraseMustContain(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	size_t count = ioMap.erase(iKey);
	ZAssertStop(iDebugLevel, count);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
Value sEraseAndReturn(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator iter = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != iter);
	const Value result = (*iter).second;
	ioMap.erase(iter);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sQErase(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::iterator i = ioMap.find(iKey);
	if (ioMap.end() == i)
		return null;
	const Value result = i->second;
	ioMap.erase(i);
	return result;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sEraseAndReturnIfContains(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey)
	{ return sQErase(ioMap, iKey); }

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sInsertMustNotContain(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	const bool didInsert =
		ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue)).second;
	ZAssertStop(iDebugLevel, didInsert);
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sInsertIfNotContains
	(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	return ioMap.insert(typename std::map<KBase,Value,Comparator>::value_type(iKey, iValue)).second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
void sSetMustContain(const int iDebugLevel,
	std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	typename std::map<KBase,Value,Comparator>::iterator i = ioMap.find(iKey);
	ZAssertStop(iDebugLevel, ioMap.end() != i);
	i->second = iValue;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
bool sSetIfContains(std::map<KBase,Value,Comparator>& ioMap, const KDerived& iKey, const Value& iValue)
	{
	typename std::map<KBase,Value,Comparator>::iterator i = ioMap.find(iKey);
	if (ioMap.end() == i)
		return false;
	i->second = iValue;
	return true;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
const Value& sGetMustContain(const int iDebugLevel,
	const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::const_iterator i = iMap.find(iKey);
	ZAssertStop(iDebugLevel, iMap.end() != i);
	return i->second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sQGet(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{
	typename std::map<KBase,Value,Comparator>::const_iterator i = iMap.find(iKey);
	if (iMap.end() == i)
		return null;
	return i->second;
	}

template <typename KBase, typename Value, typename Comparator, typename KDerived>
ZQ<Value> sGetIfContains(const std::map<KBase,Value,Comparator>& iMap, const KDerived& iKey)
	{ return sQGet(iMap, iKey); }

} // namespace ZUtil_STL
} // namespace ZooLib

#endif // __ZUtil_STL_map__
