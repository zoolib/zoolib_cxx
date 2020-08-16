// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZThread_h__
#define __ZThread_h__ 1
#include "zconfig.h"

#include "zoolib/ZThread_pthread.h"
#include "zoolib/ZThread_T.h"
#include "zoolib/ZThread_Win.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZThread, ZTSS etc.

#if 0

#elif ZCONFIG_API_Enabled(Thread_pthread)

	namespace ZThread { using namespace ZThread_pthread; }
	namespace ZTSS { using namespace ZTSS_pthread; }

	#if ZCONFIG_pthread_Debug
		typedef ZCnd_pthread ZCnd;
		typedef ZMtxChecked_pthread ZMtx;
	#else
		typedef ZCndBase_pthread ZCnd;
		typedef ZMtx_pthread ZMtx;
	#endif

	typedef ZSem_pthread ZSem;

	#if !defined(__MACH__)
		typedef ZSemNoTimeout_pthread ZSemNoTimeout;
	#endif

#elif ZCONFIG_API_Enabled(Thread_Win)

	namespace ZThread { using namespace ZThread_Win; }
	namespace ZTSS { using namespace ZTSS_Win; }

	typedef ZCnd_Win ZCnd;
	typedef ZMtx_Win ZMtx;

	typedef ZSem_Win ZSem;
	typedef ZSem_Win ZSemNoTimeout;

#endif

// =================================================================================================
#pragma mark - ZTSS

namespace ZTSS {

Key sKey(std::atomic<Key>& ioStorage);

} // namespace ZTSS

// =================================================================================================
#pragma mark - Acquirer, Releaser

typedef ZAcquirer_T<ZMtx> ZAcqMtx;

typedef ZReleaser_T<ZMtx> ZRelMtx;

// =================================================================================================
#pragma mark - ZThread

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
			const mach_port_t theMachThreadID ZMACRO_Attribute_Unused =
				::pthread_mach_thread_np(::pthread_self());
		#endif

		sStarted();

		try { iProxyParam->fProc(iProxyParam->fParam); }
		catch (...) {}

		delete iProxyParam;

		sFinished();

		return 0;
		}

public:
	static void sStart(Proc iProc, T iParam)
		{
		ProxyParam* thePP = new ProxyParam(iProc, iParam);
		try
			{
			sStartRaw(0, (ProcRaw_t)sEntry, thePP);
			}
		catch (...)
			{
			delete thePP;
			throw;
			}
		}
	};

template <class T>
void sStart_T(typename Starter_T<T>::Proc iProc, T iParam)
	{ Starter_T<T>::sStart(iProc, iParam); }

} // namespace ZThread

} // namespace ZooLib

#endif // __ZThread_h__
