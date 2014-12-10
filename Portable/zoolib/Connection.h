/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Connection_h__
#define __ZooLib_Connection_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Channer_Bin.h"

namespace ZooLib {

// ---

typedef ChannerComboRW<byte> ChannerComboRW_Bin;

//typedef Callable<ZQ<ChannerComboRW_Bin>()> ChannerComboFactoryRW_Bin;
typedef Callable<ZQ<ChannerComboRW_Bin>()> Factory_ChannerComboRW_Bin;

// ---

typedef ChannerComboRWClose<byte> ChannerComboRWClose_Bin;

//typedef Callable<ZQ<ChannerComboRWClose_Bin>()> ChannerComboFactoryRWClose_Bin;
typedef Callable<ZQ<ChannerComboRWClose_Bin>()> Factory_ChannerComboRWClose_Bin;

// ---

} // namespace ZooLib

#endif // __ZooLib_Chan_Channer_h__
