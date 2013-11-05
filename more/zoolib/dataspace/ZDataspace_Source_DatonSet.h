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

#ifndef __ZDataspace_Source_DatonSet_h__
#define __ZDataspace_Source_DatonSet_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/ZVal_Any.h"

#include "zoolib/datonset/ZDatonSet.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include "zoolib/zqe/ZQE_Expr_Rel_Search.h"
#include "zoolib/zqe/ZQE_Walker.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

ZVal_Any sAsVal(const ZDatonSet::Daton& iDaton);
ZDatonSet::Daton sAsDaton(const ZVal_Any& iVal);

// =================================================================================================
// MARK: - Annotation_Daton

class Annotation_Daton : public ZCounted
	{
public:
	Annotation_Daton(const ZDatonSet::Daton& iDaton)
	:	fDaton(iDaton)
		{}

	const ZDatonSet::Daton& GetDaton() { return fDaton; }

private:
	const ZDatonSet::Daton fDaton;
	};

// =================================================================================================
// MARK: - Source_DatonSet

class Source_DatonSet : public Source
	{
public:
	enum { kDebug = 1 };

	void ForceUpdate();

	// Some kind of index specs to be passed in too.
	Source_DatonSet(ZRef<ZDatonSet::DatonSet> iDatonSet);
	virtual ~Source_DatonSet();

// From Source
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

// Our protocol
	ZRef<ZDatonSet::DatonSet> GetDatonSet();

	ZRef<Event> Insert(const ZDatonSet::Daton& iDaton);
	ZRef<Event> Erase(const ZDatonSet::Daton& iDaton);
	ZRef<Event> Replace(const ZDatonSet::Daton& iOld, const ZDatonSet::Daton& iNew);

	size_t OpenTransaction();
	void ClearTransaction(size_t iIndex);
	void CloseTransaction(size_t iIndex);

private:
	ZMtxR fMtxR;
	size_t fWalkerCount;
	size_t fReadCount;
	size_t fStepCount;

	void pPull();
	ZRef<Event> pConditionalPushDown();
	void pModify(const ZDatonSet::Daton& iDaton, const ZVal_Any& iVal, bool iSense);
	void pChanged(const ZVal_Any& iVal);
	void pChangedAll();

	class PQuery;

	class Visitor_DoMakeWalker;
	friend class Visitor_DoMakeWalker;

	// -----

	class Walker_Concrete;
	friend class Walker_Concrete;

	ZRef<ZQE::Walker> pMakeWalker_Concrete(PQuery* iPQuery, const RelHead& iRelHead);

	void pRewind_Concrete(ZRef<Walker_Concrete> iWalker);

	void pPrime_Concrete(ZRef<Walker_Concrete> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc_Concrete(ZRef<Walker_Concrete> iWalker,
		ZVal_Any* ioResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

	// -----

	class Walker_Search;
	friend class Walker_Search;

	ZRef<ZQE::Walker> pMakeWalker_Search(PQuery* iPQuery, const ZRef<ZQE::Expr_Rel_Search>& iRel);

	// -----

	ZRef<ZDatonSet::DatonSet> fDatonSet;
	ZRef<ZDatonSet::DatonSet> fDatonSet_Temp;
	ZRef<Event> fEvent;

	// -----

	typedef std::map<ZDatonSet::Daton, std::pair<ZDatonSet::NamedEvent, ZVal_Any> > Map_Main;
	Map_Main fMap;

	typedef std::map<ZDatonSet::Daton, std::pair<ZVal_Any, bool> > Map_Pending;
	Map_Pending fMap_Pending;
	std::vector<Map_Pending> fStack_Map_Pending;

	// -----

	class DLink_ClientQuery_InPQuery;
	class DLink_ClientQuery_NeedsWork;
	class ClientQuery;
	std::map<int64, ClientQuery> fMap_Refcon_ClientQuery;
	DListHead<DLink_ClientQuery_NeedsWork> fClientQuery_NeedsWork;

	// -----

	class DLink_PQuery_NeedsWork;
	typedef std::map<ZRef<ZRA::Expr_Rel>, PQuery, Less_Compare_T<ZRef<ZRA::Expr_Rel> > >
		Map_Rel_PQuery;
	Map_Rel_PQuery fMap_Rel_PQuery;

	DListHead<DLink_PQuery_NeedsWork> fPQuery_NeedsWork;

	// -----

	class DLink_PSearch_NeedsWork;
	class PSearch;

	typedef std::map<ZRA::RelHead,PSearch> Map_PSearch;
	Map_PSearch fMap_PSearch;

	DListHead<DLink_PSearch_NeedsWork> fPSearch_NeedsWork;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_DatonSet_h__
