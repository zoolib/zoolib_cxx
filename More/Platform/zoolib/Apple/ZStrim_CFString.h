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

#ifndef __ZStrim_CFString_h__
#define __ZStrim_CFString_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZRef.h"
#include "zoolib/ZStrim.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include <CoreFoundation/CFString.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimR_CFString

class ZStrimR_CFString : public ZStrimR
	{
public:
	ZStrimR_CFString(CFStringRef iStringRef);
	~ZStrimR_CFString();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual bool Imp_ReadCP(UTF32& oCP);

private:
	ZRef<CFStringRef> fStringRef;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ZStrimW_CFString

class ZStrimW_CFString : public ZStrimW_NativeUTF16
	{
public:
	ZStrimW_CFString(CFMutableStringRef iStringRef);
	~ZStrimW_CFString();

// From ZStrimW via ZStrimW_NativeUTF16
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);

private:
	ZRef<CFMutableStringRef> fStringRef;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZStrim_CFString_h__
