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

template <class S>
struct RelopsTraits
	{
	enum
		{
		eHas_EQ = 0,
		eHas_NE = 0,
		eHas_LT = 0,
		eHas_GT = 0,
		eHas_LE = 0,
		eHas_GE = 0
		};
	};

// =================================================================================================
// MARK: - Free relational operators, with conditionalized availability

template <class T>
typename EnableIfC<not RelopsTraits<T>::eHas_LT && RelopsTraits<T>::eHas_GT,bool>::type
operator<(const T& l, const T& r)
	{ return l > r; }

template <class T>
typename EnableIfC<not RelopsTraits<T>::eHas_GT && RelopsTraits<T>::eHas_LT,bool>::type
operator>(const T& l, const T& r)
	{ return r < l; }

template <class T>
typename EnableIfC<not RelopsTraits<T>::eHas_LE && RelopsTraits<T>::eHas_LT,bool>::type
operator<=(const T& l, const T& r)
	{ return not (r < l); }

template <class T>
typename EnableIfC<not RelopsTraits<T>::eHas_GE && RelopsTraits<T>::eHas_LT,bool>::type
operator>=(const T& l, const T& r)
	{ return not (l < r); }

template <class T>
typename EnableIfC<not RelopsTraits<T>::eHas_NE && RelopsTraits<T>::eHas_EQ,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l == r); }

template <class T>
typename EnableIfC
	<not RelopsTraits<T>::eHas_NE
	&& not RelopsTraits<T>::eHas_EQ
	&& RelopsTraits<T>::eHas_LT,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l < r || r < l); }

} // namespace ZooLib

#endif // __ZUtil_Relops_h__
