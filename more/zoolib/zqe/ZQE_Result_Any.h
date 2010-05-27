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

#ifndef __ZQE_Result_Any__
#define __ZQE_Result_Any__ 1
#include "zconfig.h"

#include "zoolib/ZVal_Any.h"
#include "zoolib/zqe/ZQE_Result.h"

#include <set>

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Result

class Result_Any : public Result
	{
public:
	Result_Any(const ZVal_Any& iVal);
	Result_Any(const ZVal_Any& iVal, ZRef<Result_Any> iOther);

// From Result
	virtual bool SameAs(ZRef<Result> iOther);
	virtual ZRef<Result> ProductWith(ZRef<Result> iOther);

// Our protocol
	const ZVal_Any& GetVal();
	
	void AddAnnotation(ZRef<ZCounted> iCounted);
	void AddAnnotations(const std::set<ZRef<ZCounted> >& iAnnotations);
	const std::set<ZRef<ZCounted> >& GetAnnotations();

private:
	const ZVal_Any fVal;
	std::set<ZRef<ZCounted> > fAnnotations;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Result_Val_Any__
