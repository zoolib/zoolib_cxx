/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZValPred_Any.h"

#include "zoolib/dataspace/RowBoat.h"

#include "zoolib/RelationalAlgebra/PseudoMap.h"
#include "zoolib/RelationalAlgebra/Util_Rel_Operators.h"

namespace ZooLib {
namespace Dataspace {

using namespace ZUtil_STL;

using std::vector;

using namespace RelationalAlgebra;
using namespace QueryEngine;

// =================================================================================================
// MARK: - RowBoat

RowBoat::RowBoat(const ZRef<Stew>& iStew,
	const ZRef<Expr_Rel>& iRel,
	const RelHead& iIdentity, const RelHead& iSignificant,
	const ZRef<Callable_Make_Callable_Row>& iCallable)
:	fStew(iStew)
,	fRel(iRel)
,	fResultDiffer(iIdentity, iSignificant)
,	fCallable(iCallable)
	{}

RowBoat::~RowBoat()
	{}

void RowBoat::Initialize()
	{
	ZCounted::Initialize();
	fRegistration = fStew->Register(sCallable(sWeakRef(this), &RowBoat::pChanged), fRel);
	}

const vector<ZRef<RowBoat::Callable_Row> >& RowBoat::GetRows()
	{
	ZAssert(fStew->GetWorker()->IsWorking());
	return fRows;
	}

void RowBoat::pChanged(
	const ZRef<Stew::Registration>& iReg,
	const ZRef<Event>& iEvent,
	const ZRef<Result>& iResult,
	bool iIsFirst)
	{
	ZRef<Result> priorResult;

	vector<size_t> theRemoved;
	vector<size_t> theAdded;
	vector<size_t> theChanged_Prior;
	vector<size_t> theChanged_New;

	fResultDiffer.Apply(iResult, &priorResult,
		&theRemoved, &theAdded,
		&theChanged_Prior, &theChanged_New);

	// We generate bindings the first time we're called, so they must
	// be empty if there's no prior result.
	ZAssert(sNotEmpty(fBindings) == bool(priorResult));

	// RelHeads can't and mustn't change from one result to another.
	ZAssert(not priorResult || priorResult->GetRelHead() == iResult->GetRelHead());

	// Change indices are paired, so the vectors must be the same size.
	ZAssert(theChanged_Prior.size() == theChanged_New.size());

	// We need removed and added to be applied in result sequence (or reverse), so that
	// we can trivially map between result and fRows.
	sort(theRemoved.begin(), theRemoved.end());
	sort(theAdded.begin(), theAdded.end());

	if (not priorResult)
		{
		// Build fBindings the first time we get a result.
		const RelHead& theRH = iResult->GetRelHead();
		size_t index = 0;
		for (RelHead::const_iterator ii = theRH.begin(), end = theRH.end(); ii != end; ++ii, ++index)
			fBindings.insert(std::pair<string8,size_t>(*ii, index));
		}

	// Note that we're doing a reverse scan here...
	foreachra (yy, theRemoved)
		{
		ZAssert(yy < fRows.size());
		if (ZRef<Callable_Row> theRow = fRows[yy])
			{
			const PseudoMap thePM_Prior(&fBindings, priorResult->GetValsAt(yy));
			theRow->Call(&thePM_Prior, nullptr);
			}
		fRows.erase(fRows.begin() + yy);
		}

	// ...and a forward scan here.
	foreacha (yy, theAdded)
		{
		ZAssert(yy <= fRows.size());
		const PseudoMap thePM_New(&fBindings, iResult->GetValsAt(yy));
		ZRef<Callable_Row> theRow = fCallable->Call(thePM_New);
		fRows.insert(fRows.begin() + yy, theRow);
		if (theRow)
			theRow->Call(nullptr, &thePM_New);
		}

	for (size_t yy = 0, end = theChanged_Prior.size(); yy < end; ++yy)
		{
		if (ZRef<Callable_Row> theRow = fRows[theChanged_New[yy]])
			{
			const PseudoMap thePM_Prior(&fBindings, priorResult->GetValsAt(theChanged_Prior[yy]));
			const PseudoMap thePM_New(&fBindings, iResult->GetValsAt(theChanged_New[yy]));
			theRow->Call(&thePM_Prior, &thePM_New);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
