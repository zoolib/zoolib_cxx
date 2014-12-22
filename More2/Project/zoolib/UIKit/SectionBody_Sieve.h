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

#ifndef __ZooLib_UIKit_SectionBody_Sieve_h__
#define __ZooLib_UIKit_SectionBody_Sieve_h__ 1

#include "zoolib/Callable.h"

#include "ZooLib/Dataspace/Melange.h" // For ZooLib::Dataspace::Callable_DatonSetUpdate

#include "ZooLib/Dataspace/RelsWatcher.h"

#include "zoolib/ZTextCollator.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZVal_NS.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

#include "zoolib/uikit/UITVController_WithSections.h"

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
// MARK: - SortSpec

class SortSpec
	{
private:
	SortSpec();

public:
	SortSpec(const ZTrail& iPropTrail)
	:	fPropTrail(iPropTrail), fAscending(true), fStrength(0)
		{}

	SortSpec(const ZTrail& iPropTrail, bool iAscending)
	:	fPropTrail(iPropTrail), fAscending(iAscending), fStrength(0)
		{}

	SortSpec(const ZTrail& iPropTrail, bool iAscending, int iStrength)
	:	fPropTrail(iPropTrail), fAscending(iAscending), fStrength(iStrength)
		{}

	ZTrail fPropTrail;
	bool fAscending;
	int fStrength;
	};

// =================================================================================================
// MARK: - SectionBody_Sieve

class SectionBody_Sieve : public UIKit::SectionBody_Concrete
	{
public:
	typedef ZooLib::Dataspace::Callable_DatonSetUpdate Callable_DatonSetUpdate;
	typedef Callable<ZRef<UITableViewCell>(UITableView*,ZMap_Any)> Callable_GetCellForMap;
	typedef Callable<bool(ZMap_Any)> Callable_CanSelectForMap;
	typedef Callable<void()> Callable_NeedsUpdate;
	typedef ZMap_Any Entry;
	typedef Callable<void(std::vector<Entry> &)> Callable_PostProcess;

	SectionBody_Sieve();

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

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex,
		bool& ioIsPreceded, bool& ioIsSucceeded);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual bool CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex);

	virtual ZQ<bool> CanSelect(bool iEditing, size_t iRowIndex);

// Our protocol
	void SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ);

	void SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity);

	void SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity,
		const RelHead& iSignificant);

	void SetRel(ZRef<Expr_Rel> iRel, ZRef<Callable_Register> iCallable_Register,
		const ZQ<std::vector<SortSpec> > iSortSpecsQ,
		const RelHead& iIdentity,
		const RelHead& iSignificant,
		const ZQ<ColName>& iDatonColNameQ,
		const ZRef<Callable_DatonSetUpdate>& iCallable_DatonSetUpdate);

	ZQ<ZMap_Any> QGet(size_t iRowIndex);

	void pCreateOrDestroySieve(bool iShowing);

	void pChanged(const ZRef<ZCounted>& iRegistration,
		const ZRef<QueryEngine::Result>& iResult,
		bool iIsFirst);

	ZTextCollator fTextCollators[4];
	ZRef<Callable_GetCellForMap> fCallable_GetCellForMap;
	ZRef<Callable_CanSelectForMap> fCallable_CanSelectForMap;
	ZRef<Callable_NeedsUpdate> fCallable_NeedsUpdate;
	ZRef<Callable_PostProcess> fCallable_PostProcess;

	ZRef<Expr_Rel> fRel;
	ZRef<Callable_Register> fCallable_Register;
	ZRef<ZCounted> fRegistration;
	ZRef<QueryEngine::Result> fResult;
	std::vector<SortSpec> fSortSpecs;
	RelHead fIdentity;
	RelHead fSignificant;
	ZQ<ColName> fDatonColNameQ;
	ZRef<Callable_DatonSetUpdate> fCallable_DatonSetUpdate;

	bool fNeedsUpdate;
	std::vector<Entry> fRows;
	std::vector<Entry> fRows_Pending;

	bool fShowLoading;
	bool fWasLoading;
	bool fIsLoading;
	};

} // namespace UIKit
} // namespace ZooLib

#endif // __ZooLib_UIKit_SectionBody_Sieve_h__
