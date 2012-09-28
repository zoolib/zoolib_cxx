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

#include "ZTypes.h"

namespace ZooLib {

// =================================================================================================
// MARK: - RelopsTraits

template <class S> struct RelopsTraits_HasEQ { enum { Value = 0 }; };
template <class S> struct RelopsTraits_HasNE { enum { Value = 0 }; };
template <class S> struct RelopsTraits_HasLT { enum { Value = 0 }; };
template <class S> struct RelopsTraits_HasGT { enum { Value = 0 }; };
template <class S> struct RelopsTraits_HasLE { enum { Value = 0 }; };
template <class S> struct RelopsTraits_HasGE { enum { Value = 0 }; };

// =================================================================================================
// MARK: - Free relational operators, with conditionalized availability

template <class T>
typename EnableIfC<not RelopsTraits_HasLT<T>::Value && RelopsTraits_HasGT<T>::Value,bool>::type
operator<(const T& l, const T& r)
	{ return l > r; }

template <class T>
typename EnableIfC<not RelopsTraits_HasGT<T>::Value && RelopsTraits_HasLT<T>::Value,bool>::type
operator>(const T& l, const T& r)
	{ return r < l; }

template <class T>
typename EnableIfC<not RelopsTraits_HasLE<T>::Value && RelopsTraits_HasLT<T>::Value,bool>::type
operator<=(const T& l, const T& r)
	{ return not (r < l); }

template <class T>
typename EnableIfC<not RelopsTraits_HasGE<T>::Value && RelopsTraits_HasLT<T>::Value,bool>::type
operator>=(const T& l, const T& r)
	{ return not (l < r); }

template <class T>
typename EnableIfC<not RelopsTraits_HasNE<T>::Value && RelopsTraits_HasLT<T>::Value,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l == r); }

template <class T>
typename EnableIfC
	<not RelopsTraits_HasNE<T>::Value
	&& not RelopsTraits_HasEQ<T>::Value
	&& RelopsTraits_HasLT<T>::Value,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l < r || r < l); }

} // namespace ZooLib

#endif // __ZUtil_Relops_h__
