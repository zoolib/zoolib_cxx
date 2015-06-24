/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
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

#ifndef __ZooLib_Default_h__
#define __ZooLib_Default_h__ 1
#include "zconfig.h"

#include "zoolib/Singleton.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark sDefault<Type_p>()

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
#pragma mark -
#pragma mark sDefault()

struct Default_t { template <class T> operator const T&() { return sSingleton<T,Tag_Default>(); } };

inline Default_t sDefault() { return Default_t(); }

} // namespace ZooLib

#endif // __ZooLib_Default_h__
