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

#if ZCONFIG_CPP >= 2011

#ifndef foreachv
	#define foreachv(vardecl, container) for (vardecl : container)
#endif

#ifndef foreacha
	#define foreacha(varname, container) for (auto&& varname : container)
#endif

#else // ZCONFIG_CPP >= 2011

#include "zoolib/Compat_type_traits.h" // For remove_const and remove_reference

#include "zoolib/ZMACRO_auto.h"
#include "zoolib/ZMACRO_decltype.h"

#if ZCONFIG_SPI_Enabled(type_traits) && defined(ZMACRO_auto) && defined(ZMACRO_decltype)

// =================================================================================================
#pragma mark -

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

// This macro sets up __FEBreak used in each nested loop. It also takes a const
// reference to container, placing it in __CR, thus keeping container in scope.
#define ZMACRO_foreach_prefix(container) \
	for (int __FEBreak = 0; not __FEBreak; ++__FEBreak) \
		for (typename ZooLib::add_lvalue_reference \
			<typename ZooLib::add_const \
			<ZMACRO_decltype(container)>::type>::type \
			__CR = container; not __FEBreak; ++__FEBreak) \
			for (ZooLib::ForEachWrapper_Forward_T<ZMACRO_decltype(__CR)> __FEW(__CR); \
				not __FEBreak && __FEW.fIter != __FEW.fEnd; \
				++__FEW.fIter, ++__FEBreak)

} // namespace ZooLib

// =================================================================================================
#pragma mark - foreachv and foreacha

#ifndef foreachv
	#define foreachv(vardecl, container) \
		ZMACRO_foreach_prefix(container) \
		for (vardecl = *__FEW.fIter; not __FEBreak; --__FEBreak)
#endif

#ifndef foreacha
	#define foreacha(varname, container) \
		ZMACRO_foreach_prefix(container) \
		for (ZMACRO_auto(varname, *__FEW.fIter); not __FEBreak; --__FEBreak)
#endif

#endif // ZCONFIG_SPI_Enabled(type_traits) && defined(ZMACRO_auto) && defined(ZMACRO_decltype)
#endif // ZCONFIG_CPP >= 2011
#endif // __ZMACRO_foreach_h__
