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

#ifndef __ZDataspace_SourceMUX_h__
#define __ZDataspace_SourceMUX_h__ 1
#include "zconfig.h"

#include "zoolib/dataspace/ZDataspace_Source.h"

#include <set>

namespace ZooLib {
namespace ZDataspace {

// =================================================================================================
// MARK: - SourceMUX

class SourceMUX : public SourceFactory
	{
public:
	enum { kDebug = 1 };

	SourceMUX(ZRef<Source> iSource);
	virtual ~SourceMUX();

// From ZCounted via SourceFactory (aka ZCallable<ZRef<Source>()>)
	virtual void Initialize();
	virtual void Finalize();

// From SourceFactory
	virtual ZQ<ZRef<Source> > QCall();

private:
	class PQuery;
	class ClientQuery;
	class ClientSource;
	friend class ClientSource;

	bool pIntersects(ZRef<ClientSource> iCS, const RelHead& iRelHead);

	void pModifyRegistrations(ZRef<ClientSource> iCS,
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	void pCollectResults(ZRef<ClientSource> iCS,
		std::vector<QueryResult>& oChanged);

	void pResultsAvailable(ZRef<Source> iSource);

	void pFinalizeClientSource(ClientSource* iCS);

	ZMtxR fMtxR;

	ZRef<Source> fSource;
	bool fResultsAvailable;

	int64 fNextPRefcon;

	std::map<int64,std::pair<ClientSource*,int64> > fPRefconToClient;
	std::set<ClientSource*> fClientSources;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_SourceMUX_h__
