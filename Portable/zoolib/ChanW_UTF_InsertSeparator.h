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

#ifndef __ChanW_UTF_InsertSeparator_h__
#define __ChanW_UTF_InsertSeparator_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include <map>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_UTF_InsertSeparator

class ChanW_UTF_InsertSeparator
:	public ChanW_UTF_Native32
	{
public:
	typedef std::map<size_t,string8> Spacings;

	ChanW_UTF_InsertSeparator(size_t iSpacing, const string8& iSeparator, const ChanW_UTF& iChanW);

	ChanW_UTF_InsertSeparator(const Spacings& iSpacings, const ChanW_UTF& iChanW);
	
// From ChanW<UTF32> via ChanW_UTF_Native32
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

private:
	const ChanW_UTF& fChanW;
	Spacings fSpacings;	
	uint64 fCount;
	};

} // namespace ZooLib

#endif // __ChanW_UTF_InsertSeparator_h__
