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

#ifndef __ZStrim_NSString__
#define __ZStrim_NSString__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZStrim.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include <Foundation/NSString.h>

#include "zoolib/ZRef.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_NSString

class ZStrimR_NSString : public ZStrimR
	{
public:
	ZStrimR_NSString(ZRef<NSString> iString);
	~ZStrimR_NSString();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

//	virtual void Imp_ReadUTF16(UTF16* oDest,
//		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

//	virtual void Imp_ReadUTF8(UTF8* oDest,
//		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual bool Imp_ReadCP(UTF32& oCP);

private:
	ZRef<NSString> fString;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_NSString

class ZStrimW_NSString : public ZStrimW_NativeUTF16
	{
public:
	ZStrimW_NSString(ZRef<NSMutableString> iString);
	~ZStrimW_NSString();

// From ZStrimW via ZStrimW_NativeUTF16
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);

private:
	ZRef<NSMutableString> fString;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZStrim_NSString__
