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

#ifndef __ZStrimU_Unreader__
#define __ZStrimU_Unreader__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_Unreader

class ZStrimU_Unreader : public ZStrimU
	{
public:
	ZStrimU_Unreader(const ZStrimR& iStrimSource);

// From ZStrimR via ZStrimU 
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

	virtual bool Imp_ReadCP(UTF32& oCP);

// From ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();

private:
	const ZStrimR& fStrimSource;
	std::vector<UTF32> fStack;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStrimU_Unreader__
