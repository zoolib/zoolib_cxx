/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZThread_h__
#define __ZThread_h__ 1
#include "zconfig.h"

#include "zoolib/ZThread_boost.h"
#include "zoolib/ZThread_MacMP.h"
#include "zoolib/ZThread_pthread.h"
#include "zoolib/ZThread_T.h" // For ZAcquirer_T, ZReleaser_T, ZGuard_T
#include "zoolib/ZThread_Win.h"

#if __MACH__
	#include <mach/mach_init.h> // For mach_thread_self
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZThread, ZTSS etc.

#if 0

#elif ZCONFIG_API_Enabled(Thread_pthread)

	namespace ZThread { using namespace ZThread_pthread; }
	namespace ZTSS { using namespace ZTSS_pthread; }

	typedef ZCnd_pthread ZCnd;
	typedef ZMtx_pthread ZMtx;
	typedef ZMtxR_pthread ZMtxR;

	typedef ZSem_pthread ZSem;
	typedef ZSemNoTimeout_pthread ZSemNoTimeout;

#elif ZCONFIG_API_Enabled(Thread_MacMP)

	namespace ZThread { using namespace ZThread_MacMP; }
	namespace ZTSS { using namespace ZTSS_MacMP; }

	typedef ZCnd_MacMP ZCnd;
	typedef ZMtx_MacMP ZMtx;
	typedef ZMtxR_MacMP ZMtxR;

	typedef ZSem_MacMP ZSem;

#elif ZCONFIG_API_Enabled(Thread_Win)

	namespace ZThread { using namespace ZThread_Win; }
	namespace ZTSS { using namespace ZTSS_Win; }

	typedef ZCnd_Win ZCnd;
	typedef ZMtx_Win ZMtx;
	typedef ZMtxR_Win ZMtxR;

	typedef ZSem_Win ZSem;
	typedef ZSem_Win ZSemNoTimeout;

#elif ZCONFIG_API_Enabled(Thread_boost)

	namespace ZThread {}
	namespace ZTSS {}

	typedef ZCnd_boost ZCnd;
	typedef ZMtx_boost ZMtx;
	typedef ZSem_boost ZSem;

#endif

// =================================================================================================
// MARK: - ZTSS

namespace ZTSS {

ZTSS::Key sKey(ZAtomicPtr_t& ioStorage);

} // namespace ZTSS

// =================================================================================================
// MARK: - ZBen (benaphore)

typedef ZBen_T<ZSemNoTimeout> ZBen;
typedef ZBenR_T<ZBen,ZThread::ID,ZThread::sID> ZBenR;

// =================================================================================================
// MARK: - Acquirer, Releaser, Guard

typedef ZAcquirer_T<ZMtx> ZAcqMtx;
typedef ZAcquirer_T<ZMtxR> ZAcqMtxR;

typedef ZReleaser_T<ZMtx> ZRelMtx;
typedef ZReleaser_T<ZMtxR> ZRelMtxR;

typedef ZGuard_T<ZMtx> ZGuardMtx;
typedef ZGuard_T<ZMtxR> ZGuardMtxR;

typedef ZAcquirer_T<ZBen> ZAcqBen;
typedef ZGuard_T<ZBenR> ZGuardBenR;

// =================================================================================================
// MARK: - ZThread

namespace ZThread {

void sStarted();
void sFinished();
void sDontTearDownTillAllThreadsExit();
void sWaitTillAllThreadsExit();

class InitHelper
	{
public:
	InitHelper();
	~InitHelper();
	};

static InitHelper sInitHelper;

// The Starter_T class is used to provide a scope for the Proc typedef
// and the ProxyParam struct, without which the code is pretty impenetrable.

template <class T>
class Starter_T
	{
public:
	typedef void (*Proc)(T);

private:
	struct ProxyParam
		{
		Proc fProc;
		T fParam;

		ProxyParam(Proc iProc, T iParam)
		:	fProc(iProc), fParam(iParam)
			{}
		};


	static ProcResult_t
	#if ZCONFIG_API_Enabled(Thread_Win)
		__stdcall
	#endif
	sEntry(ProxyParam* iProxyParam)
		{
		// Useful when debugging under GDB, which (on MacOS) doesn't always
		// tell us what pthread_id corresponds to a thread.
		const ZThread::ID theThreadID ZMACRO_Attribute_Unused = ZThread::sID();

		#if __MACH__
			const mach_port_t theMachThreadID ZMACRO_Attribute_Unused = ::mach_thread_self();
		#endif

		sStarted();

		try { iProxyParam->fProc(iProxyParam->fParam); }
		catch (...) {}

		delete iProxyParam;

		sFinished();

		return 0;
		}

public:
	static void sCreate(Proc iProc, T iParam)
		{
		ProxyParam* thePP = new ProxyParam(iProc, iParam);
		try
			{
			sCreateRaw(0, (ProcRaw_t)sEntry, thePP);
			}
		catch (...)
			{
			delete thePP;
			throw;
			}
		}
	};

template <class T>
void sCreate_T(typename Starter_T<T>::Proc iProc, T iParam)
	{ Starter_T<T>::sCreate(iProc, iParam); }

typedef void (*ProcVoid_t)();
void sCreateVoid(ProcVoid_t iProcVoid);

} // namespace ZThread

} // namespace ZooLib

#endif // __ZThread_h__
