/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZCtorDtor__
#define __ZCtorDtor__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Placement construction/destruction

template <class T>
inline T* sConstruct_T(void* oBytes)
	{
	new(oBytes) T();
	return static_cast<T*>(oBytes);
	}

template <class T, class P0>
inline T* sConstruct_T(void* oBytes, const P0& iP0)
	{
	new(oBytes) T(iP0);
	return static_cast<T*>(oBytes);
	}

template <class T, class P0, class P1>
inline T* sConstruct_T(void* oBytes, const P0& iP0, const P1& iP1)
	{
	new(oBytes) T(iP0, iP1);
	return static_cast<T*>(oBytes);
	}

template <class T>
inline void sDestroy_T(void* oBytes)
	{ static_cast<T*>(oBytes)->~T(); }

template <class T>
inline T& sAssign_T(void* ioBytes, const T& iOther)
	{ return *static_cast<T*>(ioBytes) = iOther; }

template <class T>
inline const T* sFetch_T(const void* iBytes)
	{ return static_cast<const T*>(iBytes); }

template <class T>
inline T* sFetch_T(void* iBytes)
	{ return static_cast<T*>(iBytes); }

template <class T>
inline T* sConstructFromVoidStar_T(void* oBytes, const void* iSource)
	{
	new(oBytes) T(*static_cast<const T*>(iSource));
	return static_cast<T*>(oBytes);
	}

template <class T>
inline T& sAssignFromVoidStar_T(void* ioBytes, const void* iBytes)
	{ return *static_cast<T*>(ioBytes) = *static_cast<const T*>(iBytes); }

} // namespace ZooLib

#endif // __ZCtorDtor__
