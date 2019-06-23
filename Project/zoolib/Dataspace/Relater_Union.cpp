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

#include "zoolib/Dataspace/Relater_Union.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Log.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Dataspace/Util_Strim.h"

#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_GetNames.h"
#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_Do_GetNames.h"

namespace ZooLib {
namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace Util_STL;

namespace RA = RelationalAlgebra;

// =================================================================================================
#pragma mark - InsertPrefix (anonymous)

namespace { // anonymous

class InsertPrefix
:	public virtual Visitor_Expr_Op_Do_Transform_T<RA::Expr_Rel>
,	public virtual RA::Visitor_Expr_Rel_Concrete
	{
public:
	InsertPrefix(const string8& iPrefix);

// From RA::Visitor_Expr_Rel_Concrete
	virtual void Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr);

private:
	const string8 fPrefix;
	};

InsertPrefix::InsertPrefix(const string8& iPrefix)
:	fPrefix(iPrefix)
	{}

void InsertPrefix::Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr)
	{
	const RelHead& theRelHead = RA::sRelHead(iExpr->GetConcreteHead());

	if (const ZQ<RelHead> theQ = RA::sQPrefixErased(fPrefix, theRelHead))
		{
		ZP<RA::Expr_Rel> theRel = RA::sConcrete(*theQ);
		foreacha (entry, *theQ)
			theRel = sRename(theRel, RA::sPrefixInserted(fPrefix, entry), entry);

		this->pSetResult(theRel);
		}
	else
		{
		// The prefix does not exist on every ColName, so we can't produce a valid Rel.
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Relater_Union::PIP

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
	bool fWasAdded;
	int64 fRefcon;
	ZP<QueryEngine::Result> fResult;
	};

// =================================================================================================
#pragma mark - Relater_Union::Proxy declaration

class Relater_Union::Proxy
:	public virtual RA::Expr_Rel
,	public virtual Expr_Op0_T<RA::Expr_Rel>
	{
	typedef Expr_Op0_T<Expr_Rel> inherited;
public:
	Proxy(Relater_Union* iRelater);

// From Counted
	virtual void Finalize();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// From Expr
	virtual std::string DebugDescription();

// From Expr_Op0_T<Expr_Rel>
	virtual void Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor);

	virtual ZP<RA::Expr_Rel> Self();
	virtual ZP<RA::Expr_Rel> Clone();

// Our protocol
	virtual void Accept_Proxy(Visitor_Proxy& iVisitor);

	Relater_Union* const fRelater;
	ZP<RA::Expr_Rel> fRel;
	RA::RelHead fResultRelHead;
	set<PQuery*> fDependentPQueries;

	// Something that ties this proxy to each Relater.
	DListHead<DLink_PIP_InProxy> fPIP_InProxy;
	};

// =================================================================================================
#pragma mark - Relater_Union::Visitor_Proxy

class Relater_Union::Visitor_Proxy
:	public virtual Visitor_Expr_Op0_T<RA::Expr_Rel>
	{
public:
	virtual void Visit_Proxy(const ZP<Proxy>& iExpr)
		{ this->Visit_Expr_Op0(iExpr); }
	};

// =================================================================================================
#pragma mark - Relater_Union::Proxy definition

Relater_Union::Proxy::Proxy(Relater_Union* iRelater)
:	fRelater(iRelater)
	{}

void Relater_Union::Proxy::Finalize()
	{ fRelater->pFinalizeProxy(this); }

void Relater_Union::Proxy::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

std::string Relater_Union::Proxy::DebugDescription()
	{
	string8 result;
	ChanW_UTF_string8(&result) << "Proxy(" << fRel << ")";
	return result;
	}

void Relater_Union::Proxy::Accept_Expr_Op0(Visitor_Expr_Op0_T<RA::Expr_Rel>& iVisitor)
	{
	if (Visitor_Proxy* theVisitor = sDynNonConst<Visitor_Proxy>(&iVisitor))
		this->Accept_Proxy(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<RA::Expr_Rel> Relater_Union::Proxy::Self()
	{ return this; }

ZP<RA::Expr_Rel> Relater_Union::Proxy::Clone()
	{ return this; }

void Relater_Union::Proxy::Accept_Proxy(Visitor_Proxy& iVisitor)
	{ iVisitor.Visit_Proxy(this); }

// =================================================================================================
#pragma mark - Relater_Union::Walker_Proxy

class Relater_Union::Walker_Proxy : public QueryEngine::Walker
	{
public:
	Walker_Proxy(ZP<Relater_Union> iRelater, ZP<Proxy> iProxy)
	:	fRelater(iRelater)
	,	fProxy(iProxy)
	,	fIter_PIP(iProxy->fPIP_InProxy)
		{}

	virtual ~Walker_Proxy()
		{}

	virtual void Rewind()
		{
		this->Called_Rewind();
		fRelater->pRewind(this);
		}

	virtual ZP<Walker> Prime(
		const map<string8,size_t>& iOffsets,
		map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset)
		{
		fRelater->pPrime(this, iOffsets, oOffsets, ioBaseOffset);
		return this;
		}

	virtual bool QReadInc(Val_Any* ioResults)
		{
		this->Called_QReadInc();
		return fRelater->pReadInc(this, ioResults);
		}

	ZP<Relater_Union> const fRelater;
	ZP<Proxy> const fProxy;
	size_t fBaseOffset;

	DListIterator<PIP, DLink_PIP_InProxy> fIter_PIP;
	ZP<QueryEngine::Result> fCurrentResult;
	size_t fCurrentIndex;
	};

// =================================================================================================
#pragma mark - Relater_Union::ClientQuery

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
#pragma mark - Relater_Union::PQuery

class Relater_Union::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Union::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(const ZP<RA::Expr_Rel>& iRel)
	:	fRel(iRel)
		{}

	ZP<RA::Expr_Rel> const fRel;
	ZP<RA::Expr_Rel> fRel_Analyzed;
	set<ZP<Proxy>> fProxiesDependedUpon;
	DListHead<DLink_ClientQuery_InPQuery> fClientQueries;
	ZP<QueryEngine::Result> fResult;
	};

// =================================================================================================
#pragma mark - Relater_Union::Visitor_DoMakeWalker

class Relater_Union::Visitor_DoMakeWalker
:	public virtual QueryEngine::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Proxy
	{
public:
	Visitor_DoMakeWalker(ZP<Relater_Union> iRelater)
	:	fRelater(iRelater)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr)
		{
		// We'll never see a Concrete -- we're called only an a Rel that's been
		// analyzed and where any concrete or expression incorporating a concrete
		// will have been replaced with a Relater_Union::Proxy;
		ZUnimplemented();
		}

	virtual void Visit_Proxy(const ZP<Proxy>& iExpr)
		{ this->pSetResult(fRelater->pMakeWalker(iExpr)); }

private:
	ZP<Relater_Union> const fRelater;
	};

// =================================================================================================
#pragma mark - Relater_Union::Analyze

class Relater_Union::Analyze
:	public virtual Visitor_Expr_Op_Do_Transform_T<RA::Expr_Rel>
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
	virtual void Visit_Expr_Rel_Calc(const ZP<RA::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<RA::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZP<RA::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<RA::Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZP<RA::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<RA::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<RA::Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZP<RA::Expr_Rel_Union>& iExpr);

// Our protocol
	ZP<RA::Expr_Rel> TopLevelDo(ZP<RA::Expr_Rel> iRel);

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

void Relater_Union::Analyze::Visit_Expr_Rel_Calc(const ZP<RA::Expr_Rel_Calc>& iExpr)
	{
	RA::Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(iExpr);

	fResultRelHead |= iExpr->GetColName();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Concrete(const ZP<RA::Expr_Rel_Concrete>& iExpr)
	{
	ZAssertStop(kDebug, fResultRelHead.empty());

	fResultRelHead = RA::sRelHead(iExpr->GetConcreteHead());

	// Identify which PRelaters can service this concrete.
	fPRelaters = fRelater_Union->pIdentifyPRelaters(fResultRelHead);

	if (fPRelaters.size() == 0)
		{
		// There's no relater for this concrete, so there's no rel.
		}
	else if (fPRelaters.size() <= 1)
		{ this->pSetResult(iExpr); }
	else
		{ this->pSetResult(fRelater_Union->pGetProxy(fPQuery, fPRelaters, fResultRelHead, iExpr)); }
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Const(const ZP<RA::Expr_Rel_Const>& iExpr)
	{
	RA::Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(iExpr);
	fResultRelHead |= iExpr->GetColName();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Embed(const ZP<RA::Expr_Rel_Embed>& iExpr)
	{
	// Visit parent
	const ZP<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters = fPRelaters;

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit embedee
	const ZP<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

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
			ZP<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet. Will
				// only happen if there's no restrict on the right branch.
				ZP<RA::Expr_Rel> proxy1 =
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
			ZP<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Product(const ZP<RA::Expr_Rel_Product>& iExpr)
	{
	// Visit left branch
	const ZP<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters;
	leftPRelaters.swap(fPRelaters);

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZP<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

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
			ZP<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZP<RA::Expr_Rel> proxy1 =
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
			ZP<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Project(const ZP<RA::Expr_Rel_Project>& iExpr)
	{
	RA::Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(iExpr);

	fResultRelHead &= iExpr->GetProjectRelHead();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Rename(const ZP<RA::Expr_Rel_Rename>& iExpr)
	{
	RA::Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(iExpr);

	if (sQErase(fResultRelHead, iExpr->GetOld()))
		fResultRelHead |= iExpr->GetNew();
	}

void Relater_Union::Analyze::Visit_Expr_Rel_Restrict(const ZP<RA::Expr_Rel_Restrict>& iExpr)
	{
	// If fPRelaters is not empty, then we're in an embed. Remember it.
	set<PRelater*> priorPRelaters = fPRelaters;

	const ZP<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

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
			ZP<RA::Expr_Rel> proxy0 =
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

void Relater_Union::Analyze::Visit_Expr_Rel_Union(const ZP<RA::Expr_Rel_Union>& iExpr)
	{
	// Visit left branch
	const ZP<RA::Expr_Rel> newOp0 = this->Do(iExpr->GetOp0());

	// Remember which PRelaters it touches.
	set<PRelater*> leftPRelaters;
	leftPRelaters.swap(fPRelaters);

	// And the relhead
	RA::RelHead leftRelHead;
	leftRelHead.swap(fResultRelHead);

	// Visit right branch
	const ZP<RA::Expr_Rel> newOp1 = this->Do(iExpr->GetOp1());

	// Remember its PRelaters.
	set<PRelater*> rightPRelaters;
	rightPRelaters.swap(fPRelaters);
	RA::RelHead rightRelHead;
	rightRelHead.swap(fResultRelHead);

	fPRelaters = leftPRelaters | rightPRelaters;
	if (leftRelHead != rightRelHead)
		{
		if (ZLOGPF(w, eErr))
			{
			w
				<< "leftRelHead and rightRelHead don't match, newOp0 and newOp1 are incompatible"
				<< "\n" << "leftRelHead: " << leftRelHead
				<< "\n" << "newOp0: " << newOp0
				<< "\n" << "rightRelHead: " << rightRelHead
				<< "\n" << "newOp1: " << newOp1
			;
			}
		ZUnimplemented();
		}

	fResultRelHead = leftRelHead;

	if (not newOp0)
		{ this->pSetResult(newOp1); }
	else if (not newOp1)
		{ this->pSetResult(newOp0); }
	else if (leftPRelaters.size() <= 1)
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
			ZP<RA::Expr_Rel> proxy0 =
				fRelater_Union->pGetProxy(fPQuery, leftPRelaters, leftRelHead, newOp0);

			if (rightPRelaters.size() <= 1)
				{
				// Right branch is simple, and thus won't have registered a proxy yet.
				ZP<RA::Expr_Rel> proxy1 =
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
			ZP<RA::Expr_Rel> proxy1 =
				fRelater_Union->pGetProxy(fPQuery, rightPRelaters, rightRelHead, newOp1);
			this->pSetResult(iExpr->Clone(newOp0, proxy1));
			}
		else
			{
			this->pSetResult(iExpr->SelfOrClone(newOp0, newOp1));
			}
		}
	}

ZP<RA::Expr_Rel> Relater_Union::Analyze::TopLevelDo(ZP<RA::Expr_Rel> iRel)
	{
	ZP<RA::Expr_Rel> result = this->Do(iRel);
	if (fPRelaters.size() <= 1)
		return fRelater_Union->pGetProxy(fPQuery, fPRelaters, fResultRelHead, result);
	return result;
	}

// =================================================================================================
#pragma mark - Relater_Union::PRelater

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
	PRelater(ZP<Relater> iRelater, const string8& iPrefix);

	bool Intersects(const RelHead& iRelHead);
	ZP<RA::Expr_Rel> UsableRel(ZP<RA::Expr_Rel> iRel);

	ZP<Relater> fRelater;
	int64 fNextRefcon;

	Map_Refcon_PIP fMap_Refcon_PIP;
	DListHead<DLink_PIP_NeedsWork> fPIP_NeedsWork;

private:
	const string8 fPrefix;
	};

Relater_Union::PRelater::PRelater(ZP<Relater> iRelater, const string8& iPrefix)
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

ZP<RA::Expr_Rel> Relater_Union::PRelater::UsableRel(ZP<RA::Expr_Rel> iRel)
	{
	if (fPrefix.empty())
		return iRel;

	return InsertPrefix(fPrefix).Do(iRel);
	}

// =================================================================================================
#pragma mark - Relater_Union

Relater_Union::Relater_Union()
	{}

Relater_Union::~Relater_Union()
	{}

void Relater_Union::Initialize()
	{
	Relater::Initialize();
	fCallable_RelaterResultsAvailable = sCallable(sWeakRef(this), &Relater_Union::pResultsAvailable);
	}

bool Relater_Union::Intersects(const RelHead& iRelHead)
	{
	ZAcqMtx acq(fMtx);

	foreacha (entry, fMap_Relater_PRelater)
		{
		if (entry.first->Intersects(iRelHead))
			return true;
		}

	return false;
	}

void Relater_Union::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtx acq(fMtx);

	// -----------------

	// Remove any Queries that need it
	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;

		Map_Refcon_ClientQuery::iterator iterClientQuery =
			fMap_Refcon_ClientQuery.find(theRefcon);

		ClientQuery* theClientQuery = &iterClientQuery->second;
		sQErase(fClientQuery_NeedsWork, theClientQuery);

		PQuery* thePQuery = theClientQuery->fPQuery;

		sQErase(thePQuery->fClientQueries, theClientQuery);

		if (thePQuery->fClientQueries.IsEmpty())
			{
			foreacha (theProxy, thePQuery->fProxiesDependedUpon)
				{
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
		ZP<RA::Expr_Rel> theRel = iAdded->GetRel();

		pair<Map_Rel_PQuery::iterator,bool> inPQuery =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		PQuery* thePQuery = &inPQuery.first->second;

		const int64 theRefcon = iAdded->GetRefcon();

		if (inPQuery.second)
			{
			thePQuery->fRel_Analyzed = Analyze(this, thePQuery).TopLevelDo(theRel);
			sInsertBackMust(fPQuery_NeedsWork, thePQuery);
			}

		ClientQuery* theClientQuery = &fMap_Refcon_ClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery))).first->second;

		sInsertBackMust(thePQuery->fClientQueries, theClientQuery);

		if (not inPQuery.second)
			sInsertBackMust(fClientQuery_NeedsWork, theClientQuery);
		}

	ZRelMtx rel(fMtx);
	Relater::pTrigger_RelaterResultsAvailable();
	}

void Relater_Union::CollectResults(vector<QueryResult>& oChanged, int64& oChangeCount)
	{
	Relater::pCalled_RelaterCollectResults();

	oChangeCount = 0xDEADBEEF2;

	ZAcqMtx acq(fMtx);

	// -----------------

	// We have to call out to the underlying relater, which can actually call back into us,
	// so we have to be able to release fMtx before making the call.
	while (sNotEmpty(fPRelater_NeedsWork))
		{
		PRelater* thePRelater = sGetEraseFront<PRelater>(fPRelater_NeedsWork);
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
					if (ZP<Expr_Rel> theUsableRel = thePRelater->UsableRel(thePIP->fProxy->fRel))
						{
						theAddedQueries.push_back(AddedQuery(thePIP->fRefcon, theUsableRel));
						thePIP->fWasAdded = true;
						}
					}
				}
			else if (not thePIP->fNeedsAdd and thePIP->fWasAdded)
				{
				theRemoves.push_back(thePIP->fRefcon);
				}
			}
		ZP<Relater> theRelater = thePRelater->fRelater;
		ZRelMtx rel(fMtx);

		theRelater->ModifyRegistrations(
			sFirstOrNil(theAddedQueries), theAddedQueries.size(),
			sFirstOrNil(theRemoves), theRemoves.size());
		}

	// -----------------

	while (sNotEmpty(fPRelater_CollectFrom))
		{
		PRelater* thePRelater = sGetEraseFront<PRelater>(fPRelater_CollectFrom);
		ZP<Relater> theRelater = thePRelater->fRelater;
		vector<QueryResult> theQueryResults;
		{
		ZRelMtx rel(fMtx);
		int64 theCC;
		theRelater->CollectResults(theQueryResults, theCC);
		}

		// It's feasible that thePRelater got whacked while we were unlocked. Not sure what to do about it.
		foreacha (entry, theQueryResults)
			{
			const int64 theRefcon = entry.GetRefcon();
			if (PIP* thePIP = sPMut(thePRelater->fMap_Refcon_PIP, theRefcon))
				{
				thePIP->fResult = entry.GetResult();
				foreacha (entryPQuery, thePIP->fProxy->fDependentPQueries)
					sQInsertBack(fPQuery_NeedsWork, entryPQuery);
				}
			}
		}

	// -----------------

	if (sNotEmpty(fPQuery_NeedsWork))
		{
		for (DListEraser<PQuery, DLink_PQuery_NeedsWork>
			eraserPQuery = fPQuery_NeedsWork; eraserPQuery; eraserPQuery.Advance())
			{
			PQuery* thePQuery = eraserPQuery.Current();
			bool allOK = true;
			for (set<ZP<Proxy>>::iterator iterProxy = thePQuery->fProxiesDependedUpon.begin();
				allOK && iterProxy != thePQuery->fProxiesDependedUpon.end(); ++iterProxy)
				{
				ZP<Proxy> theProxy = *iterProxy;
				for (DListIterator<PIP, DLink_PIP_InProxy> iterPIP = theProxy->fPIP_InProxy;
					allOK && iterPIP; iterPIP.Advance())
					{
					PIP* thePIP = iterPIP.Current();
					if (not thePIP->fResult)
						allOK = false;
					}
				}

			if (allOK)
				{
				fWalkerCount = 0;
				fReadCount = 0;
				fStepCount = 0;

				ZP<QueryEngine::Walker> theWalker =
					Relater_Union::Visitor_DoMakeWalker(this).Do(thePQuery->fRel_Analyzed);

				thePQuery->fResult = QueryEngine::sResultFromWalker(theWalker);

				for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
					iterCS = thePQuery->fClientQueries; iterCS; iterCS.Advance())
					{ sQInsertBack(fClientQuery_NeedsWork, iterCS.Current()); }
				}
			}
		}

	for (DListEraser<ClientQuery, DLink_ClientQuery_NeedsWork>
		eraserClientQuery = fClientQuery_NeedsWork; eraserClientQuery; eraserClientQuery.Advance())
		{
		ClientQuery* theClientQuery = eraserClientQuery.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(
			QueryResult(theClientQuery->fRefcon, thePQuery->fResult));
		}
	}

void Relater_Union::InsertRelater(ZP<Relater> iRelater, const string8& iPrefix)
	{
	ZAcqMtx acq(fMtx);

	// We can't add Relaters on the fly -- require that no queries exist yet.
	ZAssertStop(kDebug, fMap_Refcon_ClientQuery.empty());

	sInsertMust(kDebug, fMap_Relater_PRelater, iRelater, PRelater(iRelater, iPrefix));

	iRelater->SetCallable_RelaterResultsAvailable(fCallable_RelaterResultsAvailable);
	}

void Relater_Union::EraseRelater(ZP<Relater> iRelater)
	{
	ZAcqMtx acq(fMtx);
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

	iRelater->SetCallable_RelaterResultsAvailable(null);

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

ZP<RA::Expr_Rel> Relater_Union::pGetProxy(PQuery* iPQuery,
	const set<PRelater*>& iPRelaters, const RelHead& iRelHead, ZP<RA::Expr_Rel> iRel)
	{
	if (iPRelaters.empty())
		return iRel;

	ZP<Proxy> theProxyRef;
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
		foreacha (thePRelater, iPRelaters)
			{
			const int64 theRefcon = thePRelater->fNextRefcon++;

			PIP* thePIP = &thePRelater->fMap_Refcon_PIP.insert(
				make_pair(theRefcon, PIP())).first->second;

			thePIP->fNeedsAdd = true;
			thePIP->fWasAdded = false;
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

ZP<Relater_Union::Walker_Proxy> Relater_Union::pMakeWalker(ZP<Proxy> iProxy)
	{
	++fWalkerCount;
	return new Walker_Proxy(this, iProxy);
	}

void Relater_Union::pRewind(ZP<Walker_Proxy> iWalker)
	{
	iWalker->fIter_PIP = iWalker->fProxy->fPIP_InProxy;
	iWalker->fCurrentResult.Clear();
	}

void Relater_Union::pPrime(ZP<Walker_Proxy> iWalker,
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	iWalker->fBaseOffset = ioBaseOffset;
	foreacha (entry, iWalker->fProxy->fResultRelHead)
		oOffsets[entry] = ioBaseOffset++;
	}

bool Relater_Union::pReadInc(ZP<Walker_Proxy> iWalker, Val_Any* ioResults)
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
			if (ZLOGPF(s, eErr))
				{
				s	<< "\n" << theRH
					<< "\n" << iWalker->fProxy->fResultRelHead;
				}
			ZUnimplemented();
			}
		size_t theOffset = iWalker->fBaseOffset;
		const Val_Any* theVals = iWalker->fCurrentResult->GetValsAt(iWalker->fCurrentIndex);

		for (size_t xx = 0, count = theRH.size(); xx < count; ++xx)
			ioResults[theOffset++] = *theVals++;
		++iWalker->fCurrentIndex;
		return true;
		}
	}

void Relater_Union::pResultsAvailable(ZP<Relater> iRelater)
	{
	{
	ZAcqMtx acq(fMtx);
	Map_Relater_PRelater::iterator iterRelater = fMap_Relater_PRelater.find(iRelater);
	sQInsertBack(fPRelater_CollectFrom, &iterRelater->second);
	}
	Relater::pTrigger_RelaterResultsAvailable();
	}

} // namespace Dataspace
} // namespace ZooLib
