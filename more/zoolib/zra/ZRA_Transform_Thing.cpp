/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ZUtil_STL_map.h"

#include "zoolib/zra/ZRA_Transform_Thing.h"

using std::set;

namespace ZooLib {
namespace ZRA {

using namespace ZUtil_STL;

// =================================================================================================
#pragma mark -
#pragma mark * Transform_Thing

Transform_Thing::Transform_Thing()
:	fNextInt(1)
	{}

void Transform_Thing::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{
	const Rename& theRename = iExpr->GetBindings();
	Rename newRename;
	for (Rename::const_iterator i = theRename.begin(); i != theRename.end(); ++i)
		{
		const string8& concreteName = i->first;
		const string8 tempName = sGetMustContain(1, fRename_Concrete2Temp, concreteName);
		sInsertMustNotContain(1, newRename, tempName, i->second);
		}

	const RelName theRelName = iExpr->GetRelName();
	const string8 newRelName = ZStringf("T%d", fNextInt++);
	fResultingRelHead |= newRelName;

	sInsertMustNotContain(1, fRename_Temp2Concrete, newRelName, theRelName);
	sInsertMustNotContain(1, fRename_Concrete2Temp, theRelName, newRelName);

	this->pSetResult(new Expr_Rel_Calc(newRelName, newRename, iExpr->GetCallable()));
	}

void Transform_Thing::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	// Insert renames from concrete to guaranteed unique temp names.
	const RelHead& theRH = iExpr->GetConcreteRelHead();
	ZRef<Expr_Rel> result = iExpr;
	for (RelHead::const_iterator i = theRH.begin(); i != theRH.end(); ++i)
		{
		const string8 tempName = ZStringf("T%d", fNextInt++);
		fResultingRelHead |= tempName;

		const string8 concreteName = *i;
		sInsertMustNotContain(1, fRename_Temp2Concrete, tempName, concreteName);
		sInsertMustNotContain(1, fRename_Concrete2Temp, concreteName, tempName);
		result = ZRA::sRename(result, tempName, concreteName);
		}
	this->pSetResult(result);
	}

void Transform_Thing::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{
	const string8 tempName = ZStringf("T%d", fNextInt++);
	fResultingRelHead |= tempName;

	const string8 concreteName = iExpr->GetRelName();
	sInsertMustNotContain(1, fRename_Temp2Concrete, tempName, concreteName);
	sInsertMustNotContain(1, fRename_Concrete2Temp, concreteName, tempName);

	this->pSetResult(new Expr_Rel_Const(tempName, iExpr->GetVal()));
	}

void Transform_Thing::Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr)
	{ this->pSetResult(iExpr); }

void Transform_Thing::Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr)
	{
	// Still need to decide how to handle ultimate renaming.
	const Rename& theRename = iExpr->GetBindings();
	Rename newRename;
	for (Rename::const_iterator i = theRename.begin(); i != theRename.end(); ++i)
		{
		const string8& concreteName = i->first;
		const string8 tempName = sGetMustContain(1, fRename_Concrete2Temp, concreteName);
		sInsertMustNotContain(1, newRename, tempName, i->second);
		}

	const RelName theRelName = iExpr->GetRelName();
	const string8 newRelName = ZStringf("T%d", fNextInt++);
	fResultingRelHead |= newRelName;

	sInsertMustNotContain(1, fRename_Temp2Concrete, newRelName, theRelName);
	sInsertMustNotContain(1, fRename_Concrete2Temp, theRelName, newRelName);

	ZSetRestore_T<Rename> sr1(fRename_Concrete2Temp);
	ZSetRestore_T<Rename> sr2(fRename_Temp2Concrete);
	ZSetRestore_T<RelHead> sr3(fResultingRelHead);

	ZRef<Expr_Rel> newOp0 = this->TopLevelDo(iExpr->GetOp0());

	this->pSetResult(new Expr_Rel_Embed(newRelName, newRename, newOp0));
	}

void Transform_Thing::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{ inherited::Visit_Expr_Op2(iExpr); }

void Transform_Thing::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	const RelHead savedRH = fResultingRelHead;
	fResultingRelHead.clear();

	this->pSetResult(this->Do(iExpr->GetOp0()));
	const RelHead& theRH = iExpr->GetProjectRelHead();
	
	// Only add to newResultingRH those names that would not be stripped by theRH.
	RelHead newResultingRH = savedRH;
	for (RelHead::const_iterator i = fResultingRelHead.begin(); i != fResultingRelHead.end(); ++i)
		{
		const string8 tempName = *i;
		const string8 concreteName = sGetMustContain(1, fRename_Temp2Concrete, tempName);
		if (theRH.Contains(concreteName))
			newResultingRH |= tempName;
		}

	fResultingRelHead.swap(newResultingRH);
	}

void Transform_Thing::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	this->pSetResult(this->Do(iExpr->GetOp0()));

	const string8 oldName = iExpr->GetOld();
	const string8 newName = iExpr->GetNew();

	// Update mappings, so ancestor expressions and so forth can be rewritten
	// in terms of our 'canonical' temp names.

	const string8 tempName = sEraseAndReturn(1, fRename_Concrete2Temp, oldName);
	sInsertMustNotContain(1, fRename_Concrete2Temp, newName, tempName);
	sSetMustContain(1, fRename_Temp2Concrete, tempName, newName);
	}

void Transform_Thing::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	ZRef<Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	ZRef<ZExpr_Bool> newExpr_Bool =
		Util_Expr_Bool::sRenamed(fRename_Concrete2Temp, iExpr->GetExpr_Bool());
	this->pSetResult(sRestrict(newOp0, newExpr_Bool));
	}

ZRef<Expr_Rel> Transform_Thing::TopLevelDo(ZRef<Expr_Rel> iExpr)
	{
	ZRef<ZRA::Expr_Rel> result = this->Do(iExpr);
	result = sProject(result, fResultingRelHead);
	for (RelHead::const_iterator i = fResultingRelHead.begin(); i != fResultingRelHead.end(); ++i)
		result = sRename(result, sGetMustContain(1, fRename_Temp2Concrete, *i), *i);
	return result;
	}

} // namespace ZRA
} // namespace ZooLib
