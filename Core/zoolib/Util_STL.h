// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_STL_h__
#define __ZooLib_Util_STL_h__ 1
#include "zconfig.h"

#include <type_traits> // For enable_if, is_member_function_pointer

#include "zoolib/ZTypes.h" // For EnableIf_t

namespace ZooLib {
namespace Util_STL {

using std::is_member_function_pointer;

// =================================================================================================

template <typename T> struct IsAType { enum {value=1}; };

template <typename T, typename... Other> struct AreTypes { enum {value=1}; };

// =================================================================================================

template <typename T, typename S> struct AreSameType {};

template <typename T> struct AreSameType<T,T> { enum {value=1}; };

// =================================================================================================

/** Invoke delete on all elements between begin and end. */
template <class ForwardIterator>
void sDeleteAll(ForwardIterator begin, ForwardIterator end)
	{
	while (begin != end)
		delete *begin++;
	}

// =================================================================================================

template <typename T>
EnableIf_t<is_member_function_pointer<decltype(&T::empty)>::value,
	bool>
sIsEmpty(const T& iT)
	{ return iT.empty(); }

// =================================================================================================

template <typename T>
EnableIf_t<is_member_function_pointer<decltype(&T::empty)>::value,
	bool>
sNotEmpty(const T& iT)
	{ return not iT.empty(); }

// =================================================================================================

template <typename T>
EnableIf_t<is_member_function_pointer<decltype(&T::clear)>::value,
	void>
sClear(T& ioT)
	{ ioT.clear(); }

// =================================================================================================

//template <typename Container>
//	EnableIf_t<IsAType<typename Container::key_type>::value,
//		EnableIf_t<AreSameType<size_t,decltype(((Container*)nullptr)->erase(*(const typename Container::key_type*)nullptr))>::value,
//			bool>>
//sQErase2(Container& ioContainer, const typename Container::key_type& iKey)
//	{ return ioContainer.erase(iKey); }

// =================================================================================================

template <typename Container>
	EnableIf_t<IsAType<typename Container::key_type>::value,
		void>
sErase(Container& ioContainer, const typename Container::key_type& iKey)
	{ ioContainer.erase(iKey); }

// =================================================================================================

//template <typename Container>
//EnableIf_t<is_member_function_pointer<decltype(&Container::end)>::value,
//	EnableIf_t<is_member_function_pointer<decltype(&Container::erase)>::value,
//		EnableIf_t<IsAType<typename Container::iterator>::value,
//			typename Container::Iterator>>>
//sEraseInc(Container& ioContainer, typename Container::iterator iter)
//	{
//	ZAssert(ioContainer.end() != iter);
//	return ioContainer.erase(iter);
//	}

// =================================================================================================

} // namespace Util_STL
} // namespace ZooLib

#endif // __ZooLib_Util_STL_h__
