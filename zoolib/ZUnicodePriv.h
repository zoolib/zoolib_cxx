/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUnicodePriv__
#define __ZUnicodePriv__ 1
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Unicode string operations

/// Prepend a UTF32 code point to a UTF16 standard library string.
string16 operator+(UTF32 iCP, const string16& iString);

/// Append a UTF32 code point to a UTF16 standard library string.
string16& operator+=(string16& ioString, UTF32 iCP);

/// Append a UTF32 code point to a UTF16 standard library string.
inline string16 operator+(const string16& iString, UTF32 iCP)
	{
	string16 temp = iString;
	return temp += iCP;
	}


/// Prepend a UTF32 code point to a UTF8 standard library string.
string8 operator+(UTF32 iCP, const string8& iString);

/// Append a UTF32 code point to a UTF8 standard library string.
string8& operator+=(string8& ioString, UTF32 iCP);

/// Append a UTF32 code point to a UTF8 standard library string.
inline string8 operator+(const string8& iString, UTF32 iCP)
	{
	string8 temp = iString;
	return temp += iCP;
	}

namespace ZUnicode {

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode::Functions_CountCU

// Functions_CountCU::sCountCU will call through to the standard
// string length functions for pointers, and for other iterator
// types there's a definition in ZUnicodePrivB.h and explicit
// instantiations in ZUnicode.cpp

template <class I>
struct Functions_CountCU_T
	{
	static size_t sCountCU(I iSource);
	};

template <>
struct Functions_CountCU_T<const UTF32*>
	{
	static size_t sCountCU(const UTF32* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF32>::length(iString);
		#else
			return std::char_traits<UTF32>::length(iString);
		#endif
		}
	};

template <>
struct Functions_CountCU_T<const UTF16*>
	{
	static size_t sCountCU(const UTF16* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF16>::length(iString);
		#else
			return std::char_traits<UTF16>::length(iString);
		#endif
		}
	};

template <>
struct Functions_CountCU_T<const UTF8*>
	{
	static size_t sCountCU(const UTF8* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF8>::length(iString);
		#else
			return std::char_traits<UTF8>::length(iString);
		#endif
		}
	};

template <>
struct Functions_CountCU_T<UTF32*>
	{
	static size_t sCountCU(UTF32* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF32>::length(iString);
		#else
			return std::char_traits<UTF32>::length(iString);
		#endif
		}
	};

template <>
struct Functions_CountCU_T<UTF16*>
	{
	static size_t sCountCU(UTF16* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF16>::length(iString);
		#else
			return std::char_traits<UTF16>::length(iString);
		#endif
		}
	};

template <>
struct Functions_CountCU_T<UTF8*>
	{
	static size_t sCountCU(UTF8* iString)
		{
		#if defined(__GNUC__) && __GNUC__ == 2 && __GNUC_MINOR__ <= 9
			return std::string_char_traits<UTF8>::length(iString);
		#else
			return std::char_traits<UTF8>::length(iString);
		#endif
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode::Functions_Count

template <class I>
struct Functions_Count_T
	{
	static size_t sCountCP(I iSource);
	static void sCount(I iSource, size_t* oCountCU, size_t* oCountCP);

	static size_t sCPToCU(I iSource, size_t iCountCP);
	static size_t sCPToCU(I iSource, size_t iCountCU, size_t iCountCP, size_t* oCountCP);
	static size_t sCPToCU(I iSource, I iEnd, size_t iCountCP, size_t* oCountCP);

	static size_t sCUToCP(I iSource, size_t iCountCU);
	static size_t sCUToCP(I iSource, I iEnd);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode::Functions_Read

template <class I, typename C = typename std::iterator_traits<I>::value_type>
struct Functions_Read_T
	{
	static void sAlign(I& ioCurrent);
	static void sAlign(I& ioCurrent, I iEnd);

	static void sInc(I& ioCurrent);
	static bool sInc(I& ioCurrent, I iEnd);

	static void sDec(I& ioCurrent);
	static bool sDec(I iStart, I& ioCurrent, I iEnd);

	static UTF32 sRead(I iCurrent);
	static bool sRead(I iCurrent, I iEnd, UTF32& oCP);

	static UTF32 sReadInc(I& ioCurrent);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped);

	static UTF32 sDecRead(I& ioCurrent);
	static bool sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode::Functions_Write

template <class I, typename C = typename std::iterator_traits<I>::value_type>
struct Functions_Write_T
	{
	static bool sWrite(I oDest, I iEnd, UTF32 iCP);
	static bool sWriteInc(I& ioDest, I iEnd, UTF32 iCP);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUnicode::Functions_Convert

template <class I>
struct Functions_Convert_T
	{
	static string32 sAsUTF32(I iSource);
	static string32 sAsUTF32(I iSource, size_t iCountCU);
	static string32 sAsUTF32(I iSource, I iEnd);

	static string16 sAsUTF16(I iSource);
	static string16 sAsUTF16(I iSource, size_t iCountCU);
	static string16 sAsUTF16(I iSource, I iEnd);

	static string8 sAsUTF8(I iSource);
	static string8 sAsUTF8(I iSource, size_t iCountCU);
	static string8 sAsUTF8(I iSource, I iEnd);
	};

} // namespace ZUnicode

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * GCC workaround

#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ >= 2 && __GNUC_MINOR__ < 4
// AG 2002-10-07. Workaround for the gcc 3.2+ STL's lack of a definition
// for char_traits<X> where X is neither char nor wchar_t.

template <class charT>
inline void std::char_traits<charT>::assign(char_type& c1, const char_type& c2)
	{
	c1 = c2;
	}

template <class charT>
inline bool std::char_traits<charT>::eq(const char_type& c1, const char_type& c2)
	{
	return c1 == c2;
	}

template <class charT>
inline bool std::char_traits<charT>::lt(const char_type& c1, const char_type& c2)
	{ 
	return c1 < c2;
	}

template <class charT>
int std::char_traits<charT>::compare(const char_type* s1, const char_type* s2, size_t n)
	{
	for (size_t i = 0; i < n; ++i, ++s1, ++s2)
		{
		if (lt(*s1, *s2))
			return -1;
		if (lt(*s2, *s1))
			return 1;
		}
	return 0;
	}

template <class charT>
size_t std::char_traits<charT>::length(const char_type* s)
	{
	size_t len = 0;
	while (!eq(*s, charT()))
		{
		++s;
		++len;
		}
	return len;
	}

template <class charT>
const typename std::char_traits<charT>::char_type*
std::char_traits<charT>::find(const char_type* s, size_t n, const char_type& a)
	{
	if (n == 0)
		return 0;
	const char_type* p = s;
	size_t i = 0;
	while (!eq(*p, a))
		{
		++i;
		if (i == n)
			return 0;
		++p;
		}
	return p;
	}

template <class charT>
typename std::char_traits<charT>::char_type*
std::char_traits<charT>::move(char_type* s1, const char_type* s2, size_t n)
	{
	char_type* r = s1;
	if (s1 < s2)
		{
		for (size_t i = 0; i < n; ++i)
			assign(*s1++, *s2++);
		}
	else if (s2 < s1)
		{
		s1 += n;
		s2 += n;
		for (; n > 0; --n)
			assign(*--s1, *--s2);
		}
	return r;
	}

template <class charT>
inline typename std::char_traits<charT>::char_type*
std::char_traits<charT>::copy(char_type* s1, const char_type* s2, size_t n)
	{
	char_type* r = s1;
	for (size_t i = 0; i < n; ++i)
		assign(*s1++, *s2++);
	return r;
	}

template <class charT>
inline typename std::char_traits<charT>::char_type*
std::char_traits<charT>::assign(char_type* s, size_t n, char_type a)
	{
	char_type* r = s;
	for (size_t i = 0; i < n; ++i)
		assign(*s++, a);
	return r;
	}

template <class charT>
inline typename std::char_traits<charT>::int_type
std::char_traits<charT>::not_eof(const int_type& c)
	{
	return eq_int_type(c, eof()) ? ~eof() : c;
	}

template <class charT>
inline typename std::char_traits<charT>::char_type
std::char_traits<charT>::to_char_type(const int_type& c)
	{
	return charT(c);
	}

template <class charT>
inline typename std::char_traits<charT>::int_type
std::char_traits<charT>::to_int_type(const char_type& c)
	{
	return int_type(c);
	}

template <class charT>
inline bool std::char_traits<charT>::eq_int_type(const int_type& c1, const int_type& c2)
	{
	return c1 == c2;
	}

template <class charT>
inline typename std::char_traits<charT>::int_type std::char_traits<charT>::eof()
	{
	return EOF;
	}

#endif // defined(__GNUC__) && __GNUC__ >= 3 && __GNUC_MINOR__ >= 2

#endif // __ZUnicodePriv__
