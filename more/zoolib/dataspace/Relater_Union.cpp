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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/ZValPred_GetNames.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Do_GetNames.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/ZUtil_Strim_Operators.h"

#include "zoolib/dataspace/Relater_Union.h"
#include "zoolib/dataspace/Util_Strim.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"
#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"
#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"

namespace ZooLib {
namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace ZUtil_STL;

namespace RA = RelationalAlgebra;

// =================================================================================================
// MARK: - InsertPrefix (anonymous)

namespace { // anonymous

class InsertPrefix
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<RA::Expr_Rel>
,	public virtual RA::Visitor_Expr_Rel_Concrete
	{
public:
	InsertPrefix(const string8& iPrefix);

// From RA::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr);

private:
	const string8 fPrefix;
	};

InsertPrefix::InsertPrefix(const string8& iPrefix)
:	fPrefix(iPrefix)
	{}

void InsertPrefix::Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
	{
	const RelHead& theRelHead = RA::sRelHead(iExpr->GetConcreteHead());

	if (const ZQ<RelHead> theQ = RA::sQPrefixErased(fPrefix, theRelHead))
		{
		ZRef<RA::Expr_Rel> theRel = RA::sConcrete(*theQ);
		foreachi (ii, *theQ)
			theRel = sRename(theRel, RA::sPrefixInserted(fPrefix, *ii), *ii);

		this->pSetResult(theRel);
		}
	else
		{
		// The prefix does not exist on every ColName, so we can't produce a valid Rel.
		}
	}

} // anonymous namespace

// =================================================================================================
// MARK: - Relater_Union::PIP

class Relater_Union::DLink_PIP_InProxy
:	public DListLink<PIP, DLink_PIP_InProxy, kDebug>
	{};

class Relater_Union::DLink_PIP_NeedsWork
:	public DListLink<PIP, DLink_PIP_NeedsWork, kDebug>
	{};

class Relater_Union::PIP
:	public DLink_PIP_InProxy
,	public DLink_PIP_NeedsWork
	{
public:
	Proxy* fProxy;
	PRelater* fPRelater;
	bool fNeedsAdd;
	int64 fRefcon;
	ZRef<QueryEngine::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
// MARK: - Relater_Union::Proxy declaration

class Relater_Union::Proxy
:	public virtual RA::Expr_Rel
,	public virtual ZExpr_Op0_T<RA::Expr_Rel>
	{
	typedef ZExpr_Op0_T<Expr_Rel> inherited;
public:
	Proxy(Relater_Union* iRelater);

// From ZCounted
	virtual void Finalize();

// From ZVisitee
	virtual void Accept(const ZVisitor& iVisitor);

// From ZExpr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<RA::Expr_Rel> Self();
	virtual ZRef<RA::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Proxy(Visitor_Proxy& iVisitor);

	Relater_Union* const fRelater;
	ZRef<RA::Expr_Rel> fRel;
	RA::RelHead fResultRelHead;
	set<PQuery*> fDependentPQueries;

	// Something that ties this proxy to each Relater.
	DListHead<DLink_PIP_InProxy> fPIP_InProxy;
	};

// =================================================================================================
// MARK: - Relater_Union::Visitor_Proxy

class Relater_Union::Visitor_Proxy
:	public virtual ZVisitor_Expr_Op0_T<RA::Expr_Rel>
	{
public:
	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->Visit_Expr_Op0(iExpr); }
	};

// =================================================================================================
// MARK: - Relater_Union::Proxy definition

Relater_Union::Proxy::Proxy(Relater_Union* iRelater)
:	fRelater(iRelater)
	{}

void Relater_Union::Proxy::Finalize()
	{ fRelater->pFinalizeProxy(this); }

void Relater_Union::Proxy::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Relater_Union::Proxy::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<RA::Expr_Rel>& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<RA::Expr_Rel> Relater_Union::Proxy::Self()
	{ return this; }

ZRef<RA::Expr_Rel> Relater_Union::Proxy::Clone()
	{ return this; }

void Relater_Union::Proxy::Accept_Proxy(Visitor_Proxy& iVisitor)
	{ iVisitor.Visit_Proxy(this); }

// =================================================================================================
// MARK: - Relater_Union::Walker_Proxy

class Relater_Union::Walker_Proxy : public QueryEngine::Walker
	{
public:
	Walker_Proxy(ZRef<Relater_Union> iRelater, ZRef<Proxy> iProxy)
	:	fRelater(iRelater)
	,	fProxy(iProxy)
	,	fIter_PIP(iProxy->fPIP_InProxy)
		{}

	virtual ~Walker_Proxy()
		{}

	virtual void Rewind()
		{ fRelater->pRewind(this); }

	virtual ZRef<Walker> Prime(
		const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fRelater->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(ZVal_Any* ioResults)
		{ return fRelater->pReadInc(this, ioResults); }

	ZRef<Relater_Union> const fRelater;
	ZRef<Proxy> const fProxy;
	size_t fBaseOffset;

	DListIterator<PIP, DLink_PIP_InProxy> fIter_PIP;
	ZRef<QueryEngine::Result> fCurrentResult;
	size_t fCurrentIndex;
	};

// =================================================================================================
// MARK: - Relater_Union::ClientQuery

class Relater_Union::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Relater_Union::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Relater_Union::ClientQuery
:	public DLink_ClientQuery_NeedsWork
,	public DLink_ClientQuery_InPQuery
	{
public:
	ClientQuery(int64 iRefcon, PQuery* iPQuery)
	:	fRefcon(iRefcon)
	,	fPQuery(iPQuery)
		{}

	int64 const fRefcon;
	PQuery* const fPQuery;
	};

// =================================================================================================
// MARK: - Relater_Union::PQuery

class Relater_Union::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Union::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(const ZRef<RA::Expr_Rel>& iRel)
	:	fRel(iRel)
		{}

	ZRef<RA::Expr_Rel> const fRel;
	ZRef<RA::Expr_Rel> fRel_Analyzed;
	set<ZRef<Proxy> > fProxiesDependedUpon;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	ZRef<QueryEngine::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
// MARK: - Relater_Union::Visitor_DoMakeWalker

class Relater_Union::Visitor_DoMakeWalker
:	public virtual QueryEngine::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Proxy
	{
public:
	Visitor_DoMakeWalker(ZRef<Relater_Union> iRelater)
	:	fRelater(iRelater)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
		{
		// We'll never see a Concrete -- we're called only an a Rel that's been
		// analyzed and where any concrete or expression incorporating a concrete
		// will have been replaced with a Relater_Union::Proxy;
		ZUnimplemented();
		}

	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->pSetResult(fRelater->pMakeWalker(iExpr)); }

private:
	ZRef<Relater_Union> const fRelater;
	};

// =================================================================================================
// MARK: - Relater_Union::Analyze

class Relater_Union::Analyze
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<RA::Expr_Rel>
,	public virtual RA::Visitor_Expr_Rel_Calc
,	public virtual RA::Visitor_Expr_Rel_Embed
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual RA::Visitor_Expr_Rel_Const
,	public virtual RA::Visitor_Expr_Rel_Product
,	public virtual RA::Visitor_Expr_Rel_Project
,	public virtual RA::Visitor_Expr_Rel_Rename
,	public virtual RA::Visitor_Expr_Rel_Restrict
,	public virtual RA::Visitor_Expr_Rel_Union
	{
public:
	Analyze(Relater_Union* iRelater_Union, PQuery* iPQuery);

// From RA::Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Calc(const ZRef<RA::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<RA::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZRef<RA::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<RA::Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<RA::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<RA::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<RA::Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZRef<RA::Expr_Rel_Union>& iExpr);

// Our protocol
	ZRef<RA::Expr_Rel> TopLevelDo(ZRef<RA::Expr_Rel> iRel);

private:
	Relater_Union* fRelater_Union;
	PQuery* fPQuery;
	set<PRelater*> fPRelaters;
	RA::RelHead fResultRelHead;
	};

Relater_Union::Analyze::Analyze(Relater_Union* iRelater_Union, PQuery* iPQuery)
:	fRelater_Union(iRelater_Union)
,	fPQuery(iPQuery)
	{}

void Relater_Union::Analyze::Visit_Expr_Rel_Calc(const ZRef<RA::Expr_Rel_Calc>& iExpr)
	{
	RA::Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(iExpr);

	fResultRelHead |= iExpr->GetColName();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
	{
	ZAssertStop(kDebug, fResultRelHead.empty());

	fResultRelHead = RA::sRelHead(iExpr->GetConcreteHead());

	// Identify which PRelaters can service this concrete.
	fPRelaters = fRelater_Union->pIdentifyPRelaters(fResultRelHead);

	if (fPRelaters.size() <= 1)
		this->pSetResult(iExpr);
	else
		this->pSetResult(fRelater_Union->pGetProxy(fPQuery, fPRelaters, fResultRelHead, iExpr));
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Const(const ZRef<RA::Expr_Rel_Const>& iExpr)
	{
	RA::Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(iExpr);
	fResultRelHead |= iExpr->GetColName();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Embed(const ZRef<RA::Expr_Rel_Embed>& iExpr)
	{
	// Visit parent
	const ZRef<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters = fPRelaters;

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit embedee
	const ZRef<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PRelaters.
	set<PRelater*> rightPRelaters;
	rightPRelaters.swap(fPRelaters);
	RA::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPRelaters = leftPRelaters | rightPRelaters;
	fResultRelHead = leftRelHead | iExpr->GetColName();

	if (leftPRelaters.size() <= 1)
		{
		// Our left branch is simple, it references zero or one Relater.
		if (fPRelaters.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 Relater.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// With the addition of our right branch we *now* reference multiple Relaters.
			// We register a proxy for the left branch.
			ZRef<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet. Will
				// only happen if there's no restrict on the right branch.
				ZRef<RA::Expr_Rel> proxy1 =
					fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
				this->pSetResult(iExpr->Clone(proxy0, proxy1));
				}
			else
				{
				this->pSetResult(iExpr->Clone(proxy0, newOp1));
				}
			}
		}
	else
		{
		// Our left branch itself references multiples Relaters.
		if (rightPRelaters.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Product(const ZRef<RA::Expr_Rel_Product>& iExpr)
	{
	// Visit left branch
	const ZRef<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters;
	leftPRelaters.swap(fPRelaters);

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZRef<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PRelaters.
	set<PRelater*> rightPRelaters;
	rightPRelaters.swap(fPRelaters);
	RA::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPRelaters = leftPRelaters | rightPRelaters;
	fResultRelHead = leftRelHead | rightRelHead;

	if (leftPRelaters.size() <= 1)
		{
		// Our left branch is simple, it references zero or one Relater.
		if (fPRelaters.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 Relater.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// This is the interesting scenario. With the addition of our right branch
			// we *now* reference multiple Relaters. We register a proxy for the left branch.
			ZRef<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<RA::Expr_Rel> proxy1 =
					fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
				this->pSetResult(iExpr->Clone(proxy0, proxy1));
				}
			else
				{
				this->pSetResult(iExpr->Clone(proxy0, newOp1));
				}
			}
		}
	else
		{
		// Our left branch itself references multiples Relaters.
		if (rightPRelaters.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Project(const ZRef<RA::Expr_Rel_Project>& iExpr)
	{
	RA::Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(iExpr);

	fResultRelHead &= iExpr->GetProjectRelHead();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Rename(const ZRef<RA::Expr_Rel_Rename>& iExpr)
	{
	RA::Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	if (sQErase(fResultRelHead, iExpr->GetOld()))
		fResultRelHead |= iExpr->GetNew();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Restrict(const ZRef<RA::Expr_Rel_Restrict>& iExpr)
	{
	// If fPRelaters is not empty, then we're in an embed. Remember it.
	set<PRelater*> priorPRelaters = fPRelaters;

	const ZRef<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	if (fPRelaters.size() > 1)
		{
		// Child is complex, proxies will have been created already.
		fPRelaters = fPRelaters | priorPRelaters;
		this->pSetResult(iExpr->SelfOrClone(newOp0));
		}
	else
		{
		// Child is simple.
		set<PRelater*> combinedPRelaters = priorPRelaters | fPRelaters;
		if (combinedPRelaters.size() > 1)
			{
			// We're complex with the addition of our prior Relaters.
			// Create a proxy for the child.
			ZRef<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, fPRelaters, fResultRelHead, newOp0);
			this->pSetResult(iExpr->Clone(proxy0));
			}
		else
			{
			// We're still simple.
			this->pSetResult(iExpr->SelfOrClone(newOp0));
			}
		fPRelaters.swap(combinedPRelaters);
		}
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Union(const ZRef<RA::Expr_Rel_Union>& iExpr)
	{
	// Visit left branch
	const ZRef<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters;
	leftPRelaters.swap(fPRelaters);

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZRef<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PRelaters.
	set<PRelater*> rightPRelaters;
	rightPRelaters.swap(fPRelaters);
	RA::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPRelaters = leftPRelaters | rightPRelaters;
	if (leftRelHead != rightRelHead)
		{
		if (ZLOGF(w,eInfo))
			{
			w
				<< "leftRelHead and rightRelHead don't match, newOp0 and newOp1 must be incompatible"
				<< "\n" << "leftRelHead: " << leftRelHead
				<< "\n" << "newOp0: " << newOp0
				<< "\n" << "rightRelHead: " << rightRelHead
				<< "\n" << "newOp1: " << newOp1
			;
			}
		ZUnimplemented();
		}

	fResultRelHead = leftRelHead;

	if (leftPRelaters.size() <= 1)
		{
		// Our left branch is simple, it references zero or one Relater.
		if (fPRelaters.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 Relater.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// This is the interesting scenario. With the addition of our right branch
			// we *now* reference multiple Relaters. We register a proxy for the left branch.
			ZRef<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<RA::Expr_Rel> proxy1 =
					fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
				this->pSetResult(iExpr->Clone(proxy0, proxy1));
				}
			else
				{
				this->pSetResult(iExpr->Clone(proxy0, newOp1));
				}
			}
		}
	else
		{
		// Our left branch itself references multiples Relaters.
		if (rightPRelaters.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

ZRef<RA::Expr_Rel> Relater_Union::Analyze::TopLevelDo(ZRef<RA::Expr_Rel> iRel)
	{
	ZRef<RA::Expr_Rel> result = this->Do(iRel);
	if (fPRelaters.size() <= 1)
		return fRelater_Union->pGetProxy(fPQuery, fPRelaters, fResultRelHead, result);
	return result;
	}

// =================================================================================================
// MARK: - Relater_Union::PRelater

class Relater_Union::DLink_PRelater_NeedsWork
:	public DListLink<PRelater, DLink_PRelater_NeedsWork, kDebug>
	{};

class Relater_Union::DLink_PRelater_CollectFrom
:	public DListLink<PRelater, DLink_PRelater_CollectFrom, kDebug>
	{};

class Relater_Union::PRelater
:	public DLink_PRelater_NeedsWork
,	public DLink_PRelater_CollectFrom
	{
public:
	PRelater(ZRef<Relater> iRelater, const string8& iPrefix);

	bool Intersects(const RelHead& iRelHead);
	ZRef<RA::Expr_Rel> UsableRel(ZRef<RA::Expr_Rel> iRel);

	ZRef<Relater> fRelater;
	int64 fNextRefcon;

	Map_Refcon_PIP fMap_Refcon_PIP;
	DListHead<DLink_PIP_NeedsWork> fPIP_NeedsWork;

private:
	const string8 fPrefix;
	};

Relater_Union::PRelater::PRelater(ZRef<Relater> iRelater, const string8& iPrefix)
:	fRelater(iRelater)
,	fNextRefcon(1)
,	fPrefix(iPrefix)
	{}

bool Relater_Union::PRelater::Intersects(const RelHead& iRelHead)
	{
	if (fPrefix.empty())
		return fRelater->Intersects(iRelHead);
	else if (RA::sHasPrefix(fPrefix, iRelHead))
		return fRelater->Intersects(RA::sPrefixErased(fPrefix, iRelHead));

	return false;
	}

ZRef<RA::Expr_Rel> Relater_Union::PRelater::UsableRel(ZRef<RA::Expr_Rel> iRel)
	{
	if (fPrefix.empty())
		return iRel;

	return InsertPrefix(fPrefix).Do(iRel);
	}

// =================================================================================================
// MARK: - Relater_Union

Relater_Union::Relater_Union()
:	fEvent(Event::sZero())
	{}

Relater_Union::~Relater_Union()
	{}

void Relater_Union::Initialize()
	{
	Relater::Initialize();
	fCallable_ResultsAvailable = sCallable(sWeakRef(this), &Relater_Union::pResultsAvailable);
	}

bool Relater_Union::Intersects(const RelHead& iRelHead)
	{
	ZAcqMtxR acq(fMtxR);

	foreachi (iterRelater, fMap_Relater_PRelater)
		{
		if (iterRelater->first->Intersects(iRelHead))
			return true;
		}

	return false;
	}

void Relater_Union::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	// -----------------

	// Remove any Queries that need it
	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;

		if (ZLOGPF(s, eDebug + 1))
			s << "Remove: " << theRefcon;

		Map_Refcon_ClientQuery::iterator iterClientQuery =
			fMap_Refcon_ClientQuery.find(theRefcon);

		ClientQuery* theClientQuery = &iterClientQuery->second;
		sQErase(fClientQuery_NeedsWork, theClientQuery);

		PQuery* thePQuery = theClientQuery->fPQuery;

		sQErase(thePQuery->fClientQueries, theClientQuery);

		if (thePQuery->fClientQueries.IsEmpty())
			{
			foreachi (iterProxies, thePQuery->fProxiesDependedUpon)
				{
				ZRef<Proxy> theProxy = *iterProxies;
				sEraseMust(kDebug, theProxy->fDependentPQueries, thePQuery);
				if (theProxy->fDependentPQueries.empty())
					{
					for (DListEraser<PIP, DLink_PIP_InProxy>
						eraserPIP = theProxy->fPIP_InProxy; eraserPIP; eraserPIP.Advance())
						{
						PIP* thePIP = eraserPIP.Current();
						PRelater* thePRelater = thePIP->fPRelater;
						sQInsertBack(thePRelater->fPIP_NeedsWork, thePIP);
						sQInsertBack(fPRelater_NeedsWork, thePRelater);
						thePIP->fProxy = nullptr;
						}
					}
				}
			sQErase(fPQuery_NeedsWork, thePQuery);
			sEraseMust(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
			}

		fMap_Refcon_ClientQuery.erase(iterClientQuery);
		}

	// -----------------

	// Add any Queries
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> inPQuery =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &inPQuery.first->second;

		const int64 theRefcon = iAdded->GetRefcon();

		if (ZLOGPF(s, eDebug + 1))
			s << "Add: " << theRefcon;

		if (inPQuery.second)
			{
			if (ZLOGPF(s, eDebug + 1))
				s << "Raw:\n" << theRel;

			thePQuery->fRel_Analyzed = Analyze(this, thePQuery).TopLevelDo(theRel);
			sInsertBackMust(fPQuery_NeedsWork, thePQuery);

			if (ZLOGPF(s, eDebug + 1))
				s << "Analyzed:\n" << thePQuery->fRel_Analyzed;
			}

		ClientQuery* theClientQuery = &fMap_Refcon_ClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery))).first->second;

		sInsertBackMust(thePQuery->fClientQueries, theClientQuery);

		if (not inPQuery.second)
			sInsertBackMust(fClientQuery_NeedsWork, theClientQuery);
		}

	guard.Release();
	Relater::pTriggerResultsAvailable();
	}

void Relater_Union::CollectResults(vector<QueryResult>& oChanged)
	{
	Relater::pCollectResultsCalled();

	ZAcqMtxR acq(fMtxR);

	// -----------------

	for (DListEraser<PRelater, DLink_PRelater_NeedsWork>
		eraserPRelater = fPRelater_NeedsWork; eraserPRelater; eraserPRelater.Advance())
		{
		PRelater* thePRelater = eraserPRelater.Current();
		vector<AddedQuery> theAddedQueries;
		vector<int64> theRemoves;
		for (DListEraser<PIP, DLink_PIP_NeedsWork>
			eraserPIP = thePRelater->fPIP_NeedsWork; eraserPIP; eraserPIP.Advance())
			{
			PIP* thePIP = eraserPIP.Current();
			if (thePIP->fProxy)
				{
				if (sGetSet(thePIP->fNeedsAdd, false))
					{
					if (ZRef<Expr_Rel> usableRel = thePRelater->UsableRel(thePIP->fProxy->fRel))
						theAddedQueries.push_back(AddedQuery(thePIP->fRefcon, usableRel));
					}
				}
			else if (not thePIP->fNeedsAdd)
				{
				theRemoves.push_back(thePIP->fRefcon);
				thePRelater->fMap_Refcon_PIP.erase(thePIP->fRefcon);
				}
			}

		thePRelater->fRelater->ModifyRegistrations(
			sFirstOrNil(theAddedQueries), theAddedQueries.size(),
			sFirstOrNil(theRemoves), theRemoves.size());
		}

	// -----------------

	for (DListEraser<PRelater, DLink_PRelater_CollectFrom>
		eraserPRelater = fPRelater_CollectFrom; eraserPRelater; eraserPRelater.Advance())
		{ this->pCollectFrom(eraserPRelater.Current()); }

	// -----------------

	if (sNotEmpty(fPQuery_NeedsWork))
		{
		ZLOGPF(s, eDebug+1);

		const ZTime start = ZTime::sNow();

		for (DListEraser<PQuery, DLink_PQuery_NeedsWork>
			eraserPQuery = fPQuery_NeedsWork; eraserPQuery; eraserPQuery.Advance())
			{
			PQuery* thePQuery = eraserPQuery.Current();
			bool allOK = true;
			ZRef<Event> theEvent = Event::sZero();
			for (set<ZRef<Proxy> >::iterator iterProxy = thePQuery->fProxiesDependedUpon.begin();
				allOK && iterProxy != thePQuery->fProxiesDependedUpon.end(); ++iterProxy)
				{
				ZRef<Proxy> theProxy = *iterProxy;
				for (DListIterator<PIP, DLink_PIP_InProxy> iterPIP = theProxy->fPIP_InProxy;
					allOK && iterPIP; iterPIP.Advance())
					{
					PIP* thePIP = iterPIP.Current();
					if (thePIP->fResult)
						theEvent = theEvent->Joined(thePIP->fEvent);
					else
						allOK = false;
					}
				}

			if (allOK)
				{
				fWalkerCount = 0;
				fReadCount = 0;
				fStepCount = 0;

				const ZTime start = ZTime::sNow();

				ZRef<QueryEngine::Walker> theWalker =
					Relater_Union::Visitor_DoMakeWalker(this).Do(thePQuery->fRel_Analyzed);
				const ZTime afterMakeWalker = ZTime::sNow();

				thePQuery->fResult = QueryEngine::sResultFromWalker(theWalker);
				thePQuery->fEvent = theEvent;
				const ZTime afterDoQuery = ZTime::sNow();

				if (s)
					{
					s << "\n" << thePQuery->fRel_Analyzed
						<< "\nWalkerCount: " << fWalkerCount
						<< "\nReadCount: " << fReadCount
						<< "\nStepCount: " << fStepCount
						<< "\nMakeWalker: " << sStringf("%gms", 1000*(afterMakeWalker-start))
						<< "\nDoQuery: " << sStringf("%gms", 1000*(afterDoQuery-afterMakeWalker));
					}

				for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
					iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
					{ sQInsertBack(fClientQuery_NeedsWork, iterCS.Current()); }
				}
			}

		const ZTime afterCollect = ZTime::sNow();

		if (s)
			{
			s << "\nOverall Elapsed: " << sStringf("%gms", 1000*(afterCollect-start));
			}
		}

	for (DListEraser<ClientQuery, DLink_ClientQuery_NeedsWork>
		eraserClientQuery = fClientQuery_NeedsWork; eraserClientQuery; eraserClientQuery.Advance())
		{
		ClientQuery* theClientQuery = eraserClientQuery.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(
			QueryResult(theClientQuery->fRefcon, thePQuery->fResult, thePQuery->fEvent));
		}
	}

void Relater_Union::InsertRelater(ZRef<Relater> iRelater, const string8& iPrefix)
	{
	ZAcqMtxR acq(fMtxR);

	// We can't add Relaters on the fly -- require that no queries exist yet.
	ZAssertStop(kDebug, fMap_Refcon_ClientQuery.empty());

	sInsertMust(kDebug, fMap_Relater_PRelater, iRelater, PRelater(iRelater, iPrefix));

	iRelater->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);
	}

void Relater_Union::EraseRelater(ZRef<Relater> iRelater)
	{
	ZAcqMtxR acq(fMtxR);
	Map_Relater_PRelater::iterator iterRelater = fMap_Relater_PRelater.find(iRelater);
	PRelater& thePRelater = iterRelater->second;
	vector<int64> toRemove;
	for (Map_Refcon_PIP::iterator
		iterPIP = thePRelater.fMap_Refcon_PIP.begin(),
		endPIP = thePRelater.fMap_Refcon_PIP.end();
		iterPIP != endPIP; ++iterPIP)
		{
		toRemove.push_back(iterPIP->first);
		sEraseMust(iterPIP->second.fProxy->fPIP_InProxy, &iterPIP->second);
		}
	thePRelater.fMap_Refcon_PIP.clear();

	iRelater->ModifyRegistrations(nullptr, 0, sFirstOrNil(toRemove), toRemove.size());

	iRelater->SetCallable_ResultsAvailable(null);

	fMap_Relater_PRelater.erase(iterRelater);
	}

set<Relater_Union::PRelater*> Relater_Union::pIdentifyPRelaters(const RelHead& iRelHead)
	{
	set<PRelater*> result;
	for (Map_Relater_PRelater::iterator
		iterRelater = fMap_Relater_PRelater.begin(),
		endRelater = fMap_Relater_PRelater.end();
		iterRelater != endRelater; ++iterRelater)
		{
		PRelater* thePRelater = &iterRelater->second;
		if (thePRelater->Intersects(iRelHead))
			result.insert(thePRelater);
		}
	return result;
	}

ZRef<RA::Expr_Rel> Relater_Union::pGetProxy(PQuery* iPQuery,
	const set<PRelater*>& iPRelaters, const RelHead& iRelHead, ZRef<RA::Expr_Rel> iRel)
	{
	if (iPRelaters.empty())
		return iRel;

	ZRef<Proxy> theProxyRef;
	if (ZQ<Proxy*> theQ = sQGet(fMap_Rel_ProxyX, iRel))
		{
		theProxyRef = *theQ;
		ZAssert(iRelHead == theProxyRef->fResultRelHead);
		}
	else
		{
		Proxy* theProxy = new Proxy(this);
		theProxyRef = theProxy;
		theProxy->fRel = iRel;
		theProxy->fResultRelHead = iRelHead;
		sInsertMust(kDebug, fMap_Rel_ProxyX, iRel, theProxy);
		foreachi (iterPRelaters, iPRelaters)
			{
			PRelater* thePRelater = *iterPRelaters;

			const int64 theRefcon = thePRelater->fNextRefcon++;

			PIP* thePIP = &thePRelater->fMap_Refcon_PIP.insert(
				make_pair(theRefcon, PIP())).first->second;

			thePIP->fNeedsAdd = true;
			thePIP->fRefcon = theRefcon;
			thePIP->fProxy = theProxy;
			thePIP->fPRelater = thePRelater;
			sInsertBackMust(theProxy->fPIP_InProxy, thePIP);

			sInsertBackMust(thePRelater->fPIP_NeedsWork, thePIP);
			sQInsertBack(fPRelater_NeedsWork, thePRelater);
			}
		}

	theProxyRef->fDependentPQueries.insert(iPQuery);
	iPQuery->fProxiesDependedUpon.insert(theProxyRef);

	return theProxyRef;
	}

void Relater_Union::pFinalizeProxy(Proxy* iProxy)
	{
	if (not iProxy->FinishFinalize())
		return;

	ZAssertStop(kDebug, iProxy->fPIP_InProxy.IsEmpty());

	sEraseMust(kDebug, fMap_Rel_ProxyX, iProxy->fRel);

	delete iProxy;
	}

ZRef<Relater_Union::Walker_Proxy> Relater_Union::pMakeWalker(ZRef<Proxy> iProxy)
	{
	++fWalkerCount;
	return new Walker_Proxy(this, iProxy);
	}

void Relater_Union::pRewind(ZRef<Walker_Proxy> iWalker)
	{
	iWalker->fIter_PIP = iWalker->fProxy->fPIP_InProxy;
	iWalker->fCurrentResult.Clear();
	}

void Relater_Union::pPrime(ZRef<Walker_Proxy> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fBaseOffset = ioBaseOffset;
	foreachi (ii, iWalker->fProxy->fResultRelHead)
		oOffsets[*ii] = ioBaseOffset++;
	}

bool Relater_Union::pReadInc(ZRef<Walker_Proxy> iWalker, ZVal_Any* ioResults)
	{
	++fReadCount;
	for (;;)
		{
		++fStepCount;
		if (not iWalker->fCurrentResult)
			{
			if (not iWalker->fIter_PIP)
				return false;
			iWalker->fCurrentIndex = 0;
			iWalker->fCurrentResult = iWalker->fIter_PIP.Current()->fResult;
			}

		ZAssertStop(kDebug, iWalker->fCurrentResult);

		if (iWalker->fCurrentIndex >= iWalker->fCurrentResult->Count())
			{
			iWalker->fCurrentResult.Clear();
			iWalker->fIter_PIP.Advance();
			continue;
			}

		const RA::RelHead& theRH = iWalker->fCurrentResult->GetRelHead();
		if (theRH != iWalker->fProxy->fResultRelHead)
			{
			if (ZLOGPF(s, eDebug))
				{
				s	<< "\n" << theRH
					<< "\n" << iWalker->fProxy->fResultRelHead;
				}
			ZDebugStop(0);
			}
		size_t theOffset = iWalker->fBaseOffset;
		const ZVal_Any* theVals = iWalker->fCurrentResult->GetValsAt(iWalker->fCurrentIndex);
		foreachi (ii, theRH)
			ioResults[theOffset++] = *theVals++;
		++iWalker->fCurrentIndex;
		return true;
		}
	}

void Relater_Union::pCollectFrom(PRelater* iPRelater)
	{
	vector<QueryResult> theQueryResults;
	iPRelater->fRelater->CollectResults(theQueryResults);

	foreachi (iterQueryResults, theQueryResults)
		{
		const int64 theRefcon = iterQueryResults->GetRefcon();
		Map_Refcon_PIP::iterator iter = iPRelater->fMap_Refcon_PIP.find(theRefcon);
		if (iPRelater->fMap_Refcon_PIP.end() != iter)
			{
			PIP* thePIP = &iter->second;
			thePIP->fResult = iterQueryResults->GetResult();
			thePIP->fEvent = iterQueryResults->GetEvent();
			foreachi (ii, thePIP->fProxy->fDependentPQueries)
				sQInsertBack(fPQuery_NeedsWork, *ii);
			}
		}
	}

void Relater_Union::pResultsAvailable(ZRef<Relater> iRelater)
	{
	ZGuardMtxR guard(fMtxR);
	Map_Relater_PRelater::iterator iterRelater = fMap_Relater_PRelater.find(iRelater);
	sQInsertBack(fPRelater_CollectFrom, &iterRelater->second);
	guard.Release();
	Relater::pTriggerResultsAvailable();
	}

} // namespace Dataspace
} // namespace ZooLib
