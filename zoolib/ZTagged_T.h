/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZTagged_T__
#define __ZTagged_T__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTagged_T

template <class Tag, class Value>
class ZTagged_T
	{
	Value fValue;
public:
	ZTagged_T() {}
	ZTagged_T(const ZTagged_T& iOther) : fValue(iOther.fValue) {}
	~ZTagged_T() {}
	ZTagged_T& operator=(const ZTagged_T& iOther) { fValue = iOther.fValue; return *this; }

	ZTagged_T(const Value& iValue) : fValue(iValue) {}
	ZTagged_T& operator=(const Value& iValue) { fValue = iValue; return *this; }

	const Value& Get() const { return fValue; }
	};

} // namespace ZooLib

#endif // __ZTagged_T__
