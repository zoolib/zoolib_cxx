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

#include "zoolib/ZMACRO_auto.h"
#include "zoolib/ZMACRO_decltype.h"

// =================================================================================================
// MARK: - ZMACRO_foreachi

#ifdef ZMACRO_foreachi
#elif 0
	#define ZMACRO_foreachi(iter, cont) \
		for (ZMACRO_auto_(iter,(cont).begin()), \
			foreach_end = (cont).end(); \
			iter != foreach_end; ++iter)
#elif 1
	#define ZMACRO_foreachi(iter, cont) \
		for (ZMACRO_auto_(iter,(cont).begin()), \
			ZMACRO_Concat(foreach_end,__LINE__) = (cont).end(); \
			iter != ZMACRO_Concat(foreach_end,__LINE__); ++iter)
#endif

#ifndef foreachi
	#define foreachi ZMACRO_foreachi
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachri

#ifndef ZMACRO_foreachri
	#define ZMACRO_foreachri(iter, cont) \
		for (ZMACRO_auto_(iter,(cont).rbegin()), \
			foreach_end = (cont).rend(); \
			iter != foreach_end; ++iter)
#endif

#ifndef foreachri
	#define foreachri ZMACRO_foreachri
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachv

namespace ZooLib {

template <typename Container>
class ZWrapper_foreachv_T
	{
public:
    inline ZWrapper_foreachv_T(const Container& iContainer)
	:	fIter(iContainer.begin())
	,	fEnd(iContainer.end())
	,	fMismatch(0)
		{}

    typename Container::const_iterator fIter;
	const typename Container::const_iterator fEnd;
    int fMismatch;
	};

} // namespace ZooLib

#ifdef ZMACRO_foreachv
#elif 0
	// Terse version without line number suffix on local wrapper.
	#define ZMACRO_foreachv(vardecl, cont) \
		for (ZooLib::ZWrapper_foreachv_T<ZMACRO_decltype(cont)> wrap(cont); \
			not wrap.fMismatch && wrap.fIter != wrap.fEnd; \
			++wrap.fIter, ++wrap.fMismatch) \
			for (vardecl = *wrap.fIter; not wrap.fMismatch; --wrap.fMismatch)

#elif 1
	#define ZMACRO_foreachv(VarDeclaration, Container) \
		for (ZooLib::ZWrapper_foreachv_T<ZMACRO_decltype(Container)> \
			ZMACRO_Concat(Wrapper_foreachv,__LINE__)(Container); \
			not ZMACRO_Concat(Wrapper_foreachv,__LINE__).fMismatch \
			&& ZMACRO_Concat(Wrapper_foreachv,__LINE__).fIter \
			!= ZMACRO_Concat(Wrapper_foreachv,__LINE__).fEnd; \
			++ZMACRO_Concat(Wrapper_foreachv,__LINE__).fIter, \
			++ZMACRO_Concat(Wrapper_foreachv,__LINE__).fMismatch) \
			for (VarDeclaration = \
			*ZMACRO_Concat(Wrapper_foreachv,__LINE__).fIter; \
			not ZMACRO_Concat(Wrapper_foreachv,__LINE__).fMismatch; \
			--ZMACRO_Concat(Wrapper_foreachv,__LINE__).fMismatch)

#endif

#ifndef foreachv
	#define foreachv ZMACRO_foreachv
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachrv

namespace ZooLib {

template <typename Container>
class ZWrapper_foreachrv_T
	{
public:
    inline ZWrapper_foreachrv_T(const Container& iContainer)
	:	fIter(iContainer.rbegin())
	,	fEnd(iContainer.rend())
	,	fMismatch(0)
		{}

    typename Container::const_reverse_iterator fIter;
	const typename Container::const_reverse_iterator fEnd;
    int fMismatch;
	};

} // namespace ZooLib

#ifdef ZMACRO_foreachrv
#elif 0
	// Terse version without line number suffix on local wrapper.
	#define ZMACRO_foreachrv(vardecl, cont) \
		for (ZooLib::ZWrapper_foreachrv_T<ZMACRO_decltype(cont)> wrap(cont); \
			not wrap.fMismatch && wrap.fIter != wrap.fEnd; \
			++wrap.fIter, ++wrap.fMismatch) \
			for (vardecl = *wrap.fIter; not wrap.fMismatch; --wrap.fMismatch)

#elif 1
	#define ZMACRO_foreachrv(VarDeclaration, Container) \
		for (ZooLib::ZWrapper_foreachrv_T<ZMACRO_decltype(Container)> \
			ZMACRO_Concat(Wrapper_foreachrv,__LINE__)(Container); \
			not ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fMismatch \
			&& ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fIter \
			!= ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fEnd; \
			++ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fIter, \
			++ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fMismatch) \
			for (VarDeclaration = \
			*ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fIter; \
			not ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fMismatch; \
			--ZMACRO_Concat(Wrapper_foreachrv,__LINE__).fMismatch)

#endif

#ifndef foreachrv
	#define foreachrv ZMACRO_foreachrv
#endif

#endif // __ZMACRO_foreach_h__
