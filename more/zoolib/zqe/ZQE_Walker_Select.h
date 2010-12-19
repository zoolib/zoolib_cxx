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

#ifndef __ZQE_Walker_Select__
#define __ZQE_Walker_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Bool.h"
#include "zoolib/zqe/ZQE_Walker.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Select

class Walker_Select : public Walker_Unary
	{
public:
	Walker_Select(ZRef<Walker> iWalker, ZRef<ZExpr_Bool> iExpr_Bool);
	virtual ~Walker_Select();

// From ZQE::Walker
	virtual void Prime(const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	ZRef<ZExpr_Bool> fExpr_Bool;
	std::map<string8,size_t> fBindingOffsets;
	std::map<string8,size_t> fChildOffsets;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Walker_Select__
