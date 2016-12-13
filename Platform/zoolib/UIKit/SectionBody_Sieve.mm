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

#include <UIKit/UIApplication.h> // For UIApplicationWillEnterForegroundNotification etc

#include "zoolib/UIKit/SectionBody_Sieve.h"

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Callable_PMF.h"
#include "zoolib/Compare_T.h"
#include "zoolib/Log.h"
#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/PseudoMap.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zooLib/Apple/Starter_CFRunLoop.h"
#include "zooLib/Apple/Util_NS.h"

#include "zoolib/UIKit/TVCell.h"

// =================================================================================================
#pragma mark -
#pragma mark sCompare_T specialized for ZRef<SectionBody_Sieve::Callable_GetCellForMap>

namespace ZooLib {

using UIKit::SectionBody_Sieve;

template <>
int sCompare_T<ZRef<SectionBody_Sieve::Callable_GetCellForMap> >
	(const ZRef<SectionBody_Sieve::Callable_GetCellForMap>& iL,
	const ZRef<SectionBody_Sieve::Callable_GetCellForMap>& iR)
	{ return iL < iR ? -1 : iR < iL ? 1 : 0; }

ZMACRO_CompareRegistration_T(ZRef<SectionBody_Sieve::Callable_GetCellForMap>);

} // namespace ZooLib

namespace ZooLib {
namespace UIKit {

using QueryEngine::Result;

using std::pair;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark CompareEntries (anonymous)

typedef void ZTextCollator;

namespace { // anonymous

struct CompareEntries
	{
	CompareEntries(ZTextCollator* ioTextCollators, const vector<SortSpec>& fSortSpecs);

	bool operator()
		(const SectionBody_Sieve::Entry& iLeft, const SectionBody_Sieve::Entry& iRight) const;

	ZTextCollator* fTextCollators;
	const vector<SortSpec>& fSort;

	typedef Map_Any first_argument_type;
	typedef Map_Any second_argument_type;
	typedef bool result_type;
	};

int spCompare(int iStrength, ZTextCollator* ioTextCollators,
	const Val_Any& iLeft, const Val_Any& iRight)
	{
//	if (iStrength != 0)
//		{
//		if (const string8* l = iLeft.PGet<string8>())
//			{
//			if (const string8* r = iRight.PGet<string8>())
//				{
//				if (iStrength <= 4)
//					{
//					if (not ioTextCollators[iStrength - 1])
//						ioTextCollators[iStrength - 1] = ZTextCollator(iStrength);
//					return ioTextCollators[iStrength - 1].Compare(*l, *r);
//					}
//				else
//					{
//					return ZTextCollator(iStrength).Compare(*l, *r);
//					}				
//				}
//			}
//		}
	return sCompare_T(iLeft, iRight);
	}

CompareEntries::CompareEntries(ZTextCollator* ioTextCollators, const vector<SortSpec>& iSort)
:	fTextCollators(ioTextCollators)
,	fSort(iSort)
	{}

const Val_Any* spPGet(const Trail& iTrail, const Map_Any& iMap)
	{
	const Map_Any* theMap = &iMap;
	for (size_t xx = 0, count = iTrail.Count(); xx < count && theMap; ++xx)
		{
		const Val_Any* theVal = theMap->PGet(iTrail.At(xx));
		if (xx == count - 1)
			return theVal;
		if (not theVal)
			break;
		theMap = theVal->PGet<Map_Any>();
		}
	return nullptr;
	}

inline bool CompareEntries::operator()(
	const SectionBody_Sieve::Entry& iLeft, const SectionBody_Sieve::Entry& iRight) const
	{
	for (size_t xx = 0, count = fSort.size(); xx < count; ++xx)
		{
		if (const Val_Any* l = spPGet(fSort[xx].fPropTrail, iLeft))
			{
			if (const Val_Any* r = spPGet(fSort[xx].fPropTrail, iRight))
				{
				if (int compare = spCompare(fSort[xx].fStrength, fTextCollators, *l, *r))
					return fSort[xx].fAscending == (compare < 0);
				}
			else
				{
				return false;
				}
			}
		else if (spPGet(fSort[xx].fPropTrail, iRight))
			{
			return true;
			}
		}
	return false;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark SectionBody_Sieve

SectionBody_Sieve::SectionBody_Sieve()
:	fNeedsUpdate(false)
,	fShowLoading(false)
,	fWasLoading(false)
,	fIsLoading(true)
	{}

void SectionBody_Sieve::Initialize()
	{
	UIKit::SectionBody_Concrete::Initialize();

	const SEL willEnterForegroundNotificationSEL = sel_registerName("willEnterForegroundNotification");

	fDelegate.Set(willEnterForegroundNotificationSEL,
		sCallable(sWeakRef(this), &SectionBody_Sieve::pWillEnterForegroundNotification));

	[[NSNotificationCenter defaultCenter]
		addObserver:fDelegate
		selector:willEnterForegroundNotificationSEL
		name:UIApplicationWillEnterForegroundNotification
		object:nil];

	const SEL didEnterBackgroundNotificationSEL = sel_registerName("didEnterBackgroundNotification");

	fDelegate.Set(didEnterBackgroundNotificationSEL,
		sCallable(sWeakRef(this), &SectionBody_Sieve::pDidEnterBackgroundNotification));

	[[NSNotificationCenter defaultCenter]
		addObserver:fDelegate
		selector:didEnterBackgroundNotificationSEL
		name:UIApplicationDidEnterBackgroundNotification
		object:nil];
	}

size_t SectionBody_Sieve::NumberOfRows()
	{
	if (fShowLoading)
		{
		if (fWasLoading)
			return 1;
		}
	return fRows.size();
	}

void SectionBody_Sieve::PreUpdate()
	{
	fRows_Pending = fRows;
	fIsLoading = true;
	if (fRegistration)
		{
		ZLOGF(w, eDebug + 1);
		vector<Entry> theRows;
		if (ZRef<Result> theResult = fResult)
			{
			fIsLoading = false;
			const RelHead& theRelHead = theResult->GetRelHead();
			for (size_t xx = 0, count = theResult->Count(); xx < count; ++xx)
				{
				theRows.push_back(
					RelationalAlgebra::PseudoMap_RelHead(theRelHead, theResult->GetValsAt(xx))
					.AsMap());
				w << "\n" << xx << ": ";
//##				ZUtil_Any_JSON::sWrite(theRows.back(), w);
				}
			}

		if (fCallable_PostProcess)
			fCallable_PostProcess->Call(theRows);

		if (fSortSpecs.size())
			sort(theRows.begin(), theRows.end(), CompareEntries(nullptr, fSortSpecs));
//##			sort(theRows.begin(), theRows.end(), CompareEntries(fTextCollators, fSortSpecs));
		fRows_Pending.swap(theRows);
		}
	}

bool SectionBody_Sieve::WillBeEmpty()
	{
	if (fShowLoading && fIsLoading)
		return false;
	return fRows_Pending.empty();
	}

void SectionBody_Sieve::Update_NOP()
	{
	fNeedsUpdate = false;
	}

static size_t spFind(const RelHead& iIdentity, const SectionBody_Sieve::Entry& iOld,
	size_t iStart, const vector<SectionBody_Sieve::Entry>& iRows, size_t iEnd)
	{
	for (size_t xx = iStart; xx < iEnd; ++xx)
		{
		bool allEqual = true;
		foreachi (ii, iIdentity)
			{
			if (not (iOld.Get(*ii) == iRows[xx].Get(*ii)))
				allEqual = false;
			}
		if (allEqual)
			return xx;
		}
	return iEnd;
	}

static bool spSame(const Map_Any& l, const RelHead& iRelHead, const Map_Any& r)
	{
	if (iRelHead.empty())
		return false;

	foreachi (ii, iRelHead)
		{
		if (l.Get(*ii).Compare(r.Get(*ii)))
			return false;
		}
	return true;
	}

void SectionBody_Sieve::Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
	RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload)
	{
	if (fShowLoading)
		{
		if (fIsLoading && not fWasLoading)
			{
			ioRowMeta_Old.UpdateCount(fRows.size());
			ioRowUpdate_Delete.AddAll(UITableViewRowAnimationFade);
			ioRowMeta_New.UpdateCount(1);
			ioRowUpdate_Insert.AddAll(UITableViewRowAnimationFade);
			fNeedsUpdate = false;
			return;
			}

		if (not fIsLoading && fWasLoading)
			{
			ioRowMeta_Old.UpdateCount(1);
			ioRowUpdate_Delete.AddAll(UITableViewRowAnimationFade);
			ioRowMeta_New.UpdateCount(fRows_Pending.size());
			ioRowUpdate_Insert.AddAll(UITableViewRowAnimationFade);
			fNeedsUpdate = false;
			return;
			}
		}

	if (fRows.empty())
		{
		// We had no rows, so everything is an insert.		
		if (size_t theCount = fRows_Pending.size())
			{
			ioRowMeta_New.UpdateCount(theCount);
			ioRowUpdate_Insert.AddAll(this->RowAnimation_Insert());
			}
		fNeedsUpdate = false;
		return;
		}

	if (fRows_Pending.empty())
		{
		// We have no pending rows, so everything is a delete.
		if (size_t theCount = fRows.size())
			{
			ioRowMeta_Old.UpdateCount(theCount);
			ioRowUpdate_Delete.AddAll(this->RowAnimation_Delete());
			}
		fNeedsUpdate = false;
		return;
		}

	// We had rows, and still do, may need to diff them.

	const size_t endOld = fRows.size();
	ioRowMeta_Old.UpdateCount(endOld);

	size_t iterNew = 0;
	const size_t endNew = fRows_Pending.size();
	ioRowMeta_New.UpdateCount(endNew);

	if (not sGetSet(fNeedsUpdate, false))
		return;

	const UITableViewRowAnimation theAnimation_Insert = this->RowAnimation_Insert();
	const UITableViewRowAnimation theAnimation_Delete = this->RowAnimation_Delete();
	const UITableViewRowAnimation theAnimation_Reload = this->RowAnimation_Reload();

	for (size_t iterOld = 0; iterOld < endOld; ++iterOld)
		{
		const Entry rowOld = fRows[iterOld];
		size_t inNew = spFind(fIdentity, rowOld, iterNew, fRows_Pending, endNew);

		if (inNew == endNew)
			{
			// It's no longer in rowsNew, and its row must be deleted.
			ioRowUpdate_Delete.Add(iterOld, theAnimation_Delete);
			}
		else
			{
			// It's (still) in rowsNew.
			const Entry rowNew = fRows_Pending[inNew];
			if (not spSame(rowOld, fSignificant, rowNew))
				{
				// But has different content.
				ioRowUpdate_Reload.Add(iterOld, theAnimation_Reload);
				}
			
			if (size_t countToInsert = inNew - iterNew)
				{
				// There are rows to insert prior to rowOld.
				ioRowUpdate_Insert.AddRange(iterNew, countToInsert, theAnimation_Insert);
				}
			iterNew = inNew + 1;
			}
		}

	// Insert remainder of pending.
	if (size_t countToInsert = endNew - iterNew)
		ioRowUpdate_Insert.AddRange(iterNew, countToInsert, theAnimation_Insert);
	}

void SectionBody_Sieve::Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New)
	{
	fNeedsUpdate = false;

	if (fShowLoading && fIsLoading)
		ioRowMeta_New.UpdateCount(1);
	else
		ioRowMeta_New.UpdateCount(fRows_Pending.size());

	ioRowUpdate_New.AddAll(this->RowAnimation_Insert());
	}

void SectionBody_Sieve::Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old)
	{
	fNeedsUpdate = false;

	if (fShowLoading && fWasLoading)
		ioRowMeta_Old.UpdateCount(1);
	else
		ioRowMeta_Old.UpdateCount(fRows.size());

	ioRowUpdate_Old.AddAll(this->RowAnimation_Delete());
	}

void SectionBody_Sieve::FinishUpdate()
	{
	fWasLoading = fIsLoading;
	fRows = fRows_Pending;
	}

void SectionBody_Sieve::ViewWillAppear(UITableView* iTV)
	{
	fMayBeShowing = true;
	this->pGetSieveCorrectlySetup();
	}

void SectionBody_Sieve::ViewDidAppear(UITableView* iTV)
	{}

void SectionBody_Sieve::ViewWillDisappear(UITableView* iTV)
	{}

void SectionBody_Sieve::ViewDidDisappear(UITableView* iTV)
	{
	fMayBeShowing = false;
	this->pGetSieveCorrectlySetup();
	}

ZRef<UITableViewCell> SectionBody_Sieve::UITableViewCellForRow(UITableView* iView, size_t iRowIndex,
	bool& ioIsPreceded, bool& ioIsSucceeded)
	{
	if (fShowLoading && fIsLoading)
		{
		ZAssertStop(0, iRowIndex == 0);
		ZRef<UITableViewCell> theCell = sGetCell_Simple(iView, Util_NS::sString("Loading..."), false);
//##		[theCell->fUILabel_Left setTextColor:sColor_Text_Gray()];
		return theCell;
		}
	else if (fCallable_GetCellForMap)
		{
		if (ZRef<UITableViewCell> theCell =
			fCallable_GetCellForMap->Call(iView, fRows[iRowIndex]))
			{
			if (fApplyAccessory)
				this->ApplyAccessory(iRowIndex, theCell);
			if (iRowIndex > 0)
				ioIsPreceded = true;
			if (iRowIndex < fRows.size() - 1)
				ioIsSucceeded = true;
			return theCell;
			}
		}
	ZUnimplemented();
	return null;
	}

ZQ<UITableViewCellEditingStyle> SectionBody_Sieve::QEditingStyle(size_t iRowIndex)
	{
	if (fShowLoading && fIsLoading)
		return UITableViewCellEditingStyleNone;

	if (not fDatonColNameQ)
		return UITableViewCellEditingStyleNone;

	return SectionBody_Concrete::QEditingStyle(iRowIndex);
	}

ZQ<bool> SectionBody_Sieve::QShouldIndentWhileEditing(size_t iRowIndex)
	{
	if (fShowLoading && fIsLoading)
		return false;
	return SectionBody_Concrete::QShouldIndentWhileEditing(iRowIndex);
	}

bool SectionBody_Sieve::CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex)
	{
	if (iRowIndex < fRows.size())
		{
		if (fDatonColNameQ)
			{
			if (ZQ<Dataspace::Daton> theDatonQ =
				fRows[iRowIndex].QGet<Dataspace::Daton>(*fDatonColNameQ))
				{
				sCall(fCallable_DatonUpdate, nullptr, 0, &*theDatonQ, 1);
				}
			}
		}
	return true;
	}

ZQ<bool> SectionBody_Sieve::CanSelect(bool iEditing, size_t iRowIndex)
	{
	if (fShowLoading && fIsLoading)
		return false;

	if (fCallable_CanSelectForMap)
		return fCallable_CanSelectForMap->Call(fRows[iRowIndex]);

	return SectionBody_Concrete::CanSelect(iEditing, iRowIndex);
	}

void SectionBody_Sieve::SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
	const ZQ<vector<SortSpec> > iSortSpecsQ)
	{
	const RelHead theRH = sGetRelHead(iRel);
	this->SetRel(iRel, iCallable_Register, iSortSpecsQ, theRH, theRH, null, null);
	}

void SectionBody_Sieve::SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
	const ZQ<vector<SortSpec> > iSortSpecsQ,
	const RelHead& iIdentity)
	{ this->SetRel(iRel, iCallable_Register, iSortSpecsQ, iIdentity, sGetRelHead(iRel), null, null); }

void SectionBody_Sieve::SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
	const ZQ<vector<SortSpec> > iSortSpecsQ,
	const RelHead& iIdentity,
	const RelHead& iSignificant)
	{ this->SetRel(iRel, iCallable_Register, iSortSpecsQ, iIdentity, iSignificant, null, null); }

void SectionBody_Sieve::SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
	const ZQ<vector<SortSpec> > iSortSpecsQ,
	const RelHead& iIdentity,
	const RelHead& iSignificant,
	const ZQ<ColName>& iDatonColNameQ,
	const ZRef<Callable_DatonUpdate>& iCallable_DatonUpdate)
	{
	if (fRel == iRel)
		return;

	if (ZLOGPF(s, eDebug))
		s << "\n" << iRel;

	fRel = iRel;

	fSortSpecs = sGet(iSortSpecsQ);

	fIdentity = iIdentity;
	fSignificant = iSignificant;
	fDatonColNameQ = iDatonColNameQ;
	fCallable_Register = iCallable_Register;
	fCallable_DatonUpdate = iCallable_DatonUpdate;

	ZAssert(not iDatonColNameQ || iCallable_DatonUpdate);

	if (fDatonColNameQ && fRel)
		ZAssert(Util_STL::sContains(sGetRelHead(fRel), *fDatonColNameQ));

	fRegistration.Clear();

	fMayBeShowing = true;

	this->pGetSieveCorrectlySetup();
	}

ZQ<Map_Any> SectionBody_Sieve::QGet(size_t iRowIndex)
	{
	if (iRowIndex < fRows.size())
		return fRows[iRowIndex];
	return null;
	}

void SectionBody_Sieve::pGetSieveCorrectlySetup()
	{
	if (fMayBeShowing
		&& (UIApplicationStateBackground != [UIApplication sharedApplication].applicationState))
		{
		if (fRel and not fRegistration)
			{
			fRegistration = sCall(fCallable_Register,
				sCallable(sWeakRef(this), &SectionBody_Sieve::pChanged), fRel);
			}
		}
	else
		{
		fRegistration.Clear();
		}

	if (fShowLoading && not sGetSet(fNeedsUpdate, true))
		fCallable_NeedsUpdate->Call();
	}

void SectionBody_Sieve::pChanged(const ZRef<ZCounted>& iRegistration,
	const ZRef<QueryEngine::Result>& iResult,
	bool iIsFirst)
	{
	fResult = iResult;
	if (not sGetSet(fNeedsUpdate, true))
		fCallable_NeedsUpdate->Call();
	}

void SectionBody_Sieve::pWillEnterForegroundNotification()
	{
	// At this point, applicationState is still UIApplicationStateBackground. So call
	// pGetSieveCorrectlySetup the next time round the event loop. Or more likely at the end of
	// this event loop, and in any case, after the application considers itself to be out of
	// the background.
	//
	Starter_CFRunLoop::sMain()->QStart(sCallable(sWeakRef(this), &SectionBody_Sieve::pGetSieveCorrectlySetup));
	}

void SectionBody_Sieve::pDidEnterBackgroundNotification()
	{ this->pGetSieveCorrectlySetup(); }

} // namespace UIKit
} // namespace ZooLib
