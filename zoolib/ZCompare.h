/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCompare__
#define __ZCompare__ 1
#include "zconfig.h"

#include "zoolib/ZTypes.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

// A default implementation of sCompare_T is nice to have, but it does make
// it hard to tell when we accidentally call operator<.
template <class T> int sCompare_T(const T& iL, const T& iR);

// sCompare_T definitions for simple types. They require that int has more bits than int16.
template <> inline int sCompare_T(const ZType& iL, const ZType& iR)
	{ return int(iL) - int(iR); }

template <> inline int sCompare_T(const bool& iL, const bool& iR)
	{ return int(iL) - int(iR); }

// We use real type names, rather than the int8/int16/int32 etc stuff
// because there's no telling when an int8 is going to be a char vs a
// signed char, nor if an int32 is an int or a long, and we want sCompare_T
// to work for everything.

template <> inline int sCompare_T(const char& iL, const char& iR)
	{ return iL - iR; }

template <> inline int sCompare_T(const unsigned char& iL, const unsigned char& iR)
	{ return int(iL) - int(iR); }

template <> inline int sCompare_T(const signed char& iL, const signed char& iR)
	{ return iL - iR; }


template <> inline int sCompare_T(const short& iL, const short& iR)
	{ return iL - iR; }

template <> inline int sCompare_T(const unsigned short& iL, const unsigned short& iR)
	{ return int(iL) - int(iR); }


template <> inline int sCompare_T(const int& iL, const int& iR)
	{ return iL - iR; }

template <> inline int sCompare_T(const unsigned int& iL, const unsigned int& iR)
	{ return int(iL) - int(iR); }


template <> inline int sCompare_T(const long& iL, const long& iR)
	{ return iL - iR; }

template <> inline int sCompare_T(const unsigned long& iL, const unsigned long& iR)
	{ return int(iL) - int(iR); }


template <> inline int sCompare_T(const int64& iL, const int64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


template <> inline int sCompare_T(const uint64& iL, const uint64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


template <> inline int sCompare_T(const VoidStar_t& iL, const VoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <> inline int sCompare_T(const ConstVoidStar_t& iL, const ConstVoidStar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


// sCompare_T using string's compare function.
template <> inline int sCompare_T(const std::string& iL, const std::string& iR)
	{ return iL.compare(iR); }

// sCompare_T defined out-of-line in ZCompare.cpp
template <> int sCompare_T(const ZRectPOD& iL, const ZRectPOD& iR);
template <> int sCompare_T(const ZPointPOD& iL, const ZPointPOD& iR);
template <> int sCompare_T(const float& iL, const float& iR);
template <> int sCompare_T(const double& iL, const double& iR);

NAMESPACE_ZOOLIB_END

#endif // __ZCompare__
