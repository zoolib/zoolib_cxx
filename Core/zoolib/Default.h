// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Default_h__
#define __ZooLib_Default_h__ 1
#include "zconfig.h"

#include "zoolib/Singleton.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sDefault<Type_p>()

template <class Type_p> struct DefaultTraits { typedef const Type_p& Return_t; };

template <> struct DefaultTraits<void> { typedef void Return_t; };

struct Tag_Default;

template <class Type_p>
typename DefaultTraits<Type_p>::Return_t sDefault()
	{ return sSingleton<Type_p,Tag_Default>(); }

template <>
inline DefaultTraits<void>::Return_t sDefault<void>()
	{}

// =================================================================================================
#pragma mark - sDefault()

struct Default_t { template <class T> operator const T&() { return sSingleton<T,Tag_Default>(); } };

inline Default_t sDefault() { return Default_t(); }

} // namespace ZooLib

#endif // __ZooLib_Default_h__
