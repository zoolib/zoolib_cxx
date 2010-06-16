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

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"

// Switch off optimization for CodeWarrior -- it runs out of registers.
#if ZCONFIG(Compiler, CodeWarrior)
#	pragma push
#	pragma optimization_level 1
#	pragma opt_common_subs off
#	pragma opt_lifetimes off
#	pragma peephole off
#	if ZCONFIG(Processor, x86)
#		pragma register_coloring off
#	endif
#endif

namespace ZooLib {

void ZByteSwap_Test();
void ZByteSwap_Test()
	{
	int16 value16 = 0x1234;
	int32 value32 = 0x12345678;
	int64 value64 = 0x123456789ABCDEF0LL;

	ZAssertStop(0, 0x3412 == ZByteSwap_16(value16));
	ZAssertStop(0, 0x78563412 == ZByteSwap_32(value32));
	ZAssertStop(0, 0xF0DEBC9A78563412LL == ZByteSwap_64(value64));

	ZAssertStop(0, 0x3412 == ZByteSwap_Read16(&value16));
	ZAssertStop(0, 0x78563412 == ZByteSwap_Read32(&value32));
	ZAssertStop(0, 0xF0DEBC9A78563412LL == ZByteSwap_Read64(&value64));

	ZByteSwap_16(&value16);
	ZAssertStop(0, 0x3412 == value16);

	ZByteSwap_32(&value32);
	ZAssertStop(0, 0x78563412 == value32);

	ZByteSwap_64(&value64);
	ZAssertStop(0, 0xF0DEBC9A78563412LL == value64);

	ZByteSwap_Write16(&value16, 0x1234);
	ZAssertStop(0, 0x3412 == value16);

	ZByteSwap_Write32(&value32, 0x12345678);
	ZAssertStop(0, 0x78563412 == value32);

	ZByteSwap_Write64(&value64, 0x123456789ABCDEF0LL);
	ZAssertStop(0, 0xF0DEBC9A78563412LL == value64);
	}

} // namespace ZooLib
