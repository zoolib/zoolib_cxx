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
#ifndef __ZCompat_MSVCStaticLib__
#define __ZCompat_MSVCStaticLib__ 1
#include "zconfig.h"

/**
\file
\sa ZFunctionChain.h
\section Background

Static variables in a translation unit are initialized before any regular code in the
translation unit executes. In practice the initialization happens when the containing
executable or dynamic library is loaded. When your \c main() is called, or your call to
<code>LoadLibrary()/dlopen()</code> completes, any static variables will have been initialized.

\section TheProblem The Problem
As described by <a href="http://msdn.microsoft.com/en-us/library/5tkz6s71(v=VS.80).aspx">MSDN</a>:
	\par
	<em>Constructors and assignment by global function or static methods in the declaration do
	not create a reference and will not prevent /OPT:REF elimination. Side effects from such
	code should not be depended on when no other references to the data exist.</em>

It can be convenient to place the object code from multiple translation units in a single
file, a static library conventionally named with a \c .lib or \c .a suffix. The MSVC linker does
dependency analysis on static libraries and will not include code that is not referenced
by the including entity.

The common pattern of using a static variable to declare and cause the registration of
a factory object can fail in this circumstance -- the MSVC linker deems the static as
being unreachable and strips it from the result.

\section Solutions
A useful google search: http://www.google.com/search?q=msvc+factory+static+library

One solution is to set the <code>/OPT:NOREF</code> linker flag on the including entity. However,
this is an all or nothing setting, and will require that all included libraries be fully linkable.

If something in the file containing the static is referenced (directly or indirectly) by
the including entity, then by the language rules the static itself must be preserved.

The most basic approach is to put a dummy function in the file, and reference that from
somewhere known to be considered reachable.

Another approach is to use the \c /INCLUDE linker flag to reference an entity in the problem file.
Assuming an entity named \c DummyForLinkProblem, this can be done in the including entity's source:
\code #pragma comment(linker, "/include:DummyForLinkProblem") \endcode

\section ZooLibSolution ZooLib's Solution
ZooLib entities currently affected by this problem are those in
ZFile_Win.cpp, ZGRgnRep_HRGN.cpp, ZNet_Internet_WinSock.cpp, ZStreamRWCon_SSL_Win.cpp,
ZTextCoder_Win.cpp and ZUnicode_Normalize_Win.cpp.

We \#include ZCompat_MSVCStaticLib.h in the corresponding header files, and put in each a
\c ZMACRO_MSVCStaticLib_Reference(ModifiedFileName). In the cpp files we put a
\c ZMACRO_MSVCStaticLib_cpp(ModifiedFileName). The ModifiedFileName is generally the
filename with the leading Z and file extension removed, the same style as
used in \c ZCONFIG_API_XXX macros.

To ensure that your executable or library does not strip these entities, simply \#include
the appropriate header file from known referenced code in your including entity. This will
cause a non-executing reference to occur, and things will work as expected.
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZCompat_MSVCStaticLib

#if ZCONFIG(Compiler, MSVC)

#define ZMACRO_MSVCStaticLib_Reference(a) \
	namespace ZooLib { \
	namespace MSVCStaticLib { \
	namespace a { \
	extern const int DummyInteger; \
	namespace /* anonymous */ { \
	int ReferenceDummyInteger(); \
	int ReferenceDummyInteger() { return DummyInteger; } \
	} /* anonymous namespace*/ \
	} /* namespace a */ \
	} /* namespace MSVCStaticLib */ \
	} /* namespace ZooLib */

#define ZMACRO_MSVCStaticLib_cpp(a) \
	namespace ZooLib { \
	namespace MSVCStaticLib { \
	namespace a { \
	const int DummyInteger = 0; \
	} /* namespace a */ \
	} /* namespace MSVCStaticLib */ \
	} /* namespace ZooLib */

#endif // ZCONFIG(Compiler, MSVC)

#if !defined(ZMACRO_MSVCStaticLib_Reference)
	#define ZMACRO_MSVCStaticLib_Reference(a)
#endif

#if !defined(ZMACRO_MSVCStaticLib_cpp)
	#define ZMACRO_MSVCStaticLib_cpp(a)
#endif

#endif // __ZCompat_MSVCStaticLib__
