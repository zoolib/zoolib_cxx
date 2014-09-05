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

#ifndef __ZUtil_Relops_h__
#define __ZUtil_Relops_h__
#include "zconfig.h"

#include "ZTypes.h" // For EnableIfC

namespace ZooLib {

// =================================================================================================
// MARK: - RelopsTraits

struct RelopsTraits_Has
	{ enum { Yes = 1, No = 0 }; };

template <class S> struct RelopsTraits_HasEQ { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasNE { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasLT { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasGT { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasLE { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasGE { enum { No = 1 }; };

// =================================================================================================
// MARK: - Free relational operators, with conditionalized availability

template <class T>
typename EnableIfC
	<RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasNE<T>::Yes,bool>::type
operator==(const T& l, const T& r)
	{ return not (l != r); }

template <class T>
typename EnableIfC
	<RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasLT<T>::Yes,bool>::type
operator==(const T& l, const T& r)
	{ return not (l < r || r < l); }

template <class T>
typename EnableIfC
	<RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasEQ<T>::Yes,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l == r); }

template <class T>
typename EnableIfC
	<RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasLT<T>::Yes,bool>::type
operator!=(const T& l, const T& r)
	{ return l < r || r < l; }

template <class T>
typename EnableIfC
	<RelopsTraits_HasLT<T>::No
	&& RelopsTraits_HasGT<T>::Yes,bool>::type
operator<(const T& l, const T& r)
	{ return l > r; }

template <class T>
typename EnableIfC
	<RelopsTraits_HasGT<T>::No
	&& RelopsTraits_HasLT<T>::Yes,bool>::type
operator>(const T& l, const T& r)
	{ return r < l; }

template <class T>
typename EnableIfC
	<RelopsTraits_HasLE<T>::No
	&& RelopsTraits_HasLT<T>::Yes,bool>::type
operator<=(const T& l, const T& r)
	{ return not (r < l); }

template <class T>
typename EnableIfC
	<RelopsTraits_HasGE<T>::No
	&& RelopsTraits_HasLT<T>::Yes,bool>::type
operator>=(const T& l, const T& r)
	{ return not (l < r); }

} // namespace ZooLib

#endif // __ZUtil_Relops_h__
