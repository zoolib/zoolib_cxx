// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UIKit_SectionBody_Sieve_h__
#define __ZooLib_UIKit_SectionBody_Sieve_h__ 1

#include "zoolib/Callable.h"
#include "zoolib/Apple/Delegate.h"

#include "zoolib/Dataspace/Melange.h" // For ZooLib::Dataspace::Callable_DatonUpdate
#include "zoolib/Dataspace/RelsWatcher.h"

//###include "zoolib/ZTextCollator.h"

#include "zoolib/Trail.h"
#include "zoolib/Val_ZZ.h"

#include "zoolib/Apple/Val_NS.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

#include "zoolib/UIKit/UITVController_WithSections.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/Dataspace/Types.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace UIKit {

using RelationalAlgebra::ColName;
using RelationalAlgebra::Expr_Rel;
using RelationalAlgebra::RelHead;
using Dataspace::RelsWatcher::Callable_Register;

// =================================================================================================
#pragma mark - SortSpec

class SortSpec
	{
private:
	SortSpec();

public:
	SortSpec(const Trail& iPropTrail)
	:	fPropTrail(iPropTrail), fAscending(true), fStrength(0)
		{}

	SortSpec(const Trail& iPropTrail, bool iAscending)
	:	fPropTrail(iPropTrail), fAscending(iAscending), fStrength(0)
		{}

	SortSpec(const Trail& iPropTrail, bool iAscending, int iStrength)
	:	fPropTrail(iPropTrail), fAscending(iAscending), fStrength(iStrength)
		{}

	Trail fPropTrail;
	bool fAscending;
	int fStrength;
	};

// =================================================================================================
#pragma mark - SectionBody_Sieve

class SectionBody_Sieve : public UIKit::SectionBody_Concrete
	{
public:
	typedef ZooLib::Dataspace::Callable_DatonUpdate Callable_DatonUpdate;
	typedef Callable<ZP<UITableViewCell>(UITableView*,Map_ZZ)> Callable_GetCellForMap;
	typedef Callable<bool(Map_ZZ)> Callable_CanSelectForMap;
	typedef Callable<void()> Callable_NeedsUpdate;
	typedef Map_ZZ Entry;
	typedef Callable<void(std::vector<Entry> &)> Callable_PostProcess;

	SectionBody_Sieve();

	virtual void Initialize();

// From SectionBody
	virtual size_t NumberOfRows();

	virtual void PreUpdate();
	virtual bool WillBeEmpty();
	virtual void Update_NOP();
	virtual void Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
		RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload);
	virtual void Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New);
	virtual void Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old);
	virtual void FinishUpdate();

// -----

	virtual void ViewWillAppear(UITableView* iTV);
	virtual void ViewDidAppear(UITableView* iTV);
	virtual void ViewWillDisappear(UITableView* iTV);
	virtual void ViewDidDisappear(UITableView* iTV);

// -----

	virtual ZP<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex,
		bool& ioIsPreceded, bool& ioIsSucceeded);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual bool CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex);

	virtual ZQ<bool> CanSelect(bool iEditing, size_t iRowIndex);

// Our protocol
	void SetRel(ZP<Expr_Rel> iRel, ZP<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ);

	void SetRel(ZP<Expr_Rel> iRel, ZP<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity);

	void SetRel(ZP<Expr_Rel> iRel, ZP<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity,
		const RelHead& iSignificant);

	void SetRel(ZP<Expr_Rel> iRel, ZP<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity,
		const RelHead& iSignificant,
		const ZQ<ColName>& iDatonColNameQ,
		const ZP<Callable_DatonUpdate>& iCallable_DatonUpdate);

	ZQ<Map_ZZ> QGet(size_t iRowIndex);

	void pGetSieveCorrectlySetup();

	void pChanged(
		const ZP<Counted>& iRegistration,
		int64 iChangeCount,
		const ZP<QueryEngine::Result>& iResult,
		const ZP<QueryEngine::ResultDeltas>& iResultDeltas);

//##	ZTextCollator fTextCollators[4];
	ZP<Callable_GetCellForMap> fCallable_GetCellForMap;
	ZP<Callable_CanSelectForMap> fCallable_CanSelectForMap;
	ZP<Callable_NeedsUpdate> fCallable_NeedsUpdate;
	ZP<Callable_PostProcess> fCallable_PostProcess;

	ZP<Expr_Rel> fRel;
	ZP<Callable_Register> fCallable_Register;
	ZP<Counted> fRegistration;
	ZP<QueryEngine::Result> fResult;
	std::vector<SortSpec> fSortSpecs;
	RelHead fIdentity;
	RelHead fSignificant;
	ZQ<ColName> fDatonColNameQ;
	ZP<Callable_DatonUpdate> fCallable_DatonUpdate;

	bool fNeedsUpdate;
	std::vector<Entry> fRows;
	std::vector<Entry> fRows_Pending;

	bool fShowLoading;
	bool fWasLoading;
	bool fIsLoading;

	bool fMayBeShowing;
	Delegate fDelegate;
	void pWillEnterForegroundNotification();
	void pDidEnterForeground();
	void pDidEnterBackgroundNotification();
	};

} // namespace UIKit
} // namespace ZooLib

#endif // __ZooLib_UIKit_SectionBody_Sieve_h__
