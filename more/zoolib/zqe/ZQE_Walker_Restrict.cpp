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

#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches.h"
#include "zoolib/zqe/ZQE_Walker_Restrict.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Restrict

Walker_Restrict::Walker_Restrict(ZRef<Walker> iWalker, ZRef<ZExpr_Bool> iExpr_Bool)
:	Walker_Unary(iWalker)
,	fExpr_Bool(iExpr_Bool)
	{}

Walker_Restrict::~Walker_Restrict()
	{}

ZRef<Walker> Walker_Restrict::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker = fWalker->Prime(iOffsets, fChildOffsets, ioBaseOffset);
	if (!fWalker)
		return null;
	oOffsets.insert(fChildOffsets.begin(), fChildOffsets.end());
	fChildOffsets.insert(iOffsets.begin(), iOffsets.end());
	return this;
	}

bool Walker_Restrict::ReadInc(
	ZVal_Any* ioResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	set<ZRef<ZCounted> > localAnnotations;
	set<ZRef<ZCounted> >* localAnnotationsPtr = nullptr;
	if (oAnnotations)
		localAnnotationsPtr = &localAnnotations;

	for (;;)
		{
		if (!fWalker->ReadInc(ioResults, localAnnotationsPtr))
			return false;

		ZMap_Any theMap;
		for (map<string8,size_t>::iterator i = fChildOffsets.begin();
			i != fChildOffsets.end(); ++i)
			{
			string8 theName = i->first;
			size_t theOffset = i->second;
			ZVal_Any theVal = ioResults[theOffset];
			theMap.Set(theName, theVal);
			}
		
		if (ZLOGF(s, eDebug+2))
			{
			s << this << " ";
			ZYad_ZooLibStrim::sToStrim(sMakeYadR(theMap), s);
			}

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
