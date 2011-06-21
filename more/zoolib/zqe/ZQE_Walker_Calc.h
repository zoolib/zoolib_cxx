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

#ifndef __ZQE_Walker_Calc__
#define __ZQE_Walker_Calc__ 1
#include "zconfig.h"

#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_RelHead.h"
#include "zoolib/zqe/ZQE_Walker.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Calc

class Walker_Calc : public Walker_Unary
	{
public:
	typedef ZRA::Expr_Rel_Calc::Callable Callable;
	typedef ZRA::Expr_Rel_Calc::PseudoMap PseudoMap;

	Walker_Calc(const ZRef<Walker>& iWalker,
		const string8& iRelName,
		const ZRef<Callable>& iCallable);

	virtual ~Walker_Calc();

// From ZQE::Walker
	virtual ZRef<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	const string8 fRelName;
	const ZRef<Callable> fCallable;
	size_t fOutputOffset;
	std::map<string8,size_t> fBindings;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Walker_Calc__
