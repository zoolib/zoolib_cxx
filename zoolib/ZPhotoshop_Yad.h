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

#ifndef __ZPhotoshop_Yad__
#define __ZPhotoshop_Yad__ 1
#include "zconfig.h"

#include "zoolib/ZPhotoshop_Val.h"
#include "zoolib/ZYad.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZPhotoshop {

// =================================================================================================
#pragma mark -
#pragma mark * YadR

class YadR : public virtual ZYadR
	{
public:
	YadR(const Val& iVal);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	const Val& GetVal();

private:
	const Val fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop::MakeYadR

ZRef<ZYadR> sMakeYadR(const Val& iVal);

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END

#endif // __ZPhotoshop_Yad__
