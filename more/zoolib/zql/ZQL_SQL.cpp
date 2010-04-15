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

#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"
#include "zoolib/ZExpr_Logic_ValCondition.h"
#include "zoolib/zql/ZQL_Expr_Rel_Join.h"
#include "zoolib/zql/ZQL_Expr_Rel_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Rename.h"
#include "zoolib/zql/ZQL_Expr_Rel_Restrict.h"
#include "zoolib/zql/ZQL_Expr_Rel_Select.h"
#include "zoolib/zql/ZQL_SQL.h"

#include <map>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {
namespace SQL {

using std::map;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Anonymous

namespace ZANONYMOUS {

class DoRename
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZExpr_Logic>
,	public virtual ZVisitor_Expr_Logic_ValCondition
	{
public:
	DoRename(const map<string8, string8>& iRenameMap);

	virtual void Visit_Expr_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition> iExpr);
private:
	const map<string8, string8>& fRenameMap;
	};

DoRename::DoRename(const map<string8, string8>& iRenameMap)
:	fRenameMap(iRenameMap)
	{}

void DoRename::Visit_Expr_Logic_ValCondition(ZRef<ZExpr_Logic_ValCondition> iExpr)
	{
	ZValCondition result;
	if (iExpr->GetValCondition().Renamed(fRenameMap, result))
		this->pSetResult(new ZExpr_Logic_ValCondition(result));
	else
		this->pSetResult(iExpr);
	}

class Doer
:	public virtual ZVisitor_Expr_Op_DoTransform_T<Expr_Rel_SFW>
,	public virtual ZQL::Visitor_Expr_Rel_Join
,	public virtual ZQL::Visitor_Expr_Rel_Concrete
,	public virtual ZQL::Visitor_Expr_Rel_Project
,	public virtual ZQL::Visitor_Expr_Rel_Rename
,	public virtual ZQL::Visitor_Expr_Rel_Restrict
,	public virtual ZQL::Visitor_Expr_Rel_Select
	{
public:
	Doer();
	virtual void Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel_SFW> > > iExpr);
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel_SFW> > > iExpr);
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel_SFW> > > iExpr);

	virtual void Visit_Expr_Rel_Join(ZRef<Expr_Rel_Join> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);
	};

Doer::Doer()
	{}

void Doer::Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void Doer::Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void Doer::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void Doer::Visit_Expr_Rel_Join(ZRef<Expr_Rel_Join> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());
	ZRef<Expr_Rel_SFW> sfw1 = this->Do(iExpr->GetOp1());

	map<string8, string8> renameMap = sfw0->GetRenameMap();
	const map<string8, string8>& renameMap1 = sfw1->GetRenameMap();
	renameMap.insert(renameMap1.begin(), renameMap1.end());
	
	vector<ZRef<Expr_Rel_Concrete> > rels = sfw0->GetRels();
	const vector<ZRef<Expr_Rel_Concrete> > rels1 = sfw1->GetRels();
	rels.insert(rels.end(), rels1.begin(), rels1.end());

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		renameMap,
		sfw0->GetRelHead() | sfw1->GetRelHead(),
		sfw0->GetCondition() | sfw1->GetCondition(),
		rels);

	this->pSetResult(result);
	}

void Doer::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{
	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		map<string8, string8>(),
		iExpr->GetRelHead(),
		sTrue(),
		vector<ZRef<Expr_Rel_Concrete> >(1, iExpr));

	this->pSetResult(result);
	}

void Doer::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	// Descend
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	// Apply projection
	
	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		sfw0->GetRenameMap(),
		sfw0->GetRelHead() & iExpr->GetRelHead(),
		sfw0->GetCondition(),
		sfw0->GetRels());

	this->pSetResult(result);
	}

static RelHead spRenamed(RelHead iRelHead, const string8& iOld, const string8& iNew)
	{
	if (iRelHead.Contains(iOld))
		{
		iRelHead -= iOld;
		iRelHead |= iNew;
		}
	return iRelHead;
	}

static ZRef<ZExpr_Logic> spRenamed(
	ZRef<ZExpr_Logic> iExpr_Logic, const string8& iOld, const string8& iNew)
	{
	map<string8, string8> renameMap;
	renameMap[iOld] = iNew;
	return DoRename(renameMap).Do(iExpr_Logic);
	}

void Doer::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	const string8& oldName = iExpr->GetOld();
	const string8& newName = iExpr->GetNew();
	map<string8, string8> renameMap = sfw0->GetRenameMap();
	bool foundIt = false;
	for (map<string8, string8>::iterator i = renameMap.begin();
		i != renameMap.end(); /*no inc*/)
		{
		if ((*i).second == oldName)
			{
			(*i).second = newName;
			foundIt = true;
			}
		}

	if (!foundIt)
		renameMap[oldName] = newName;

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		renameMap,
		spRenamed(sfw0->GetRelHead(), oldName, newName),
		spRenamed(sfw0->GetCondition(), oldName, newName),
		sfw0->GetRels());

	this->pSetResult(result);	
	}

void Doer::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		sfw0->GetRenameMap(),
		sfw0->GetRelHead(),
		sfw0->GetCondition() & iExpr->GetValCondition(),
		sfw0->GetRels());

	this->pSetResult(result);
	}

void Doer::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		sfw0->GetRenameMap(),
		sfw0->GetRelHead(),
		sfw0->GetCondition() & iExpr->GetExpr_Logic(),
		sfw0->GetRels());

	this->pSetResult(result);
	}

} // namespace ZANONYMOUS

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_SFW

Expr_Rel_SFW::Expr_Rel_SFW(const map<string8, string8>& iRenameMap,
	const RelHead& iRelHead,
	ZRef<ZExpr_Logic> iCondition,
	const vector<ZRef<Expr_Rel_Concrete> >& iRels)
:	fRenameMap(iRenameMap)
,	fRelHead(iRelHead)
,	fCondition(iCondition)
,	fRels(iRels)
	{}

RelHead Expr_Rel_SFW::GetRelHead()
	{ return fRelHead; }

const map<string8, string8>& Expr_Rel_SFW::GetRenameMap()
	{ return fRenameMap; }

ZRef<ZExpr_Logic> Expr_Rel_SFW::GetCondition()
	{ return fCondition; }

const vector<ZRef<Expr_Rel_Concrete> >& Expr_Rel_SFW::GetRels()
	{ return fRels; }

// =================================================================================================
#pragma mark -
#pragma mark * ZQL::SQL

ZRef<Expr_Rel_SFW> sConvert(ZRef<Expr_Rel> iExpr)
	{ return Doer().Do(iExpr); }

} // namespace SQL
} // namespace ZQL
NAMESPACE_ZOOLIB_END
