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

#include "zoolib/dataspace/ZDataspace_Source_Union.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/QueryEngine/DoQuery.h"
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
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - InsertPrefix (anonymous)

namespace { // anonymous

class InsertPrefix
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<RelationalAlgebra::Expr_Rel>
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Concrete
	{
public:
	InsertPrefix(const string8& iPrefix);

// From RelationalAlgebra::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(const ZRef<RelationalAlgebra::Expr_Rel_Concrete>& iExpr);

private:
	const string8 fPrefix;
	};

InsertPrefix::InsertPrefix(const string8& iPrefix)
:	fPrefix(iPrefix)
	{}

void InsertPrefix::Visit_Expr_Rel_Concrete(const ZRef<RelationalAlgebra::Expr_Rel_Concrete>& iExpr)
	{
	const RelHead& theRelHead = iExpr->GetConcreteRelHead();

	const RelHead newRelHead = RelationalAlgebra::sPrefixErased(fPrefix, theRelHead);
	ZRef<RelationalAlgebra::Expr_Rel> theRel = RelationalAlgebra::sConcrete(newRelHead);
	foreachi (ii, newRelHead)
		theRel = sRename(theRel, RelationalAlgebra::sPrefixInserted(fPrefix, *ii), *ii);

	this->pSetResult(theRel);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - Source_Union::PIP

class Source_Union::DLink_PIP_InProxy
:	public DListLink<PIP, DLink_PIP_InProxy, kDebug>
	{};

class Source_Union::DLink_PIP_NeedsWork
:	public DListLink<PIP, DLink_PIP_NeedsWork, kDebug>
	{};

class Source_Union::PIP
:	public DLink_PIP_InProxy
,	public DLink_PIP_NeedsWork
	{
public:
	Proxy* fProxy;
	PSource* fPSource;
	bool fNeedsAdd;
	int64 fRefcon;
	ZRef<QueryEngine::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
// MARK: - Source_Union::Proxy declaration

class Source_Union::Proxy
:	public virtual RelationalAlgebra::Expr_Rel
,	public virtual ZExpr_Op0_T<RelationalAlgebra::Expr_Rel>
	{
	typedef ZExpr_Op0_T<Expr_Rel> inherited;
public:
	Proxy(Source_Union* iSource);

// From ZCounted
	virtual void Finalize();

// From ZVisitee
	virtual void Accept(const ZVisitor& iVisitor);

// From ZExpr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(ZVisitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZRef<RelationalAlgebra::Expr_Rel> Self();
	virtual ZRef<RelationalAlgebra::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Proxy(Visitor_Proxy& iVisitor);

	Source_Union* const fSource;
	ZRef<RelationalAlgebra::Expr_Rel> fRel;
	RelationalAlgebra::RelHead fResultRelHead;
	set<PQuery*> fDependentPQueries;

	// Something that ties this proxy to each Source.
	DListHead<DLink_PIP_InProxy> fPIP_InProxy;
	};

// =================================================================================================
// MARK: - Source_Union::Visitor_Proxy

class Source_Union::Visitor_Proxy
:	public virtual ZVisitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>
	{
public:
	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->Visit_Expr_Op0(iExpr); }
	};

// =================================================================================================
// MARK: - Source_Union::Proxy definition

Source_Union::Proxy::Proxy(Source_Union* iSource)
:	fSource(iSource)
	{}

void Source_Union::Proxy::Finalize()
	{ fSource->pFinalizeProxy(this); }

void Source_Union::Proxy::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Source_Union::Proxy::Accept_Expr_Op0(ZVisitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZRef<RelationalAlgebra::Expr_Rel> Source_Union::Proxy::Self()
	{ return this; }

ZRef<RelationalAlgebra::Expr_Rel> Source_Union::Proxy::Clone()
	{ return this; }

void Source_Union::Proxy::Accept_Proxy(Visitor_Proxy& iVisitor)
	{ iVisitor.Visit_Proxy(this); }

// =================================================================================================
// MARK: - Source_Union::Walker_Proxy

class Source_Union::Walker_Proxy : public QueryEngine::Walker
	{
public:
	Walker_Proxy(ZRef<Source_Union> iSource, ZRef<Proxy> iProxy)
	:	fSource(iSource)
	,	fProxy(iProxy)
	,	fIter_PIP(iProxy->fPIP_InProxy)
		{}

	virtual ~Walker_Proxy()
		{}

	virtual void Rewind()
		{ fSource->pRewind(this); }

	virtual ZRef<Walker> Prime(
		const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fSource->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(ZVal_Any* ioResults)
		{ return fSource->pReadInc(this, ioResults); }

	ZRef<Source_Union> const fSource;
	ZRef<Proxy> const fProxy;
	size_t fBaseOffset;

	DListIterator<PIP, DLink_PIP_InProxy> fIter_PIP;
	ZRef<QueryEngine::Result> fCurrentResult;
	size_t fCurrentIndex;
	};

// =================================================================================================
// MARK: - Source_Union::ClientQuery

class Source_Union::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Source_Union::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Source_Union::ClientQuery
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
// MARK: - Source_Union::PQuery

class Source_Union::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Source_Union::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(const ZRef<RelationalAlgebra::Expr_Rel>& iRel)
	:	fRel(iRel)
		{}

	ZRef<RelationalAlgebra::Expr_Rel> const fRel;
	ZRef<RelationalAlgebra::Expr_Rel> fRel_Analyzed;
	set<ZRef<Proxy> > fProxiesDependedUpon;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	ZRef<QueryEngine::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
// MARK: - Source_Union::Visitor_DoMakeWalker

class Source_Union::Visitor_DoMakeWalker
:	public virtual QueryEngine::Visitor_DoMakeWalker
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Proxy
	{
public:
	Visitor_DoMakeWalker(ZRef<Source_Union> iSource)
	:	fSource(iSource)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RelationalAlgebra::Expr_Rel_Concrete>& iExpr)
		{
		// We'll never see a Concrete -- we're called only an a Rel that's been
		// analyzed and where any concrete or expression incorporating a concrete
		// will have been replaced with a Source_Union::Proxy;
		ZUnimplemented();
		}

	virtual void Visit_Proxy(const ZRef<Proxy>& iExpr)
		{ this->pSetResult(fSource->pMakeWalker(iExpr)); }

private:
	ZRef<Source_Union> const fSource;
	};

// =================================================================================================
// MARK: - Source_Union::Analyze

class Source_Union::Analyze
:	public virtual ZVisitor_Expr_Op_Do_Transform_T<RelationalAlgebra::Expr_Rel>
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Calc
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Embed
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Concrete
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Const
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Product
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Project
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Rename
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Restrict
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Union
	{
public:
	Analyze(Source_Union* iSource_Union, PQuery* iPQuery);

// From RelationalAlgebra::Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Calc(const ZRef<RelationalAlgebra::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<RelationalAlgebra::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZRef<RelationalAlgebra::Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZRef<RelationalAlgebra::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<RelationalAlgebra::Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<RelationalAlgebra::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<RelationalAlgebra::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<RelationalAlgebra::Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZRef<RelationalAlgebra::Expr_Rel_Union>& iExpr);

// Our protocol
	ZRef<RelationalAlgebra::Expr_Rel> TopLevelDo(ZRef<RelationalAlgebra::Expr_Rel> iRel);

private:
	Source_Union* fSource_Union;
	PQuery* fPQuery;
	set<PSource*> fPSources;
	RelationalAlgebra::RelHead fResultRelHead;
	};

Source_Union::Analyze::Analyze(Source_Union* iSource_Union, PQuery* iPQuery)
:	fSource_Union(iSource_Union)
,	fPQuery(iPQuery)
	{}

void Source_Union::Analyze::Visit_Expr_Rel_Calc(const ZRef<RelationalAlgebra::Expr_Rel_Calc>& iExpr)
	{
	RelationalAlgebra::Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(iExpr);

	fResultRelHead |= iExpr->GetColName();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Concrete(const ZRef<RelationalAlgebra::Expr_Rel_Concrete>& iExpr)
	{
	ZAssertStop(kDebug, fResultRelHead.empty());

	fResultRelHead = iExpr->GetConcreteRelHead();

	// Identify which PSources can service this concrete.
	fPSources = fSource_Union->pIdentifyPSources(fResultRelHead);

	if (fPSources.size() <= 1)
		this->pSetResult(iExpr);
	else
		this->pSetResult(fSource_Union->pGetProxy(fPQuery, fPSources, fResultRelHead, iExpr));
	}

void Source_Union::Analyze::Visit_Expr_Rel_Const(const ZRef<RelationalAlgebra::Expr_Rel_Const>& iExpr)
	{
	RelationalAlgebra::Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(iExpr);
	fResultRelHead |= iExpr->GetColName();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Embed(const ZRef<RelationalAlgebra::Expr_Rel_Embed>& iExpr)
	{
	// Visit parent
	const ZRef<RelationalAlgebra::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PSources it touches.
	set<PSource*> leftPSources = fPSources;

	// And the relhead
	RelationalAlgebra::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit embedee
	const ZRef<RelationalAlgebra::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PSources.
	set<PSource*> rightPSources;
	rightPSources.swap(fPSources);
	RelationalAlgebra::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPSources = leftPSources | rightPSources;
	fResultRelHead = leftRelHead | iExpr->GetColName();

	if (leftPSources.size() <= 1)
		{
		// Our left branch is simple, it references zero or one source.
		if (fPSources.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 source.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// With the addition of our right branch we *now* reference multiple sources.
			// We register a proxy for the left branch.
			ZRef<RelationalAlgebra::Expr_Rel> proxy0 =
				fSource_Union->pGetProxy(fPQuery, leftPSources, leftRelHead, newOp0);

			if (rightPSources.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet. Will
				// only happen if there's no restrict on the right branch.
				ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
					fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
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
		// Our left branch itself references multiples sources.
		if (rightPSources.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
				fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Source_Union::Analyze::Visit_Expr_Rel_Product(const ZRef<RelationalAlgebra::Expr_Rel_Product>& iExpr)
	{
	// Visit left branch
	const ZRef<RelationalAlgebra::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PSources it touches.
	set<PSource*> leftPSources;
	leftPSources.swap(fPSources);

	// And the relhead
	RelationalAlgebra::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZRef<RelationalAlgebra::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PSources.
	set<PSource*> rightPSources;
	rightPSources.swap(fPSources);
	RelationalAlgebra::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPSources = leftPSources | rightPSources;
	fResultRelHead = leftRelHead | rightRelHead;

	if (leftPSources.size() <= 1)
		{
		// Our left branch is simple, it references zero or one source.
		if (fPSources.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 source.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// This is the interesting scenario. With the addition of our right branch
			// we *now* reference multiple sources. We register a proxy for the left branch.
			ZRef<RelationalAlgebra::Expr_Rel> proxy0 =
				fSource_Union->pGetProxy(fPQuery, leftPSources, leftRelHead, newOp0);

			if (rightPSources.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
					fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
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
		// Our left branch itself references multiples sources.
		if (rightPSources.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
				fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Source_Union::Analyze::Visit_Expr_Rel_Project(const ZRef<RelationalAlgebra::Expr_Rel_Project>& iExpr)
	{
	RelationalAlgebra::Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(iExpr);

	fResultRelHead &= iExpr->GetProjectRelHead();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Rename(const ZRef<RelationalAlgebra::Expr_Rel_Rename>& iExpr)
	{
	RelationalAlgebra::Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	if (sQErase(fResultRelHead, iExpr->GetOld()))
		fResultRelHead |= iExpr->GetNew();
	}

void Source_Union::Analyze::Visit_Expr_Rel_Restrict(const ZRef<RelationalAlgebra::Expr_Rel_Restrict>& iExpr)
	{
	// If fPSources is not empty, then we're in an embed. Remember it.
	set<PSource*> priorPSources = fPSources;

	const ZRef<RelationalAlgebra::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	if (fPSources.size() > 1)
		{
		// Child is complex, proxies will have been created already.
		fPSources = fPSources | priorPSources;
		this->pSetResult(iExpr->SelfOrClone(newOp0));
		}
	else
		{
		// Child is simple.
		set<PSource*> combinedPSources = priorPSources | fPSources;
		if (combinedPSources.size() > 1)
			{
			// We're complex with the addition of our prior sources.
			// Create a proxy for the child.
			ZRef<RelationalAlgebra::Expr_Rel> proxy0 =
				fSource_Union->pGetProxy(fPQuery, fPSources, fResultRelHead, newOp0);
			this->pSetResult(iExpr->Clone(proxy0));
			}
		else
			{
			// We're still simple.
			this->pSetResult(iExpr->SelfOrClone(newOp0));
			}
		fPSources.swap(combinedPSources);
		}
	}

void Source_Union::Analyze::Visit_Expr_Rel_Union(const ZRef<RelationalAlgebra::Expr_Rel_Union>& iExpr)
	{
	// Visit left branch
	const ZRef<RelationalAlgebra::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PSources it touches.
	set<PSource*> leftPSources;
	leftPSources.swap(fPSources);

	// And the relhead
	RelationalAlgebra::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZRef<RelationalAlgebra::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PSources.
	set<PSource*> rightPSources;
	rightPSources.swap(fPSources);
	RelationalAlgebra::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPSources = leftPSources | rightPSources;
	ZAssert(leftRelHead == rightRelHead);
	fResultRelHead = leftRelHead;

	if (leftPSources.size() <= 1)
		{
		// Our left branch is simple, it references zero or one source.
		if (fPSources.size() <= 1)
			{
			// And with the addition of the right branch we still reference <= 1 source.
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		else
			{
			// This is the interesting scenario. With the addition of our right branch
			// we *now* reference multiple sources. We register a proxy for the left branch.
			ZRef<RelationalAlgebra::Expr_Rel> proxy0 =
				fSource_Union->pGetProxy(fPQuery, leftPSources, leftRelHead, newOp0);

			if (rightPSources.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
					fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
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
		// Our left branch itself references multiples sources.
		if (rightPSources.size() <= 1)
			{
			// Right branch is simple, and thus won't have registered a proxy yet.
			ZRef<RelationalAlgebra::Expr_Rel> proxy1 =
				fSource_Union->pGetProxy(fPQuery, rightPSources, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

ZRef<RelationalAlgebra::Expr_Rel> Source_Union::Analyze::TopLevelDo(ZRef<RelationalAlgebra::Expr_Rel> iRel)
	{
	ZRef<RelationalAlgebra::Expr_Rel> result = this->Do(iRel);
	if (fPSources.size() <= 1)
		return fSource_Union->pGetProxy(fPQuery, fPSources, fResultRelHead, result);
	return result;
	}

// =================================================================================================
// MARK: - Source_Union::PSource

class Source_Union::DLink_PSource_NeedsWork
:	public DListLink<PSource, DLink_PSource_NeedsWork, kDebug>
	{};

class Source_Union::DLink_PSource_CollectFrom
:	public DListLink<PSource, DLink_PSource_CollectFrom, kDebug>
	{};

class Source_Union::PSource
:	public DLink_PSource_NeedsWork
,	public DLink_PSource_CollectFrom
	{
public:
	PSource(ZRef<Source> iSource, const string8& iPrefix);

	bool Intersects(const RelHead& iRelHead);
	ZRef<RelationalAlgebra::Expr_Rel> UsableRel(ZRef<RelationalAlgebra::Expr_Rel> iRel);

	ZRef<Source> fSource;
	int64 fNextRefcon;

	Map_Refcon_PIP fMap_Refcon_PIP;
	DListHead<DLink_PIP_NeedsWork> fPIP_NeedsWork;

private:
	const string8 fPrefix;
	};

Source_Union::PSource::PSource(ZRef<Source> iSource, const string8& iPrefix)
:	fSource(iSource)
,	fNextRefcon(1)
,	fPrefix(iPrefix)
	{}

bool Source_Union::PSource::Intersects(const RelHead& iRelHead)
	{
	if (fPrefix.empty())
		return fSource->Intersects(iRelHead);
	else if (RelationalAlgebra::sHasPrefix(fPrefix, iRelHead))
		return fSource->Intersects(RelationalAlgebra::sPrefixErased(fPrefix, iRelHead));

	return false;
	}

ZRef<RelationalAlgebra::Expr_Rel> Source_Union::PSource::UsableRel(ZRef<RelationalAlgebra::Expr_Rel> iRel)
	{
	if (fPrefix.empty())
		return iRel;

	return InsertPrefix(fPrefix).Do(iRel);
	}

// =================================================================================================
// MARK: - Source_Union

Source_Union::Source_Union()
:	fEvent(Event::sZero())
	{}

Source_Union::~Source_Union()
	{}

void Source_Union::Initialize()
	{
	Source::Initialize();
	fCallable_ResultsAvailable = sCallable(sWeakRef(this), &Source_Union::pResultsAvailable);
	}

bool Source_Union::Intersects(const RelHead& iRelHead)
	{
	ZAcqMtxR acq(fMtxR);

	foreachi (iterSource, fMap_Source_PSource)
		{
		if (iterSource->first->Intersects(iRelHead))
			return true;
		}

	return false;
	}

void Source_Union::ModifyRegistrations(
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
						PSource* thePSource = thePIP->fPSource;
						sQInsertBack(thePSource->fPIP_NeedsWork, thePIP);
						sQInsertBack(fPSource_NeedsWork, thePSource);
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
		ZRef<RelationalAlgebra::Expr_Rel> theRel = iAdded->GetRel();

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
	Source::pTriggerResultsAvailable();
	}

void Source_Union::CollectResults(vector<QueryResult>& oChanged)
	{
	this->pCollectResultsCalled();

	ZAcqMtxR acq(fMtxR);

	// -----------------

	for (DListEraser<PSource, DLink_PSource_NeedsWork>
		eraserPSource = fPSource_NeedsWork; eraserPSource; eraserPSource.Advance())
		{
		PSource* thePSource = eraserPSource.Current();
		vector<AddedQuery> theAddedQueries;
		vector<int64> theRemoves;
		for (DListEraser<PIP, DLink_PIP_NeedsWork>
			eraserPIP = thePSource->fPIP_NeedsWork; eraserPIP; eraserPIP.Advance())
			{
			PIP* thePIP = eraserPIP.Current();
			if (thePIP->fProxy)
				{
				if (thePIP->fNeedsAdd)
					{
					thePIP->fNeedsAdd = false;
					theAddedQueries.push_back(
						AddedQuery(thePIP->fRefcon, thePSource->UsableRel(thePIP->fProxy->fRel)));
					}
				}
			else
				{
				if (not thePIP->fNeedsAdd)
					{
					theRemoves.push_back(thePIP->fRefcon);
					thePSource->fMap_Refcon_PIP.erase(thePIP->fRefcon);
					}
				}
			}

		thePSource->fSource->ModifyRegistrations(
			sFirstOrNil(theAddedQueries), theAddedQueries.size(),
			sFirstOrNil(theRemoves), theRemoves.size());
		}

	// -----------------

	for (DListEraser<PSource, DLink_PSource_CollectFrom>
		eraserPSource = fPSource_CollectFrom; eraserPSource; eraserPSource.Advance())
		{ this->pCollectFrom(eraserPSource.Current()); }

	// -----------------

	if (not fPQuery_NeedsWork.IsEmpty())
		{
		ZLOGPF(s, eDebug+1);

		const ZTime start = ZTime::sNow();

		for (DListEraser<PQuery, DLink_PQuery_NeedsWork>
			eraserPQuery = fPQuery_NeedsWork; eraserPQuery; eraserPQuery.Advance())
			{
			PQuery* thePQuery = eraserPQuery.Current();
			bool allOK = true;
			ZRef<Event> theEvent;
			for (set<ZRef<Proxy> >::iterator iterProxy = thePQuery->fProxiesDependedUpon.begin();
				allOK && iterProxy != thePQuery->fProxiesDependedUpon.end(); ++iterProxy)
				{
				ZRef<Proxy> theProxy = *iterProxy;
				for (DListIterator<PIP, DLink_PIP_InProxy> iterPIP = theProxy->fPIP_InProxy;
					allOK && iterPIP; iterPIP.Advance())
					{
					PIP* thePIP = iterPIP.Current();
					if (not thePIP->fResult)
						{
						allOK = false;
						}
					else
						{
						if (theEvent)
							theEvent = theEvent->Joined(thePIP->fEvent);
						else
							theEvent = thePIP->fEvent;
						}
					}
				}

			if (allOK)
				{
				fWalkerCount = 0;
				fReadCount = 0;
				fStepCount = 0;

				const ZTime start = ZTime::sNow();

				ZRef<QueryEngine::Walker> theWalker =
					Source_Union::Visitor_DoMakeWalker(this).Do(thePQuery->fRel_Analyzed);
				const ZTime afterMakeWalker = ZTime::sNow();

				thePQuery->fResult = QueryEngine::sDoQuery(theWalker);
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

void Source_Union::InsertSource(ZRef<Source> iSource, const string8& iPrefix)
	{
	ZAcqMtxR acq(fMtxR);

	// We can't add sources on the fly -- require that no queries exist yet.
	ZAssertStop(kDebug, fMap_Refcon_ClientQuery.empty());

	sInsertMust(kDebug, fMap_Source_PSource, iSource, PSource(iSource, iPrefix));

	iSource->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);
	}

void Source_Union::EraseSource(ZRef<Source> iSource)
	{
	ZAcqMtxR acq(fMtxR);
	Map_Source_PSource::iterator iterSource = fMap_Source_PSource.find(iSource);
	PSource& thePSource = iterSource->second;
	vector<int64> toRemove;
	for (Map_Refcon_PIP::iterator
		iterPIP = thePSource.fMap_Refcon_PIP.begin(),
		endPIP = thePSource.fMap_Refcon_PIP.end();
		iterPIP != endPIP; ++iterPIP)
		{
		toRemove.push_back(iterPIP->first);
		sEraseMust(iterPIP->second.fProxy->fPIP_InProxy, &iterPIP->second);
		}
	thePSource.fMap_Refcon_PIP.clear();

	iSource->ModifyRegistrations(nullptr, 0, sFirstOrNil(toRemove), toRemove.size());

	iSource->SetCallable_ResultsAvailable(null);

	fMap_Source_PSource.erase(iterSource);
	}

set<Source_Union::PSource*> Source_Union::pIdentifyPSources(const RelHead& iRelHead)
	{
	set<PSource*> result;
	for (Map_Source_PSource::iterator
		iterSource = fMap_Source_PSource.begin(),
		endSource = fMap_Source_PSource.end();
		iterSource != endSource; ++iterSource)
		{
		PSource* thePSource = &iterSource->second;
		if (thePSource->Intersects(iRelHead))
			result.insert(thePSource);
		}
	return result;
	}

ZRef<RelationalAlgebra::Expr_Rel> Source_Union::pGetProxy(PQuery* iPQuery,
	const set<PSource*>& iPSources, const RelHead& iRelHead, ZRef<RelationalAlgebra::Expr_Rel> iRel)
	{
	if (iPSources.empty())
		return iRel;

	ZRef<Proxy> theProxyRef;
	if (ZQ<Proxy*> theQ = sQGet(fProxyMap, iRel))
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
		sInsertMust(kDebug, fProxyMap, iRel, theProxy);
		foreachi (iterPSources, iPSources)
			{
			PSource* thePSource = *iterPSources;

			const int64 theRefcon = thePSource->fNextRefcon++;

			PIP* thePIP = &thePSource->fMap_Refcon_PIP.insert(
				make_pair(theRefcon, PIP())).first->second;

			thePIP->fNeedsAdd = true;
			thePIP->fRefcon = theRefcon;
			thePIP->fProxy = theProxy;
			thePIP->fPSource = thePSource;
			sInsertBackMust(theProxy->fPIP_InProxy, thePIP);

			sInsertBackMust(thePSource->fPIP_NeedsWork, thePIP);
			sQInsertBack(fPSource_NeedsWork, thePSource);
			}
		}

	theProxyRef->fDependentPQueries.insert(iPQuery);
	iPQuery->fProxiesDependedUpon.insert(theProxyRef);

	return theProxyRef;
	}

void Source_Union::pFinalizeProxy(Proxy* iProxy)
	{
	if (not iProxy->FinishFinalize())
		return;

	ZAssertStop(kDebug, iProxy->fPIP_InProxy.IsEmpty());

	sEraseMust(kDebug, fProxyMap, iProxy->fRel);

	delete iProxy;
	}

ZRef<Source_Union::Walker_Proxy> Source_Union::pMakeWalker(ZRef<Proxy> iProxy)
	{
	++fWalkerCount;
	return new Walker_Proxy(this, iProxy);
	}

void Source_Union::pRewind(ZRef<Walker_Proxy> iWalker)
	{
	iWalker->fIter_PIP = iWalker->fProxy->fPIP_InProxy;
	iWalker->fCurrentResult.Clear();
	}

void Source_Union::pPrime(ZRef<Walker_Proxy> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fBaseOffset = ioBaseOffset;
	foreachi (ii, iWalker->fProxy->fResultRelHead)
		oOffsets[*ii] = ioBaseOffset++;
	}

bool Source_Union::pReadInc(ZRef<Walker_Proxy> iWalker,ZVal_Any* ioResults)
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

		const RelationalAlgebra::RelHead& theRH = iWalker->fCurrentResult->GetRelHead();
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

void Source_Union::pCollectFrom(PSource* iPSource)
	{
	vector<QueryResult> theQueryResults;
	iPSource->fSource->CollectResults(theQueryResults);

	foreachi (iterQueryResults, theQueryResults)
		{
		const int64 theRefcon = iterQueryResults->GetRefcon();
		Map_Refcon_PIP::iterator iter = iPSource->fMap_Refcon_PIP.find(theRefcon);
		if (iPSource->fMap_Refcon_PIP.end() != iter)
			{
			PIP* thePIP = &iter->second;
			thePIP->fResult = iterQueryResults->GetResult();
			thePIP->fEvent = iterQueryResults->GetEvent();
			foreachi (ii, thePIP->fProxy->fDependentPQueries)
				sQInsertBack(fPQuery_NeedsWork, *ii);
			}
		}
	}

void Source_Union::pResultsAvailable(ZRef<Source> iSource)
	{
	ZGuardMtxR guard(fMtxR);
	Map_Source_PSource::iterator iterSource = fMap_Source_PSource.find(iSource);
	sQInsertBack(fPSource_CollectFrom, &iterSource->second);
	guard.Release();
	Source::pTriggerResultsAvailable();
	}

} // namespace ZDataspace
} // namespace ZooLib
