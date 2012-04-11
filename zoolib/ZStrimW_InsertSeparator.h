/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZStrimW_InsertSeparator_h__
#define __ZStrimW_InsertSeparator_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimW_InsertSeparator

class ZStrimW_InsertSeparator
:	public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_InsertSeparator(size_t iSpacing, const string8& iSeparator, const ZStrimW& iStrimSink);

	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	const ZStrimW& fStrimSink;
	const size_t fSpacing;
	const string8 fSeparator;
	
	size_t fCount;
	};

} // namespace ZooLib

#endif // __ZStrimW_InsertSeparator_h__
