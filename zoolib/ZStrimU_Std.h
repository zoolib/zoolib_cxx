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

#ifndef __ZStrimU_Std__
#define __ZStrimU_Std__ 1
#include "zconfig.h"

#include "zoolib/ZStrim_CRLF.h" // For ZStrimR_CRLFRemove
#include "zoolib/ZStrim_Stream.h" // For ZStrimR_StreamDecoder

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_Std

class ZStrimU_Std : public ZStrimU
	{
public:
	ZStrimU_Std(ZTextDecoder* iDecoder, const ZStreamR& iStreamR);

// From ZStrimR via ZStrimU 
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

// From ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();

// Our protocol
	void SetDecoder(ZTextDecoder* iDecoder);
	ZTextDecoder* SetDecoderReturnOld(ZTextDecoder* iDecoder);

	size_t GetLineCount();

private:
	ZStrimR_StreamDecoder fStrimR_StreamDecoder;
	ZStrimR_CRLFRemove fStrimR_CRLFRemove;
	std::vector<UTF32> fStack;
	size_t fLineCount;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStrimU_Std__
