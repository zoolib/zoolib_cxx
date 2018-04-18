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

#ifndef __ZooLib_Apple_Chan_Bin_CFStream_h__
#define __ZooLib_Apple_Chan_Bin_CFStream_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include <CoreFoundation/CFStream.h>

#include "zoolib/Apple/ZRef_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_CFStream

class ChanR_Bin_CFStream
:	public ChanR_Bin
	{
public:
	ChanR_Bin_CFStream(CFReadStreamRef iCFStream);
	~ChanR_Bin_CFStream();

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t CountReadable();

private:
	ZRef<CFReadStreamRef> fCFStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_CFStream

class ChanW_Bin_CFStream
:	public ChanW_Bin
	{
public:
	ChanW_Bin_CFStream(CFWriteStreamRef iCFStream);
	~ChanW_Bin_CFStream();

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);

private:
	ZRef<CFWriteStreamRef> fCFStream;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

#endif // __ZooLib_Apple_Chan_Bin_CFStream_h__
