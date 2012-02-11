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

// =================================================================================================
// MARK: - ZMACRO_foreachi

#define ZMACRO_foreachi(iter, cont) \
	for (ZMACRO_typeof(cont.begin()) iter = cont.begin(), \
	ZMACRO_Concat(cont##_##end,__LINE__) = cont.end(); \
	iter != ZMACRO_Concat(cont##_##end,__LINE__); ++iter)

#ifndef foreachi
	#define foreachi ZMACRO_foreachi
#endif

// =================================================================================================
// MARK: - ZMACRO_foreachv

namespace ZooLib {
template <typename Container>
class ZWrapper_foreachv_T
	{
public:
    inline ZWrapper_foreachv_T(const Container& iContainer)
	:	fContainer(iContainer)
	,	fIter(fContainer.begin())
	,	fEnd(fContainer.end())
	,	fBroke(0)
		{}

    const Container& fContainer;
    typename Container::const_iterator fIter;
	const typename Container::const_iterator fEnd;
    int fBroke;
	};

} // namespace ZooLib

#if 0
	// Curt version
	#define ZMACRO_foreachv(vardecl, cont) \
		for (ZWrapper_foreachv_T<ZMACRO_typeof(cont)> wrap(cont); \
			not wrap.fBroke && wrap.fIter != wrap.fEnd; \
			++wrap.fIter, ++wrap.fBroke) \
			for (vardecl = *wrap.fIter; not wrap.fBroke; --wrap.fBroke)

#else
	#define ZMACRO_foreachv(VarDeclaration, Container) \
		for (ZooLib::ZWrapper_foreachv_T<ZMACRO_typeof(Container)> \
		ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__))(Container); \
			not ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fBroke \
			&& ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fIter \
			!= ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fEnd; \
			++ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fIter, \
			++ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fBroke) \
			for (VarDeclaration = \
			*ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fIter; \
			not ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fBroke; \
			--ZMACRO_Concat(Container,ZMACRO_Concat(_Wrapper,__LINE__)).fBroke)

#endif

#ifndef foreachv
	#define foreachv ZMACRO_foreachv
#endif

#endif // __ZMACRO_foreach_h__
