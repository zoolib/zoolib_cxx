/* ------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZCompat_operator_bool__
#define __ZCompat_operator_bool__
#include "zconfig.h"
#include "ZCONFIG_SPI.h"

// If stlsoft is enabled, this file simply does a rename of the
// stlsoft operator bool macros. Otherwise it provides the implementation
// of those macros, simplified somewhat but still usable by CW and GCC.

// For discussions of the problems with operator bool (and our previously
// preferred operator const void*) see:
// * <http://www.artima.com/cppsource/safeboolP.html>
// * <http://synesis.com.au/software/stlsoft/help/stlsoft__operator__bool_8h.html>
// * Chapter 24 of "Imperfect C++" by Matthew Wilson (ISBN 0-321-22877-4).
// * Section 7.8 of "Modern C++ Design" by Andrei Alexandrescu, (ISBN 0-201-70431-5).
//   (Available online at
//   <http://www.awprofessional.com/articles/article.asp?p=31529&seqNum=8&rl=1>)

#if ZCONFIG_SPI_Enabled(stlsoft)

#include "stlsoft_operator_bool.h"

#define ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(U, G, B) STLSOFT_DEFINE_OPERATOR_BOOL_TYPES(U, G, B)
#define ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(U, G, B) STLSOFT_DEFINE_OPERATOR_BOOL_TYPES_T(U, G, B)

#else

/* /////////////////////////////////////////////////////////////////////////////
 * File:        stlsoft_operator_bool.h
 *
 * Purpose:     A robust and portable operator bool generator class.
 *
 * Created:     5th November 2003
 * Updated:     17th July 2005
 *
 * Home:        http://stlsoft.org/
 *
 * Copyright (c) 2003-2005, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the names of
 *   any contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////////// */

#define ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(U, G, B) \
    typedef operator_bool_generator<U>::class_type G; \
    typedef G::return_type B

#define ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(U, G, B) \
    typedef typename operator_bool_generator<U>::class_type G; \
    typedef typename G::return_type B

template <typename T>
struct operator_bool_generator
	{
public:
	typedef operator_bool_generator<T> class_type;
	typedef int class_type::*return_type;

	/// Returns the value representing the true condition
	static return_type true_value()
		{ return &class_type::i; }

private:
	int i;
public:
	/// Returns the value representing the false condition
	static return_type false_value()
		{ return static_cast<return_type>(0); }

	/// Does the ternary operator for you
	template <typename U>
	static return_type translate(U b)
		{ return b ? true_value() : false_value(); }

private:
	void operator delete(void*);
	};

#endif

#endif // __ZCompat_operator_bool__
