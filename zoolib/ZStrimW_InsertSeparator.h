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

#include <map>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimW_InsertSeparator

class ZStrimW_InsertSeparator
:	public ZStrimW_NativeUTF32
	{
public:
	typedef std::map<size_t,string8> Spacings;

	ZStrimW_InsertSeparator(size_t iSpacing, const string8& iSeparator, const ZStrimW& iStrimSink);

	ZStrimW_InsertSeparator(const Spacings& iSpacings, const ZStrimW& iStrimSink);
	
// From ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);

private:
	const ZStrimW& fStrimSink;
	Spacings fSpacings;	
	uint64 fCount;
	};

} // namespace ZooLib

#endif // __ZStrimW_InsertSeparator_h__
