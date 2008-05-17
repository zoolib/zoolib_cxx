/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTextCoder__
#define __ZTextCoder__ 1
#include "zconfig.h"

#include "ZUnicode.h"
#include <vector>

class ZStreamR;
class ZStreamW;

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCoder

namespace ZTextCoder {

void sGetAliases(const std::string& iName, std::vector<std::string>& oAliases);

} // namespace ZTextCoder

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder

class ZTextDecoder
	{
protected:
	ZTextDecoder() {}
	ZTextDecoder(const ZTextDecoder&) {}
	ZTextDecoder& operator=(const ZTextDecoder&) { return *this; }

public:
	virtual ~ZTextDecoder() {}

	static ZTextDecoder* sMake(const std::string& iCharset);

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* iDest, size_t iDestCU, size_t* oDestCU) = 0;

	virtual void Decode(const ZStreamR& iStreamR, UTF32* iDest, size_t iDestCU, size_t* oDestCU);

	virtual bool Decode(const ZStreamR& iStreamR, UTF32& oCP);

	virtual void Reset();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder

class ZTextEncoder
	{
protected:
	ZTextEncoder() {}
	ZTextEncoder(const ZTextEncoder&) {}
	ZTextEncoder& operator=(const ZTextEncoder&) { return *this; }

public:
	virtual ~ZTextEncoder() {}

	static ZTextEncoder* sMake(const std::string& iCharset);

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* iDest, size_t iDestBytes, size_t* oDestBytes) = 0;

	virtual void Encode(const ZStreamW& iStreamW,
		const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU);

	virtual void Encode(const ZStreamW& iStreamW, UTF32 iCP);

	virtual void Reset();
	};

#endif // __ZTextCoder__
