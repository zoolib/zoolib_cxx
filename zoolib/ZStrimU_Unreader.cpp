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

#include "zoolib/ZStrimU_Unreader.h"

using std::min;
using std::nothrow;
using std::range_error;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_Unreader

ZStrimU_Unreader::ZStrimU_Unreader(const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource)
	{}

void ZStrimU_Unreader::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDest < localDestEnd)
		{
		if (fStack.empty())
			{
			size_t countRead;
			fStrimSource.Read(localDest, localDestEnd - localDest, &countRead);
			if (countRead == 0)
				break;
			localDest += countRead;
			}
		else
			{
			*localDest++ = fStack.back();
			fStack.pop_back();
			}
		}

	if (oCount)
		*oCount = localDest - oDest;
	}

bool ZStrimU_Unreader::Imp_ReadCP(UTF32& oCP)
	{
	if (fStack.empty())
		return fStrimSource.ReadCP(oCP);

	oCP = fStack.back();
	fStack.pop_back();
	return true;
	}

void ZStrimU_Unreader::Imp_Unread(UTF32 iCP)
	{ fStack.push_back(iCP); }

size_t ZStrimU_Unreader::Imp_UnreadableLimit()
	{ return size_t(-1); }

NAMESPACE_ZOOLIB_END
