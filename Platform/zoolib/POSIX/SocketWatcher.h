// Copyright (c) 2013 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_SocketWatcher_h__
#define __ZooLib_SocketWatcher_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Compat_NonCopyable.h"

#include "zoolib/ZThread.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark - SocketWatcher

class SocketWatcher
:	NonCopyable
	{
public:
	SocketWatcher();

	typedef std::pair<int,ZP<Callable_void>> Pair_t;

// -----

	bool QInsert(const Pair_t& iPair);
	bool QErase(const Pair_t& iPair);

	bool QInsert(int iSocket, const ZP<Callable_void>& iCallable);
	bool QErase(int iSocket, const ZP<Callable_void>& iCallable);

// -----

private:
	void pRun();
	static void spRun(SocketWatcher* iSocketWatcher);

	ZMtx fMtx;
	ZCnd fCnd;

	bool fThreadRunning;
	typedef std::set<Pair_t> Set_t;
	Set_t fSet;
	};

} // namespace ZooLib

#endif // __ZooLib_SocketWatcher_h__
