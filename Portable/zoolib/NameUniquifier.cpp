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

#include "zoolib/NameUniquifier.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Compare_RefCountedString

bool Compare_RefCountedString::operator()(const ZRefCountedString& l, const ZRefCountedString& r) const
	{ return l->Get() < r->Get(); }

// =================================================================================================
#pragma mark - sName

Name sName(const string8& iString)
	{ return sName(sCountedVal<string8>(iString)); }

Name sName(const ZRefCountedString& iCountedString)
	{
	if (ThreadVal_NameUniquifier::Type_t* theUniquifier = ThreadVal_NameUniquifier::sPMut())
		return Name(theUniquifier->Get(iCountedString));
	return Name(iCountedString);
	}

} // namespace ZooLib
