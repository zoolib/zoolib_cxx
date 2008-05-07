/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZMemory__
#define __ZMemory__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark -
#pragma mark * BlockMove, BlockCopy, BlockSet wrappers

// ZBlockMove, ZBlockCopy and ZBlockSet -- standardizes the API and takes advantage of any native
// performance enhacnements that may be available. Note that the parameter order is reversed from
// the C standard API. This is because these methods started off as simple wrappers around MacOS
// procedures which also had what might to you seem to be awkward ordering. Sorry.

#include <string.h> // This is needed for memmove, memcpy and memset

#if ZCONFIG(OS, POSIX)
#	include <strings.h> // And this for bzero
#endif

#define ZBlockMove(srcPtr, destPtr, byteCount) ::memmove((destPtr), (srcPtr), (size_t)(byteCount))
#define ZBlockCopy(srcPtr, destPtr, byteCount) ::memcpy((destPtr), (srcPtr), (size_t)(byteCount))

#define ZBlockSet(destPtr, size, value) ::memset((destPtr), (int)(value), (size_t)(size))

#if ZCONFIG(OS, POSIX)
#	define ZBlockZero(destPtr, size) ::bzero((destPtr), (size_t)(size))
#else
#	define ZBlockZero(destPtr, size) ::memset((destPtr), 0, (size_t)(size))
#endif

// =================================================================================================

#endif // __ZMemory__
