/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZooLib_Apple_Chan_UTF_NSString_h__
#define __ZooLib_Apple_Chan_UTF_NSString_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZP.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include <Foundation/NSString.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_NSString

class ChanR_UTF_NSString : public ChanR_UTF
	{
public:
	ChanR_UTF_NSString(NSString* iString);
	~ChanR_UTF_NSString();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	ZP<NSString> fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_NSString

class ChanW_UTF_NSString : public ChanW_UTF_Native16
	{
public:
	ChanW_UTF_NSString(NSMutableString* ioString);
	~ChanW_UTF_NSString();

// From ChanW_UTF_Native16
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU);

private:
	ZP<NSMutableString> fString;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_Chan_UTF_NSString_h__
