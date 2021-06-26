// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

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
	EnableIf_t<IsMFP<decltype(static_cast<typename T::size_type(T::*)() const>
		(&T::size))>::value,
typename T::size_type>
sCount(const T& iT)
	{ return iT.size(); }

// =================================================================================================
// sIsEmpty

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<bool(T::*)() const>
		(&T::empty))>::value,
bool>
sIsEmpty(const T& iT)
	{ return iT.empty(); }

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<bool(T::*)() const>
		(&T::IsEmpty))>::value,
bool>
sIsEmpty(const T& iT)
	{ return iT.IsEmpty(); }

// =================================================================================================
// sNotEmpty

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<bool(T::*)() const>
		(&T::empty))>::value,
bool>
sNotEmpty(const T& iT)
	{ return not iT.empty(); }

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<bool(T::*)() const>
		(&T::IsEmpty))>::value,
bool>
sNotEmpty(const T& iT)
	{ return not iT.IsEmpty(); }

// =================================================================================================
// sClear

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<void(T::*)()>
		(&T::clear))>::value,
void>
sClear(T& ioT)
	{ ioT.clear(); }

template <typename T>
	EnableIf_t<IsMFP<decltype(static_cast<void(T::*)()>
		(&T::Clear))>::value,
void>
sClear(T& ioT)
	{ ioT.Clear(); }

// =================================================================================================
// sContains

template <typename CC, typename KK>
	EnableIf_t<IsMFP<decltype(static_cast<
		typename CC::const_iterator(CC::*)(const typename CC::key_type&) const>
		(&CC::find))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<
			typename CC::const_iterator(CC::*)() const>
			(&CC::end))>::value,
bool>>
sContains(const CC& iContainer, KK iKey)
	{ return iContainer.end() != iContainer.find(iKey); }

template <typename CC, typename KK>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)(const typename CC::key_type&)>
		(&CC::find))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)() const>
		(&CC::end))>::value,
bool>>
sContains(CC& ioContainer, KK iKey)
	{ return ioContainer.end() != ioContainer.find(iKey); }

// =================================================================================================
// sQErase(key)

template <typename CC, typename KK>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::size_type(CC::*)(const typename CC::key_type&)>
		(&CC::erase))>::value,
bool>
sQErase(CC& ioContainer, KK iKey)
	{ return ioContainer.erase(iKey); }

// =================================================================================================
// sErase(key)

template <typename CC, typename KK>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::size_type(CC::*)(const typename CC::key_type&)>
		(&CC::erase))>::value,
void>
sErase(CC& ioContainer, KK iKey)
	{ ioContainer.erase(iKey); }

// =================================================================================================
// sQErase(iterator)

template <typename CC, typename II>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)(II)>
		(&CC::erase))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)() const>
		(&CC::end))>::value,
bool>>
sQErase(CC& ioContainer, II iter)
	{
	if (ioContainer.end() == iter)
		return false;
	ioContainer.erase(iter);
	return true;
	}

// =================================================================================================
// sErase(iterator)

template <typename CC, typename II>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)(II)>
		(&CC::erase))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)() const>
		(&CC::end))>::value,
void>>
sErase(CC& ioContainer, II iter)
	{
	ZAssert(ioContainer.end() != iter);
	ioContainer.erase(iter);
	}

// =================================================================================================
// sEraseInc

template <typename CC, typename II>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)(II)>
		(&CC::erase))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<typename CC::iterator(CC::*)() const>
			(&CC::end))>::value,
typename CC::iterator>>
sEraseInc(CC& ioContainer, II iter)
	{
	ZAssert(ioContainer.end() != iter);
	return ioContainer.erase(iter);
	}

// =================================================================================================
// sFrontOrNullPtr

template <typename CC>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::reference(CC::*)()>
		(&CC::front))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<bool(CC::*)() const>
			(&CC::empty))>::value,
typename CC::pointer>>
sFrontOrNullPtr(CC& ioContainer)
	{
	if (ioContainer.empty())
		return nullptr;
	return &ioContainer.front();
	}

template <typename CC>
	EnableIf_t<IsMFP<decltype(static_cast<typename CC::const_reference(CC::*)() const>
		(&CC::front))>::value,
		EnableIf_t<IsMFP<decltype(static_cast<bool(CC::*)() const>
			(&CC::empty))>::value,
typename CC::const_pointer>>
sFrontOrNullPtr(const CC& iContainer)
	{
	if (iContainer.empty())
		return nullptr;
	return &iContainer.front();
	}

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_h__
