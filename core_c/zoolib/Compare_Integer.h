/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_Compare_Integer_h__
#define __ZooLib_Compare_Integer_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"

namespace ZooLib {

template <>
inline int sCompare_T(const bool& iL, const bool& iR)
	{ return int(iL) - int(iR); }


template <>
inline int sCompare_T(const char& iL, const char& iR)
	{ return iL - iR; }

template <>
inline int sCompare_T(const unsigned char& iL, const unsigned char& iR)
	{ return int(iL) - int(iR); }

template <>
inline int sCompare_T(const signed char& iL, const signed char& iR)
	{ return iL - iR; }


template <>
inline int sCompare_T(const __wchar_t& iL, const __wchar_t& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


template <>
inline int sCompare_T(const short& iL, const short& iR)
	{ return iL - iR; }

template <>
inline int sCompare_T(const unsigned short& iL, const unsigned short& iR)
	{ return int(iL) - int(iR); }


template <>
inline int sCompare_T(const int& iL, const int& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const unsigned int& iL, const unsigned int& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


template <>
inline int sCompare_T(const long& iL, const long& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const unsigned long& iL, const unsigned long& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }


template <>
inline int sCompare_T(const __int64& iL, const __int64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

template <>
inline int sCompare_T(const __uint64& iL, const __uint64& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

} // namespace ZooLib

#endif // __ZooLib_Compare_Integer_h__
