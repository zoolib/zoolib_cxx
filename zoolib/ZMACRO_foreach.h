/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZMACRO_foreach_h__
#define __ZMACRO_foreach_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_type_traits.h"
#include "zoolib/ZMACRO_auto.h"
#include "zoolib/ZMACRO_decltype.h"

#if ZCONFIG_SPI_Enabled(type_traits)

// =================================================================================================
// MARK: - ZMACRO_foreach helpers

namespace ZooLib {

template <typename ContainerRef>
struct ForEachWrapper_Forward_T
	{
	typedef typename remove_const<typename remove_reference<ContainerRef>::type>::type Container;

    inline ForEachWrapper_Forward_T(const Container& iContainer)
	:	fIter(iContainer.begin())
	,	fEnd(iContainer.end())
		{}

    typename Container::const_iterator fIter;
	const typename Container::const_iterator fEnd;
	};

template <typename ContainerRef>
struct ForEachWrapper_Reverse_T
	{
	typedef typename remove_const<typename remove_reference<ContainerRef>::type>::type Container;

    inline ForEachWrapper_Reverse_T(const Container& iContainer)
	:	fIter(iContainer.rbegin())
	,	fEnd(iContainer.rend())
		{}

    typename Container::const_reverse_iterator fIter;
	const typename Container::const_reverse_iterator fEnd;
	};

// This macro sets up __FEBreak used in each nested loop. It also takes a const
// reference to container, placing it in __CR, thus keeping container in scope.
#define ZMACRO_foreach_prefix(container) \
	for (int __FEBreak = 0; not __FEBreak; ++__FEBreak) \
	for (ZooLib::add_reference<ZooLib::add_const<ZMACRO_decltype(container)>::type>::type \
		__CR = container; not __FEBreak; ++__FEBreak) \

// These macros additionally set up the iterator and cached end value.
#define ZMACRO_foreach_prefix_Forward(container) \
	ZMACRO_foreach_prefix(container) \
	for (ZooLib::ForEachWrapper_Forward_T<ZMACRO_decltype(__CR)> __FEW(__CR); \
		not __FEBreak && __FEW.fIter != __FEW.fEnd; \
		++__FEW.fIter, ++__FEBreak) \

#define ZMACRO_foreach_prefix_Reverse(container) \
	ZMACRO_foreach_prefix(container) \
	for (ZooLib::ForEachWrapper_Reverse_T<ZMACRO_decltype(__CR)> __FEW(__CR); \
		not __FEBreak && __FEW.fIter != __FEW.fEnd; \
		++__FEW.fIter, ++__FEBreak) \

} // namespace ZooLib

// =================================================================================================
// MARK: - ZMACRO_foreachi

#ifndef ZMACRO_foreachi
	#define ZMACRO_foreachi(iter, container) \
		ZMACRO_foreach_prefix(container) \
		for (ZMACRO_auto_(iter,__CR.begin()), __FEE = __CR.end(); iter != __FEE; ++iter)
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachri

#ifndef ZMACRO_foreachri
	#define ZMACRO_foreachri(iter, container) \
		ZMACRO_foreach_prefix(container) \
		for (ZMACRO_auto_(iter,__CR.rbegin()), __FEE = __CR.rend(); iter != __FEE; ++iter)
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachv

#ifndef ZMACRO_foreachv
	#define ZMACRO_foreachv(vardecl, container) \
		ZMACRO_foreach_prefix_Forward(container) \
		for (vardecl = *__FEW.fIter; not __FEBreak; --__FEBreak)
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachrv

#ifndef ZMACRO_foreachrv
	#define ZMACRO_foreachrv(vardecl, container) \
		ZMACRO_foreach_prefix_Reverse(container) \
		for (vardecl = *__FEW.fIter; not __FEBreak; --__FEBreak)
#endif

// =================================================================================================
// MARK: - ZMACRO_foreacha

#ifndef ZMACRO_foreacha
	#define ZMACRO_foreacha(varname, container) \
		ZMACRO_foreach_prefix_Forward(container) \
		for (ZMACRO_auto(varname, *__FEW.fIter); not __FEBreak; --__FEBreak)
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachra

#ifndef ZMACRO_foreachra
	#define ZMACRO_foreachra(varname, container) \
		ZMACRO_foreach_prefix_Reverse(container) \
		for (ZMACRO_auto(varname, *__FEW.fIter); not __FEBreak; --__FEBreak)
#endif

#endif // ZCONFIG_SPI_Enabled(type_traits)

// =================================================================================================
// MARK: - Failure-marking macros

#ifndef ZMACRO_foreach_unsupported
	#define ZMACRO_foreach_unsupported (%ZMACRO_foreach_unsupported%)
#endif

#ifndef ZMACRO_foreachi
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

#ifndef ZMACRO_foreachri
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

#ifndef ZMACRO_foreachv
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

#ifndef ZMACRO_foreachrv
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

#ifndef ZMACRO_foreacha
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

#ifndef ZMACRO_foreachra
	#define ZMACRO_foreachi ZMACRO_foreach_unsupported
#endif

// =================================================================================================
// MARK: - foreachXX macros

#ifndef foreachi
	#define foreachi ZMACRO_foreachi
#endif

#ifndef foreachri
	#define foreachri ZMACRO_foreachri
#endif

#ifndef foreachv
	#define foreachv ZMACRO_foreachv
#endif

#ifndef foreachrv
	#define foreachrv ZMACRO_foreachrv
#endif

#ifndef foreacha
	#define foreacha ZMACRO_foreacha
#endif

#ifndef foreachra
	#define foreachra ZMACRO_foreachra
#endif

#endif // __ZMACRO_foreach_h__
