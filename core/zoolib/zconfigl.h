/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __zconfigl_h__
#define __zconfigl_h__ 1

// Ensure our definitions have been set up
#include "zoolib/zconfigd.h"

// =================================================================================================
#define ZCONFIG(a, b)\
	((ZCONFIG_##a##_##b) && (((ZCONFIG_##a) & (ZCONFIG_##a##_##b))==(ZCONFIG_##a##_##b)))

// =================================================================================================
// Compiler

#ifndef ZCONFIG_Compiler
	#if 0
	#elif defined(__MWERKS__)
		#define ZCONFIG_Compiler ZCONFIG_Compiler_CodeWarrior
	#elif defined(__clang__)
		#define ZCONFIG_Compiler ZCONFIG_Compiler_Clang
	#elif defined(__GNUC__)
		#define ZCONFIG_Compiler ZCONFIG_Compiler_GCC
	#elif defined(_MSC_VER)
		#define ZCONFIG_Compiler ZCONFIG_Compiler_MSVC
	#endif
#endif

#ifndef ZCONFIG_Compiler
	#error "Don't know what compiler we're using."
#endif

// =================================================================================================
// Which processor?

#ifndef ZCONFIG_Processor
	#if 0
	#elif defined(__MWERKS__)
		#if 0
		#elif __POWERPC__
			#define ZCONFIG_Processor ZCONFIG_Processor_PPC
		#elif __MC68K__
			#define ZCONFIG_Processor ZCONFIG_Processor_68K
		#elif __INTEL__
			#define ZCONFIG_Processor ZCONFIG_Processor_x86
		#endif
	#elif defined(__GNUC__) || defined(__clang__)
		#if 0
		#elif defined(__i386__)
			#define ZCONFIG_Processor ZCONFIG_Processor_x86
		#elif defined(__x86_64__)
			#define ZCONFIG_Processor ZCONFIG_Processor_x86_64
		#elif defined(__ppc__)
			#define ZCONFIG_Processor ZCONFIG_Processor_PPC
		#elif defined(__ppc64__)
			#define ZCONFIG_Processor ZCONFIG_Processor_PPC_64
		#elif defined(__arm__)
			#define ZCONFIG_Processor ZCONFIG_Processor_ARM
		#elif defined(__arm64__)
			#define ZCONFIG_Processor ZCONFIG_Processor_ARM_64
		#endif
	#elif defined(_MSC_VER)
		#if 0
		#elif defined(_M_X64)
			#define ZCONFIG_Processor ZCONFIG_Processor_x86_64
		#elif defined(_M_IX86)
			#define ZCONFIG_Processor ZCONFIG_Processor_x86
		#endif
	#endif
#endif

#ifndef ZCONFIG_Processor
	#error "Don't know what processor we're using."
#endif

// =================================================================================================
// Byte order

#ifndef ZCONFIG_Endian
	#if 0
	#elif ZCONFIG(Processor, PPC)
		#define ZCONFIG_Endian ZCONFIG_Endian_Big
	#elif ZCONFIG(Processor, PPC_64)
		#define ZCONFIG_Endian ZCONFIG_Endian_Big
	#elif ZCONFIG(Processor, 68K)
		#define ZCONFIG_Endian ZCONFIG_Endian_Big
	#elif ZCONFIG(Processor, x86)
		#define ZCONFIG_Endian ZCONFIG_Endian_Little
	#elif ZCONFIG(Processor, x86_64)
		#define ZCONFIG_Endian ZCONFIG_Endian_Little
	#elif ZCONFIG(Processor, ARM)
		#define ZCONFIG_Endian ZCONFIG_Endian_Little
	#endif
#endif

#ifndef ZCONFIG_Endian
	#error "Don't know what byte order we're using."
#endif

// =================================================================================================
// ZCONFIG_Is64Bit

#ifndef ZCONFIG_Is64Bit
	#if 0
	#elif ZCONFIG(Processor, PPC_64)
		#define ZCONFIG_Is64Bit 1
	#elif ZCONFIG(Processor, x86_64)
		#define ZCONFIG_Is64Bit 1
	#elif ZCONFIG(Processor, ARM_64)
		#define ZCONFIG_Is64Bit 1
	#endif
#endif

#ifndef ZCONFIG_Is64Bit
	#define ZCONFIG_Is64Bit 0
#endif

// =================================================================================================
// We have two different defines for debugging. ZCONFIG_DebugLevel is what type of debugging
// is wanted for a debug build (one with SYM information being generated). 0 generally means
// almost no debug code will be generated, although it's legal to have assertions that are always
// present in the code, even in a level 0 build.
// Level 1 is for lighter weight debugging -- useful for code issued to external sites, code which
// has had all the serious panic situations handled for heavily-executed code, but which may still
// have lurking problems in less busy parts of the app. Level 2 is for development. Remember, these
// are suggested values only, and the ZDebug suite of macros lets you determine whether we should
// panic and kill the app, or just fprintf(stderr, ..) and go on our way.

#ifndef ZCONFIG_DebugLevel
	#define ZCONFIG_DebugLevel 1
#endif

// The second define is ZCONFIG_Debug, which takes its value from ZCONFIG_DebugLevel if we're doing
// a SYM build, otherwise it is set to 0. You can override ZCONFIG_Debug by setting its value in
// your zconfig.h before including this file.
// With XCode we could base debug off !__OPTIMIZE__
#ifndef ZCONFIG_Debug
	#if 0
	#elif defined(DEBUG)
		#define ZCONFIG_Debug ZCONFIG_DebugLevel
	#elif defined(NDEBUG)
		#define ZCONFIG_Debug 0
	#elif defined(__MACH__)
		#if not defined(__OPTIMIZE__)
			#define ZCONFIG_Debug ZCONFIG_DebugLevel
		#else
			#define ZCONFIG_Debug 0
		#endif
	#elif defined(__MWERKS__)
		#if __option(sym)
			#define ZCONFIG_Debug ZCONFIG_DebugLevel
		#else
			#define ZCONFIG_Debug 0
		#endif
	#elif defined(_MSC_VER)
		#if defined(DEBUG) || defined(_DEBUG)
			#define ZCONFIG_Debug ZCONFIG_DebugLevel
		#else
			#define ZCONFIG_Debug 0
		#endif
	#else
		#if defined(DEBUG) || defined(_GLIBCXX_DEBUG)
			#define ZCONFIG_Debug ZCONFIG_DebugLevel
		#else
			#define ZCONFIG_Debug 0
		#endif
	#endif
#endif

#ifndef ZCONFIG_Debug
	#error "Can't figure out if we're debugging."
#endif

// =================================================================================================
// DEBUG and NDEBUG

#ifndef DEBUG
	#if !!ZCONFIG_Debug
		#define DEBUG 1
	#endif
#endif

#ifndef NDEBUG
	#if !ZCONFIG_Debug
		#define NDEBUG 1
	#endif
#endif

// =================================================================================================
// ZCONFIG_CPP

//MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
//MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
//MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
//MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
//MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
//MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
//MSVC++ 7.0  _MSC_VER == 1300
//MSVC++ 6.0  _MSC_VER == 1200
//MSVC++ 5.0  _MSC_VER == 1100

#ifndef ZCONFIG_CPP
	#if  defined(__clang__)
		#if __cplusplus >= 201103L
			#define ZCONFIG_CPP 2011
		#else
			#define ZCONFIG_CPP 2003
		#endif
	#elif defined(__GNUC__)
		#if __cplusplus >= 199711L // Go figure.
			#define ZCONFIG_CPP 2003
		#else
			#define ZCONFIG_CPP 1998
		#endif
	#elif defined(_MSC_VER)
		#if _MSC_VER >= 1600
			#define ZCONFIG_CPP 2011
		#elif _MSC_VER >= 1500
			#define ZCONFIG_CPP 1998
		#endif
	#endif
#endif

#ifndef ZCONFIG_CPP
	#define ZCONFIG_CPP 1998
#endif

// =================================================================================================
// TR1

#if  defined(__clang__)
	#include <cstddef> // To see if _LIBCPP_VERSION gets defined
	#ifdef _LIBCPP_VERSION
		#define ZMACRO_namespace_tr1_prefix std
		#define ZMACRO_namespace_tr1_begin namespace std {
		#define ZMACRO_namespace_tr1_end }
		#define ZMACRO_tr1_header(a) <a>
		#define ZMACRO_Has_tr1 1
	#else
		#define ZMACRO_namespace_tr1_prefix std::tr1
		#define ZMACRO_namespace_tr1_begin namespace std { namespace tr1 {
		#define ZMACRO_namespace_tr1_end } }
		#define ZMACRO_tr1_header(a) <tr1/a>
		#define ZMACRO_Has_tr1 1
		#define ZMACRO_Has_tr1_early 1
	#endif
#elif defined(_MSC_VER)
	#include <yvals.h> // For _HAS_TR1
	#if _MSC_VER >= 1600
		#define ZMACRO_namespace_tr1_prefix std
		#define ZMACRO_namespace_tr1_begin namespace std {
		#define ZMACRO_namespace_tr1_end }
		#define ZMACRO_tr1_header(a) <a>
		#define ZMACRO_Has_tr1 1
	#elif _MSC_VER >= 1500 && defined(_HAS_TR1) && (_HAS_TR1+0)
		#define ZMACRO_namespace_tr1_prefix std::tr1
		#define ZMACRO_namespace_tr1_begin namespace std { namespace tr1 {
		#define ZMACRO_namespace_tr1_end } }
		#define ZMACRO_tr1_header(a) <a>
		#define ZMACRO_Has_tr1 1
		#define ZMACRO_Has_tr1_early 1
	#endif
#elif defined(__GNUC__)
	#if __GNUC__ == 4 and __GNUC_MINOR__ >= 6 \
			and (defined(__GXX_EXPERIMENTAL_CXX0X__) or (__cplusplus >= 201103L))
		#define ZMACRO_namespace_tr1_prefix std
		#define ZMACRO_namespace_tr1_begin namespace std {
		#define ZMACRO_namespace_tr1_end }
		#define ZMACRO_tr1_header(a) <a>
		#define ZMACRO_Has_tr1 1
	#else
		#define ZMACRO_namespace_tr1_prefix std::tr1
		#define ZMACRO_namespace_tr1_begin namespace std { namespace tr1 {
		#define ZMACRO_namespace_tr1_end } }
		#define ZMACRO_tr1_header(a) <tr1/a>
		#define ZMACRO_Has_tr1 1
		#define ZMACRO_Has_tr1_early 1
	#endif
#endif

#ifndef ZMACRO_Has_tr1
	#define ZMACRO_Has_tr1 0
#endif

#ifndef ZMACRO_Has_tr1_early
	#define ZMACRO_Has_tr1_early 0
#endif

// =================================================================================================
// Some extra bits to patch up some CodeWarrior issues.

#if defined(__MWERKS__)

	#if __option(precompile)
		#error "Don't precompile zconfig"
// because settings on individual files will not necessarily match claimed configuration settings.
	#endif

// This definition causes some problematic math-related stuff to drop out. Seems to
// be only a CW/10.3.9 problem.
	#define __NOEXTENSIONS__

	#ifndef NEWMODE
		#define NEWMODE NEWMODE_MALLOC
	#endif

	#define MSL_USE_PRECOMPILED_HEADERS 0

	#if ZCONFIG_Debug
		// Switch off (v. nice) collapse of T* to void* in containers, so we can
		// actually work with the contents when debugging.
		#define _Inhibit_Container_Optimization
		// Switch off the placement of Red/Black flag as a single bit in the parent pointer.
		#define _Inhibit_Optimize_RB_bit
		// Enable debugging checks. This doesn't work for me yet, so I'm leaving it off.
//		#define _MSL_DEBUG
	#endif

	// Apple's headers on 10.3 define SCHAR_MAX and CHAR_BIT in terms of __SCHAR_MAX__
	// and __CHAR_BIT__, which are compiler-provided by gcc 3.3. There's a fixup for
	// SHRT, INT, LONG and LONG_LONG in /usr/include/gcc/darwin/3.3/machine/limit.h,
	// but not for these two. So we'll do it ourselves for now.
	#define __SCHAR_MAX__ 127
	#define __CHAR_BIT__ 8

#endif

// =================================================================================================
// Some extra bits to patch up some MSVC issues.

#if defined(_MSC_VER)

	#if !defined(_MT)
		#error "_MT is not defined. ZooLib requires thread safe libraries and headers."
	#endif

	// Ghastly workaround for MSVC non-compliance with C++ standard
	#if _MSC_VER < 1200
		#define for if (0) {} else for
	#endif

	// Support for operator synonyms
	#include <iso646.h>

	// "illegal pragma" -- we use pragma mark fairly extensively
	#pragma warning(disable:4068)

	// "debug info truncated to 255 characters"
	// (STL template expansion will often generate this)
	#pragma warning(disable:4786)

	// "'this' used in base member initializer list" -- we
	// know what we're doing when we do this.
	#pragma warning(disable:4355)

	// "forcing value to bool 'true' or 'false'"
	#pragma warning(disable:4800)

	// argument "conversion from 'XXX' to 'YYY', possible loss of data"
	#pragma warning(disable:4244)

	// Signed/unsigned comparison
	#pragma warning(disable:4018)

	// return "conversion from 'XXX' to 'YYY', possible loss of data"
	#pragma warning(disable:4267)

	// inherits 'XXX' via dominance
	#pragma warning(disable:4250)

	// type name first seen using 'class' now seen using 'struct'
	#pragma warning(disable:4099)

#endif

// =================================================================================================
// Previously we've used 'nil' for the null pointer. With our increasing use of
// Objective C we're switching to use the soon-to-be standardized nullptr.

#if ZCONFIG_CPP >= 2011
	// nullptr is naturally available
	#define ZCONFIG_Has_nullptr 1

#elif defined(__clang__)
	#if __has_extension(cxx_nullptr)
		// nullptr is naturally available
		#define ZCONFIG_Has_nullptr 1
	#elif defined nullptr
		// It's being faked out by stuff in cstddef.
		#define ZCONFIG_Has_nullptr 1
	#endif
#elif defined(__GXX_EXPERIMENTAL_CXX0X__)
		#define ZCONFIG_Has_nullptr 1
#endif

#ifndef ZCONFIG_Has_nullptr

	#if defined(__MWERKS__)

		class nullptr_t
			{
			void operator&() const;
		public:
			template <class T> operator T*() const { return 0; }
			};
		#define nullptr nullptr_t()

	#else

		const class nullptr_t
			{
			void operator&() const;
		public:
			template <class T> operator T*() const { return 0; }
			template <class C, class T> operator T C::*() const { return 0; }
			} nullptr = {};

	#endif

	#define ZCONFIG_Has_nullptr 1

#endif

// =================================================================================================

#if __MACH__
	#define ZMACINCLUDE2(a,b) <a/b>
	#if __MWERKS__
		#define ZMACINCLUDE3(a,b,c) <b/c>
	#else
		#define ZMACINCLUDE3(a,b,c) <a/../Frameworks/b.framework/Headers/c>
	#endif
#else
	#define ZMACINCLUDE2(a,b) <b>
	#define ZMACINCLUDE3(a,b,c) <c>
#endif

// =================================================================================================

#define ZMACRO_Concat_(x, y) x ## y
#define ZMACRO_Concat(x, y) ZMACRO_Concat_(x,y)

// =================================================================================================

#ifdef __cplusplus
	#define ZMACRO_ExternC_Begin extern "C" {
	#define ZMACRO_ExternC_End } // extern "C"
#else
	#define ZMACRO_ExternC_Begin
	#define ZMACRO_ExternC_End
#endif

// =================================================================================================

#ifndef ZMACRO_Attribute_Format_Printf
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_Attribute_Format_Printf(m,n) __attribute__((format(printf,m,n)))
	#else
		#define ZMACRO_Attribute_Format_Printf(m,n)
	#endif
#endif

#ifndef ZMACRO_Attribute_Aligned
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_Attribute_Aligned __attribute__((aligned))
	#else
		#define ZMACRO_Attribute_Aligned
	#endif
#endif

#ifndef ZMACRO_Attribute_Deprecated
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_Attribute_Deprecated __attribute__((deprecated))
	#else
		#define ZMACRO_Attribute_Deprecated
	#endif
#endif

#ifndef ZMACRO_Attribute_NoThrow
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_Attribute_NoThrow __attribute__((nothrow))
	#else
		#define ZMACRO_Attribute_NoThrow
	#endif
#endif

#ifndef ZMACRO_Attribute_Unused
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_Attribute_Unused __attribute__((unused))
	#else
		#define ZMACRO_Attribute_Unused
	#endif
#endif

#ifndef ZMACRO_NoReturn_Pre
	#if ZCONFIG(Compiler,MSVC)
		#define ZMACRO_NoReturn_Pre __declspec(noreturn)
	#else
		#define ZMACRO_NoReturn_Pre
	#endif
#endif

#ifndef ZMACRO_NoReturn_Post
	#if ZCONFIG(Compiler,GCC) || ZCONFIG(Compiler,Clang)
		#define ZMACRO_NoReturn_Post __attribute__((noreturn))
	#else
		#define ZMACRO_NoReturn_Post
	#endif
#endif

#if not defined(_MSC_VER)
	// Even today wchar_t can still be an oddball, certainly on MSVC where __wchar_t is the
	// canonical type and wchar_t can be a typedef of an int. So we define __wchar_t elsewhere
	// so we've got a type we can depend on interpreting correctly.
	typedef wchar_t __wchar_t;

	// Similarly for 64 bit ints.
	typedef long long __int64;
	typedef unsigned long long __uint64;
#else
	typedef unsigned __int64 __uint64;
#endif

#endif // __zconfigl_h__
