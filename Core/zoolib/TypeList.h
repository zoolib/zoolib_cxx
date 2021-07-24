// Copyright (c) 2016-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_TypeList_h__
#define __ZooLib_TypeList_h__

#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - TypeList

template <
	class P0 = void, class P1 = void, class P2 = void, class P3 = void,
	class P4 = void, class P5 = void, class P6 = void, class P7 = void,
	class P8 = void
	>
struct TypeList
	{
	typedef P0 T0;
	typedef P1 T1;
	typedef P2 T2;
	typedef P3 T3;
	typedef P4 T4;
	typedef P5 T5;
	typedef P6 T6;
	typedef P7 T7;
	typedef P8 T8;
	};

// ----------

template <class P, class TL>
struct TypeListPrefixed
	{
	typedef TypeList<
		P,               typename TL::T0, typename TL::T1, typename TL::T2,
		typename TL::T3, typename TL::T4, typename TL::T5, typename TL::T6,
		typename TL::T7> Result_t;
	};

// ----------

template <class TL>
struct TypeListWithoutPrefix
	{
	typedef TypeList<
		typename TL::T1, typename TL::T2, typename TL::T3, typename TL::T4,
		typename TL::T5, typename TL::T6, typename TL::T7, typename TL::T8> Result_t;
	};

} // namespace ZooLib

#endif // __ZooLib_TypeList_h__
