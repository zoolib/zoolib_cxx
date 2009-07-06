/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZMemoryBlock__
#define __ZMemoryBlock__ 1
#include "zconfig.h"

#include "zoolib/ZStream_ValData_T.h"
#include "zoolib/ZValData_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

typedef ZValData_ZooLib ZMemoryBlock;

typedef ZStreamRPos_ValData_T<ZMemoryBlock> ZStreamRPos_MemoryBlock;
typedef ZStreamRWPos_ValData_T<ZMemoryBlock> ZStreamRWPos_MemoryBlock;

NAMESPACE_ZOOLIB_END

#endif // __ZMemoryBlock__
