// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Relops_h__
#define __ZooLib_Util_Relops_h__
#include "zconfig.h"

#include "zoolib/ZTypes.h" // For EnableIfC

namespace ZooLib {

// =================================================================================================
#pragma mark - RelopsTraits

// Default templates positively say "No", and with Yes being SFINAE.

template <class S> struct RelopsTraits_HasEQ { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasNE { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasLT { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasGT { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasLE { enum { No = 1 }; };
template <class S> struct RelopsTraits_HasGE { enum { No = 1 }; };

// Explicit instantiations of RelopsTraits_HasXX can derive from this, so Yes is present
// with a truthy value, and No with a falsey one.
struct RelopsTraits_Has
	{ enum { Yes = 1, No = 0 }; };

// =================================================================================================
#pragma mark - Free relational operators, with conditionalized availability

template <class T>
typename EnableIfC
	<  RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasNE<T>::Yes
	,bool>::type
operator==(const T& l, const T& r)
	{ return not (l != r); }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasLT<T>::Yes
	,bool>::type
operator==(const T& l, const T& r)
	{ return not (l < r || r < l); }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasEQ<T>::Yes
	,bool>::type
operator!=(const T& l, const T& r)
	{ return not (l == r); }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasNE<T>::No
	&& RelopsTraits_HasEQ<T>::No
	&& RelopsTraits_HasLT<T>::Yes
	,bool>::type
operator!=(const T& l, const T& r)
	{ return l < r || r < l; }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasLT<T>::No
	&& RelopsTraits_HasGT<T>::Yes
	,bool>::type
operator<(const T& l, const T& r)
	{ return r > l; }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasGT<T>::No
	&& RelopsTraits_HasLT<T>::Yes
	,bool>::type
operator>(const T& l, const T& r)
	{ return r < l; }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasLE<T>::No
	&& RelopsTraits_HasLT<T>::Yes
	,bool>::type
operator<=(const T& l, const T& r)
	{ return not (r < l); }

template <class T>
typename EnableIfC
	<  RelopsTraits_HasGE<T>::No
	&& RelopsTraits_HasLT<T>::Yes
	,bool>::type
operator>=(const T& l, const T& r)
	{ return not (l < r); }

} // namespace ZooLib

#endif // __ZooLib_Util_Relops_h__
