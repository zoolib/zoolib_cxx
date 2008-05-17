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

#ifndef __zconfigd__
#define __zconfigd__ 1

// Which compiler -- ZCONFIG_Compiler
#define ZCONFIG_Compiler_CodeWarrior 1
#define ZCONFIG_Compiler_MSVC 2
#define ZCONFIG_Compiler_GCC 4

// Which processor -- ZCONFIG_Processor
#define ZCONFIG_Processor_68K 1
#define ZCONFIG_Processor_PPC 2
#define ZCONFIG_Processor_x86 4
#define ZCONFIG_Processor_Sparc 8
#define ZCONFIG_Processor_Alpha 16

// Byte order -- ZCONFIG_Endian
#define ZCONFIG_Endian_Big 1
#define ZCONFIG_Endian_Little 2

// Debugging
// ZCONFIG_Debug can take on any value, but by convention use 0 for a non-debug build, 1 for light debugging, 2 for heavy.
// Note that ZCONFIG_Debug is what's _used_ in assert macros, but it takes its actual value from ZCONFIG_DebugLevel if
// we're doing a debug build (.sym file being generated on CW, DEBUG defined on gcc). You should therefore define
// ZCONFIG_DebugLevel in your zconfig.h file if it needs to be different from the default value of 1.

#endif // __zconfigd__
