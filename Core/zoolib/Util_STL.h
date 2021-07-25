// Copyright (c) 2002-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_h__
#define __ZooLib_Util_STL_h__ 1
#include "zconfig.h"

#include <type_traits> // For is_member_function_pointer, is_same etc.

#include "zoolib/ZTypes.h" // For EnableIf_t

namespace ZooLib {
namespace Util_STL {

// =================================================================================================

template <typename T> struct IsAType : std::true_type {};

template <typename T, typename... Other> struct AreTypes : std::true_type {};

// =================================================================================================

template <typename T, typename S> using AreSameType = std::is_same<T,S>;

// =================================================================================================

template <class mfp> using IsMFP = std::is_member_function_pointer<mfp>;

// =================================================================================================

template <class mfp,class mfptype>
struct IsMFPWithSig
	{
	enum {value = IsMFP<mfp>::value * AreSameType<mfp,mfptype>::value};
	};

// =================================================================================================
// sDeleteAll

template <class ForwardIterator>
void sDeleteAll(ForwardIterator begin, ForwardIterator end)
	{
	while (begin != end)
		delete *begin++;
	}

// =================================================================================================
// sCount

template <typename T>
auto sCount(const T& iT)
-> decltype(iT.size())
	{ return iT.size(); }

// =================================================================================================
// sIsEmpty

template <typename T>
auto sIsEmpty(const T& iT)
-> decltype(bool(iT.empty()))
	{ return bool(iT.empty()); }

template <typename T>
auto sIsEmpty(const T& iT)
-> decltype(bool(iT.IsEmpty()))
	{ return bool(iT.IsEmpty()); }

// =================================================================================================
// sNotEmpty

template <typename T>
auto sNotEmpty(const T& iT)
-> decltype(bool(iT.empty()))
	{ return not bool(iT.empty()); }

template <typename T>
auto sNotEmpty(const T& iT)
-> decltype(not bool(iT.IsEmpty()))
	{ return not bool(iT.IsEmpty()); }

// =================================================================================================
// sClear

template <typename T>
auto sClear(T& ioT)
-> decltype(ioT.clear(),
	void())
	{ ioT.clear(); }

template <typename T>
auto sClear(T& ioT)
-> decltype(ioT.Clear(),
	void())
	{ ioT.Clear(); }

// =================================================================================================
// sContains

template <typename CC, typename KK>
auto sContains(const CC& iContainer, KK iKey)
-> decltype(bool(iContainer.end() != iContainer.find(iKey)))
	{ return bool(iContainer.end() != iContainer.find(iKey)); }

// =================================================================================================
// sQErase(key)

template <typename CC, typename KK>
auto sQErase(CC& ioContainer, KK iKey)
-> decltype(bool(ioContainer.erase(iKey)))
	{ return bool(ioContainer.erase(iKey)); }

// =================================================================================================
// sErase(key)

template <typename CC, typename KK>
auto sErase(CC& ioContainer, KK iKey)
-> decltype(void(ioContainer.erase(iKey)))
	{ ioContainer.erase(iKey); }

// =================================================================================================
// sQErase(iterator)

template <typename CC, typename II>
auto sQErase(CC& ioContainer, II iter)
-> decltype(ioContainer.end() == iter, ioContainer.erase(iter),
	bool())
	{
	if (ioContainer.end() == iter)
		return false;
	ioContainer.erase(iter);
	return true;
	}

// =================================================================================================
// sErase(iterator)

template <typename CC, typename II>
auto sErase(CC& ioContainer, II iter)
-> decltype(ioContainer.end() == iter, ioContainer.erase(iter),
	void())
	{
	ZAssert(not (ioContainer.end() == iter));
	ioContainer.erase(iter);
	}

// =================================================================================================
// sEraseInc

template <typename CC, typename II>
auto sEraseInc(CC& ioContainer, II iter)
-> decltype(ioContainer.end() == iter, ioContainer.erase(iter))
	{
	ZAssert(not (ioContainer.end() == iter));
	return ioContainer.erase(iter);
	}

// =================================================================================================
// sFrontOrNullPtr

template <typename CC>
auto sFrontOrNullPtr(const CC& iContainer)
-> decltype(iContainer.empty(), iContainer.front(),
	typename CC::const_pointer())
	{
	if (iContainer.empty())
		return nullptr;
	return &iContainer.front();
	}

template <typename CC>
auto sFrontOrNullPtr(CC& ioContainer)
-> decltype(ioContainer.empty(), ioContainer.front(),
	typename CC::pointer())
	{
	if (ioContainer.empty())
		return nullptr;
	return &ioContainer.front();
	}

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_h__
