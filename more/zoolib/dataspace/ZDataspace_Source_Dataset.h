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

#include "zoolib/ZVal_Any.h"

#include "zoolib/dataset/ZDataset.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include "zoolib/zqe/ZQE_Walker.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

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
	// Some kind of index specs to be passed in too.
	Source_Dataset(ZRef<ZDataset::Dataset> iDataset);
	virtual ~Source_Dataset();

	virtual std::set<RelHead> GetRelHeads();

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

// Our protocol
	ZRef<ZDataset::Dataset> GetDataset();

	void Insert(const ZDataset::Daton& iDaton);
	void Erase(const ZDataset::Daton& iDaton);

	void Insert(const ZVal_Any& iVal);
	void Erase(const ZVal_Any& iVal);

	size_t OpenTransaction();
	void ClearTransaction(size_t iIndex);
	void CloseTransaction(size_t iIndex);

private:
	void pModify(const ZDataset::Daton& iDaton, const ZVal_Any& iVal, bool iSense);

	class Visitor_DoMakeWalker;
	friend class Visitor_DoMakeWalker;	
	ZRef<ZQE::Walker> pMakeWalker(const RelHead& iRelHead);

	class Walker;
	friend class Walker;

	void pRewind(ZRef<Walker> iWalker);

	void pPrime(ZRef<Walker> iWalker,
		const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc(ZRef<Walker> iWalker,
		const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);
	
	bool pPull();

	ZRef<ZDataset::Dataset> fDataset;
	ZRef<Event> fEvent;

	typedef std::map<ZDataset::Daton, std::pair<ZDataset::NamedEvent, ZVal_Any> > Map_Main;
	Map_Main fMap;

	typedef std::map<ZDataset::Daton, std::pair<ZVal_Any, bool> > Map_Pending;
	Map_Pending fMap_Pending;
	std::vector<Map_Pending> fStack;
	bool fStackChanged;

	class PQuery;
	// We could have the PQuery be an in-place value in the map.
	std::map<int64, PQuery*> fMap_RefconToPQuery;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Dataset__
