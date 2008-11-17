/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZCompat_npapi__
#define __ZCompat_npapi__
#include "zconfig.h"

// Pull in ZTypes so we see its int32 definition
#include "zoolib/ZTypes.h"

// And tell the npXXX headers that we have an int32 definition
#define _INT32 1

#if defined(ZProjectHeader_npapi)
#	include ZProjectHeader_npapi
#elif defined(__APPLE__)
#	include <WebKit/npapi.h>
#else
#	include <npapi.h>
#endif

#if defined(ZProjectHeader_npfunctions)
#	include ZProjectHeader_npfunctions
#elif defined(__APPLE__)
#	include <WebKit/npfunctions.h>
#else
#	include <npfunctions.h>
#endif

#if defined(ZProjectHeader_npruntime)
#	include ZProjectHeader_npruntime
#elif defined(__APPLE__)
#	include <WebKit/npruntime.h>
#else
#	include <npruntime.h>
#endif



#endif // __ZCompat_npapi__
