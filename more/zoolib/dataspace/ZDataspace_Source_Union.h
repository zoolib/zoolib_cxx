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

#ifndef __ZDataspace_Source_Union__
#define __ZDataspace_Source_Union__ 1
#include "zconfig.h"

#include "zoolib/ZDList.h"

#include "zoolib/dataspace/ZDataspace_Source.h"
#include "zoolib/zqe/ZQE_Walker.h"

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
#pragma mark -
#pragma mark * Source_Union

class Source_Union : public Source
	{
public:
	enum { kDebug = 1 };

	Source_Union();
	virtual ~Source_Union();

// From Source
	virtual RelHead GetRelHead();

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

// Our protocol
	void InsertSource(ZRef<Source> iSource, const string8& iPrefix);
	void EraseSource(ZRef<Source> iSource);

private:
	class PSearch;
	class Proxy;
	class Visitor_Proxy;
	class Visitor_DoMakeWalker;
	ZRef<Proxy> pGetProxy(PSearch* iPSearch, const RelHead& iRelHead, ZRef<ZRA::Expr_Rel> iRel);

	class Walker_Proxy;

	ZRef<ZQE::Walker> pMakeWalker(ZRef<Proxy> iProxy);
	void pRewind(ZRef<Walker_Proxy> iWalker);

	void pPrime(ZRef<Walker_Proxy> iWalker,
		const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc(ZRef<Walker_Proxy> iWalker,
		const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

	struct Comparator_Proxy
		{
		bool operator()(const ZRef<Proxy>& iLeft, const ZRef<Proxy>& iRight) const;

		typedef ZRef<Proxy> first_argument_type;
		typedef ZRef<Proxy> second_argument_type;
		typedef bool result_type;
		};

	typedef std::set<ZRef<Proxy>, Comparator_Proxy> ProxySet;
	ProxySet fProxies;

	std::set<ZRef<Proxy> > fProxiesThatNeedWork;//Urg

	std::set<PSearch*> fPSearchesThatNeedWork;

	bool pIsSimple(const RelHead& iRelHead);

	class Analyze;
	friend class Analyze;

	ZMtxR fMtxR;

	std::map<int64, PSearch*> fMap_RefconToPSearch;

	class ProxyInPSource;

	class PSource;
	std::map<ZRef<Source>,string8> fSources_ToAdd;
	std::set<PSource*> fPSources_ToRemove;

	std::set<PSource*> fPSources_ToCollectFrom;

	std::map<ZRef<Source>, PSource*> fMap_SourceToPSource;
	std::set<PSource*> fPSourcesThatNeedWork;

	ZRef<Event> fEvent;
	ZRef<Source::Callable_ResultsAvailable> fCallable_ResultsAvailable;

	void pCollectFrom(PSource* iPSource);
	void pResultsAvailable(ZRef<Source> iSource);
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Union__
