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

#include "zoolib/zqe/ZQE_Walker_Select.h"
#include "zoolib/ZExpr_Bool_ValPred_Any.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;

class Context : public ZValContext
	{
public:
	const ZVal_Any* fBindings;
	const ZVal_Any* fResults;
	};

#if 0

maybe just bodge it for now -- pack all of iBindings and oResults into a ZMap_Any.

class Visitor
:	public virtual ZVisitor_Expr_Bool_ValPred_T<ZVal_Any>
	{
public:
	virtual void Visit_Expr_Bool_ValPred(ZRef<ZExpr_Bool_ValPred_T<ZVal_Any> > iExpr);
	};

template <class Val>
void Visitor::Visit_Expr_Bool_ValPred(
	ZRef<ZExpr_Bool_ValPred_T<ZVal_Any> > iExpr)
	{
	
	this->Visit_Expr_Op0(iExpr);
	}

#endif

static bool spMatches(const ZVal_Any* iBindings, const ZVal_Any* iResults, ZRef<ZExpr_Bool> iExpr)
	{
	Context theContext;
	theContext.fBindings = iBindings;
	theContext.fResults = iResults;
	
	ZUnimplemented();
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Select

Walker_Select::Walker_Select(ZRef<Walker> iWalker, ZRef<ZExpr_Bool> iExpr_Bool)
:	Walker_Unary(iWalker)
,	fExpr_Bool(iExpr_Bool)
	{}

Walker_Select::~Walker_Select()
	{}

void Walker_Select::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fBindingOffsets = iBindingOffsets;
	fWalker->Prime(iBindingOffsets, fChildOffsets, ioBaseOffset);
	oOffsets.insert(fChildOffsets.begin(), fChildOffsets.end());
	}

bool Walker_Select::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	set<ZRef<ZCounted> > localAnnotations;
	set<ZRef<ZCounted> >* localAnnotationsPtr = nullptr;
	if (oAnnotations)
		localAnnotationsPtr = &localAnnotations;

	for (;;)
		{
		if (!fWalker->ReadInc(iBindings, oResults, localAnnotationsPtr))
			return false;

		ZMap_Any theMap;
		for (map<string8,size_t>::iterator i = fBindingOffsets.begin(); i != fBindingOffsets.end(); ++i)
			theMap.Set(i->first, iBindings[i->second]);
		for (map<string8,size_t>::iterator i = fChildOffsets.begin(); i != fChildOffsets.end(); ++i)
			theMap.Set(i->first, oResults[i->second]);
		
		if (sMatches(fExpr_Bool, theMap))
			{
			if (oAnnotations)
				oAnnotations->insert(localAnnotations.begin(), localAnnotations.end());
			return true;
			}
		localAnnotations.clear();
		}
	}

} // namespace ZQE
} // namespace ZooLib
