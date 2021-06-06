// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/RowBoat.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/RelationalAlgebra/PseudoMap.h"
#include "zoolib/RelationalAlgebra/Util_Rel_Operators.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_DB.h"

namespace ZooLib {
namespace Dataspace {

using namespace Util_STL;

using std::pair;
using std::vector;

// =================================================================================================
#pragma mark - Comparer_t (anonymous)

namespace { // anonymous

pair<int,size_t> spCompare(const vector<size_t>& iOffsets,
	const Val_DB* iVals_Left, const Val_DB* iVals_Right)
	{
	const size_t offsetsCount = iOffsets.size();
	for (size_t yy = 0; yy < offsetsCount; ++yy)
		{
		const size_t theCol = iOffsets[yy];
		if (int compare = sCompare_T(iVals_Left[theCol], iVals_Right[theCol]))
			return pair<int,size_t>(compare, yy);
		}
	return pair<int,size_t>(0, offsetsCount);
	}

struct Comparer_t
	{
	Comparer_t(const vector<size_t>& iOffsets, const ZP<Result>& iResult)
	:	fOffsets(iOffsets)
	,	fResult(iResult)
		{}

	bool operator()(const size_t& iLeft, const size_t& iRight) const
		{
		return 0 > spCompare(fOffsets,
			fResult->GetValsAt(iLeft),
			fResult->GetValsAt(iRight)).first;
		}

	const vector<size_t>& fOffsets;
	ZP<Result> fResult;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - RowBoat

RowBoat::RowBoat(const ZP<Callable_Register>& iCallable_Register,
	const ZP<Expr_Rel>& iRel,
	const RelHead& iIdentity, const RelHead& iSignificant,
	bool iEmitDummyChanges,
	const ZP<Callable_Make_Callable_Row>& iCallable)
:	fCallable_Register(iCallable_Register)
,	fRel(iRel)
,	fIdentity(iIdentity)
,	fEmitDummyChanges(iEmitDummyChanges)
,	fCallable(iCallable)
	{}

RowBoat::~RowBoat()
	{}

void RowBoat::Initialize()
	{
	Counted::Initialize();
	fRegistration = fCallable_Register->Call(sCallable(sWP(this), &RowBoat::pChanged), fRel);
	}

const vector<ZP<RowBoat::Callable_Row>>& RowBoat::GetRows()
	{ return fRows; }

void RowBoat::pChanged(
	const ZP<Counted>& iRegistration,
	int64 iChangeCount,
	const ZP<Result>& iResult,
	const ZP<ResultDeltas>& iResultDeltas)
	{
	ZP<Result> theResult_New = iResult;
	if (iResultDeltas)
		theResult_New = sApplyDeltas(fResult_Prior, iResultDeltas);

	const RelHead& theRH = theResult_New->GetRelHead();

	if (fResult_Prior)
		{
		// We generate bindings the first time we're called, so they must be non-empty at this point.
		ZAssert(sNotEmpty(fBindings));

		// RelHeads can't and mustn't change from one result to another.
		ZAssert(fResult_Prior->GetRelHead() == theRH);
		}
	else
		{
		// This is the first time we get to see a query's relhead

		ZAssert(sIsEmpty(fBindings));
		RelationalAlgebra::sBuildBindings(theRH, fBindings);

		// Initialize fPermute so we know in which order to examine
		// columns when we sort and compare result rows.

		RelHead::const_iterator iter_Identity = fIdentity.begin();
		size_t index_Identity = 0;

		RelHead::const_iterator iter_RH = theRH.begin();
		size_t index_Other = 0;

		size_t index_RH = 0;

		fPermute.resize(theRH.size());

		for (const RelHead::const_iterator end_RH = theRH.end();
			iter_RH != end_RH;
			++iter_RH, ++index_RH)
			{
			if (iter_Identity != fIdentity.end() && *iter_Identity == *iter_RH)
				{
				fPermute[index_Identity] = index_RH;
				++index_Identity;
				++iter_Identity;
				}
			else
				{
				fPermute[fIdentity.size() + index_Other] = index_RH;
				++index_Other;
				}
			}

		// We must have consumed the entirety of our identity relhead
		ZAssert(iter_Identity == fIdentity.end());
		ZAssert(index_Identity == fIdentity.size());

		// Dummy fResult_Prior
		fResult_Prior = new Result(theRH);
		}

	size_t theIndex_Prior = 0;
	const size_t theCount_Prior = fSort_Prior.size();

	size_t theIndex_New = 0;
	const size_t theCount_New = theResult_New->Count();

	vector<size_t> theSort_New;
	theSort_New.reserve(theCount_New);
	for (size_t xx = 0; xx < theCount_New; ++xx)
		theSort_New.push_back(xx);

	sort(theSort_New.begin(), theSort_New.end(), Comparer_t(fPermute, theResult_New));

	std::vector<ZP<Callable_Row>> theRows_New(theCount_New);
	for (;;)
		{
		// We're copying/updating, dropping or inserting.

		if (theIndex_New >= theCount_New)
			{
			// Anything remaining in prior now that new is exhausted is a removal.
			while (theCount_Prior > theIndex_Prior)
				{
				const PseudoMap thePM_Prior(&fBindings,
					fResult_Prior->GetValsAt(fSort_Prior[theIndex_Prior]));
				fRows[theIndex_Prior++]->Call(&thePM_Prior, nullptr);
				}
			break;
			}

		if (theIndex_Prior >= theCount_Prior)
			{
			// Anything remaining in new now that prior is exhausted is an addition.
			while (theCount_New > theIndex_New)
				{
				const PseudoMap thePM_New(&fBindings,
					theResult_New->GetValsAt(theSort_New[theIndex_New]));
				ZP<Callable_Row> theRow = fCallable->Call(thePM_New);
				theRows_New[theIndex_New++] = theRow;
				theRow->Call(nullptr, &thePM_New);
				}
			break;
			}

		const Val_DB* vals_Prior = fResult_Prior->GetValsAt(fSort_Prior[theIndex_Prior]);
		const Val_DB* vals_New = theResult_New->GetValsAt(theSort_New[theIndex_New]);

		const pair<int,size_t> result = spCompare(fPermute, vals_Prior, vals_New);

		if (result.second < fIdentity.size())
			{
			// Comparison was terminated in the 'identity' portion of the values,
			// and so the values can't be equal.
			ZAssert(result.first != 0);

			if (result.first < 0)
				{
				// Prior is less than new, so prior is not in new, and this is a removal.
				const PseudoMap thePM_Prior(&fBindings, vals_Prior);
				fRows[theIndex_Prior++]->Call(&thePM_Prior, nullptr);
				}
			else
				{
				// Contrariwise.
				const PseudoMap thePM_New(&fBindings, vals_New);
				ZP<Callable_Row> theRow = fCallable->Call(thePM_New);
				theRows_New[theIndex_New++] = theRow;
				theRow->Call(nullptr, &thePM_New);
				}
			}
		else
			{
			ZP<Callable_Row> theRow = fRows[theIndex_Prior++];
			theRows_New[theIndex_New++] = theRow;

			if (fEmitDummyChanges || result.first)
				{
				const PseudoMap thePM_Prior(&fBindings, vals_Prior);
				const PseudoMap thePM_New(&fBindings, vals_New);
				theRow->Call(&thePM_Prior, &thePM_New);
				}
			}
		}

	for (size_t xx = 0; xx < theRows_New.size(); ++xx)
		ZAssert(theRows_New[xx]);

	ZAssert(theIndex_New == theCount_New);
	ZAssert(theIndex_Prior == theCount_Prior);

	swap(fRows, theRows_New);
	swap(fSort_Prior, theSort_New);
	swap(fResult_Prior, theResult_New);
	}

} // namespace Dataspace
} // namespace ZooLib
