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
	enum { kDebug = 0 };

	Source_Union();
	virtual ~Source_Union();

// From Source
	virtual bool Intersects(const RelHead& iRelHead);

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	virtual void CollectResults(std::vector<SearchResult>& oChanged);

// Our protocol
	void InsertSource(ZRef<Source> iSource, const string8& iPrefix);
	void EraseSource(ZRef<Source> iSource);

private:
	ZMtxR fMtxR;

	class PSearch;

	// -----

	class PIP;
	class DLink_PIP_InProxy;
	class DLink_PIP_NeedsWork;
	typedef std::map<int64, PIP> Map_Refcon_PIP;

	// -----
	class PSource;

	class Proxy;
	class Visitor_Proxy;

	typedef std::map<ZRef<ZRA::Expr_Rel>,Proxy*> ProxyMap;
	ProxyMap fProxyMap;

	std::set<PSource*> pIdentifyPSources(const RelHead& iRelHead);
	ZRef<ZRA::Expr_Rel> pGetProxy(PSearch* iPSearch,
		const std::set<PSource*>& iPSources, const RelHead& iRelHead, ZRef<ZRA::Expr_Rel> iRel);
	void pFinalizeProxy(Proxy* iProxy);

	// -----

	class Walker_Proxy;

	ZRef<Walker_Proxy> pMakeWalker(ZRef<Proxy> iProxy);

	void pRewind(ZRef<Walker_Proxy> iWalker);

	void pPrime(ZRef<Walker_Proxy> iWalker,
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	bool pReadInc(ZRef<Walker_Proxy> iWalker,
		ZVal_Any* ioResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

	// -----

	class DLink_ClientSearch_InPSearch;
	class ClientSearch;

	typedef std::map<int64, ClientSearch> Map_Refcon_ClientSearch;
	Map_Refcon_ClientSearch fMap_Refcon_ClientSearch;

	// -----

	class DLink_PSearch_NeedsWork;
	DListHead<DLink_PSearch_NeedsWork> fPSearch_NeedsWork;

	typedef std::map<ZRef<ZRA::Expr_Rel>, PSearch, Less_Compare_T<ZRef<ZRA::Expr_Rel> > >
		Map_Rel_PSearch;
	Map_Rel_PSearch fMap_Rel_PSearch;

	// -----

	class Visitor_DoMakeWalker;

	// -----

	class Analyze;
	friend class Analyze;

	// -----

	class DLink_PSource_NeedsWork;
	class DLink_PSource_CollectFrom;
	DListHead<DLink_PSource_CollectFrom> fPSource_CollectFrom;
	DListHead<DLink_PSource_NeedsWork> fPSource_NeedsWork;

	typedef std::map<ZRef<Source>, PSource> Map_Source_PSource;
	Map_Source_PSource fMap_Source_PSource;

	// -----

	ZRef<Event> fEvent;
	ZRef<Source::Callable_ResultsAvailable> fCallable_ResultsAvailable;

	void pCollectFrom(PSource* iPSource);
	void pResultsAvailable(ZRef<Source> iSource);
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_Union__
