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

#ifndef __ZDataspace_Source_Dataset__
#define __ZDataspace_Source_Dataset__ 1
#include "zconfig.h"

#include "zoolib/ZUtil_Expr_Bool_CNF.h"
#include "zoolib/ZVal_Any.h"

#include "zoolib/dataset/ZDataset.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include "zoolib/zqe/ZQE_Expr_Rel_Search.h"
#include "zoolib/zqe/ZQE_Walker.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

using Util_Expr_Bool::Disjunction;

ZVal_Any sAsVal(const ZDataset::Daton& iDaton);
ZDataset::Daton sAsDaton(const ZVal_Any& iVal);

// =================================================================================================
#pragma mark -
#pragma mark * Annotation_Daton

class Annotation_Daton : public ZCounted
	{
public:
	Annotation_Daton(const ZDataset::Daton& iDaton)
	:	fDaton(iDaton)
		{}

	const ZDataset::Daton& GetDaton() { return fDaton; }

private:
	const ZDataset::Daton fDaton;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Source_Dataset

class Source_Dataset : public Source
	{
public:
	enum { kDebug = 1 };
	
	void ForceUpdate();

	// Some kind of index specs to be passed in too.
	Source_Dataset(ZRef<ZDataset::Dataset> iDataset);
	virtual ~Source_Dataset();

	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<QueryResult>& oChanged);

// Our protocol
	ZRef<ZDataset::Dataset> GetDataset();

	void Insert(const ZDataset::Daton& iDaton);
	void Erase(const ZDataset::Daton& iDaton);

	size_t OpenTransaction();
	void ClearTransaction(size_t iIndex);
	void CloseTransaction(size_t iIndex);

private:
	ZRef<ZQE::Result> pSearch(const Disjunction& iDisjunction, const RelHead& iRelHead);


	ZMtxR fMtxR;
	size_t fWalkerCount;
	size_t fReadCount;
	size_t fStepCount;

	bool pPull();
	void pConditionalPushDown();
	void pModify(const ZDataset::Daton& iDaton, const ZVal_Any& iVal, bool iSense);

	class PQuery;

	class Visitor_DoMakeWalker;
	friend class Visitor_DoMakeWalker;	
//--
	class Walker_Concrete;
	friend class Walker_Concrete;

	ZRef<ZQE::Walker> pMakeWalker_Concrete(const RelHead& iRelHead);

	void pRewind_Concrete(ZRef<Walker_Concrete> iWalker);

	void pPrime_Concrete(ZRef<Walker_Concrete> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc_Concrete(ZRef<Walker_Concrete> iWalker,
		ZVal_Any* ioResults,
		std::set<ZRef<ZCounted> >* oAnnotations);
	
//--
	class Walker_Search;
	friend class Walker_Search;

	ZRef<ZQE::Walker> pMakeWalker_Search(PQuery* iPQuery, const ZRef<ZQE::Expr_Rel_Search>& iRel);
//--
	ZRef<ZDataset::Dataset> fDataset;
	ZRef<Event> fEvent;
//--
	typedef std::map<ZDataset::Daton, std::pair<ZDataset::NamedEvent, ZVal_Any> > Map_Main;
	Map_Main fMap;

	typedef std::map<ZDataset::Daton, std::pair<ZVal_Any, bool> > Map_Pending;
	Map_Pending fMap_Pending;
	std::vector<Map_Pending> fStack_Map_Pending;
	uint64 fChangeCount;
	std::vector<uint64> fStack_ChangeCount;
	bool fChanged;
//--
	class DLink_ClientQuery_InPQuery;
	class DLink_ClientQuery_NeedsWork;
	class ClientQuery;
	std::map<int64, ClientQuery> fMap_Refcon_ClientQuery;
	DListHead<DLink_ClientQuery_NeedsWork> fClientQuery_NeedsWork;
//--
	class DLink_PQuery_NeedsWork;
	typedef std::map<ZRef<ZRA::Expr_Rel>, PQuery, Less_Compare_T<ZRef<ZRA::Expr_Rel> > >
		Map_Rel_PQuery;
	Map_Rel_PQuery fMap_Rel_PQuery;

	DListHead<DLink_PQuery_NeedsWork> fPQuery_NeedsWork;
//--
	class DLink_PSearch_NeedsWork;
	class PSearch;

	DListHead<DLink_PSearch_NeedsWork> fPSearch_NeedsWork;
//--
	void pDetachPQuery(PQuery* iPQuery);
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Dataset__
