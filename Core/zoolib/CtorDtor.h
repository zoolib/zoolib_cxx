// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_CtorDtor_h__
#define __ZooLib_CtorDtor_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Placement construction/destruction

template <class T>
inline T* sCtor_T(void* oBytes)
	{
#if ZCONFIG(Compiler,MSVC)
	// We *want* POD types to be default initialized.
	#pragma warning(push)
	#pragma warning(disable:4345)
#endif

	new(oBytes) T();

#if ZCONFIG(Compiler,MSVC)
	#pragma warning(pop)
#endif

	return static_cast<T*>(oBytes);
	}

template <class T, class P0>
inline T* sCtor_T(void* oBytes, const P0& i0)
	{
	new(oBytes) T(i0);
	return static_cast<T*>(oBytes);
	}

template <class T, class P0, class P1>
inline T* sCtor_T(void* oBytes, const P0& i0, const P1& i1)
	{
	new(oBytes) T(i0, i1);
	return static_cast<T*>(oBytes);
	}

template <class T>
inline void sDtor_T(void* oBytes)
	{ static_cast<T*>(oBytes)->~T(); }

template <class T>
inline T& sAssign_T(void* ioBytes, const T& iOther)
	{ return *static_cast<T*>(ioBytes) = iOther; }

template <class T>
inline T* sCtorFromVoidStar_T(void* oBytes, const void* iSource)
	{
	new(oBytes) T(*static_cast<const T*>(iSource));
	return static_cast<T*>(oBytes);
	}

template <class T>
inline T& sAssignFromVoidStar_T(void* ioBytes, const void* iBytes)
	{ return *static_cast<T*>(ioBytes) = *static_cast<const T*>(iBytes); }

// =================================================================================================
#pragma mark - Access

template <class T>
inline const T* sFetch_T(const void* iBytes)
	{ return static_cast<const T*>(iBytes); }

template <class T>
inline T* sFetch_T(void* iBytes)
	{ return static_cast<T*>(iBytes); }

} // namespace ZooLib

#endif // __ZooLib_CtorDtor_h__
