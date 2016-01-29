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

#ifndef __ZooLib_Dataspace_RelaterMUX_h__
#define __ZooLib_Dataspace_RelaterMUX_h__ 1
#include "zconfig.h"

#include "zooLib/Dataspace/Relater.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -
#pragma mark RelaterMUX

class RelaterMUX : public Factory_Relater
	{
public:
	enum { kDebug = 1 };

	RelaterMUX(ZRef<Relater> iRelater);
	virtual ~RelaterMUX();

// From ZCounted via Factory_Relater (aka Callable<ZRef<Relater>()>)
	virtual void Initialize();
	virtual void Finalize();

// From Factory_Relater
	virtual ZQ<ZRef<Relater> > QCall();

private:
	class PQuery;
	class ClientQuery;
	class Relater_Client;
	friend class Relater_Client;

	bool pIntersects(ZRef<Relater_Client> iCR, const RelHead& iRelHead);

	void pModifyRegistrations(ZRef<Relater_Client> iCR,
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount);

	void pCollectResults(ZRef<Relater_Client> iCR,
		std::vector<QueryResult>& oChanged);

	void pResultsAvailable(ZRef<Relater> iRelater);

	void pFinalizeRelater_Client(Relater_Client* iCR);

	ZMtxR fMtxR;

	ZRef<Relater> fRelater;
	bool fResultsAvailable;

	int64 fNextPRefcon;

	std::map<int64,std::pair<Relater_Client*,int64> > fPRefconToClient;
	std::set<Relater_Client*> fRelater_Clients;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_RelaterMUX_h__
