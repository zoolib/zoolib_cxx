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

#ifndef __ZThread__
#define __ZThread__ 1
#include "zconfig.h"

#include "zoolib/ZThread_boost.h"
#include "zoolib/ZThread_MacMP.h"
#include "zoolib/ZThread_pthread.h"
#include "zoolib/ZThread_T.h" // For ZGuard_T
#include "zoolib/ZThread_Win.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZThread, ZTSS etc.

#if 0

#elif ZCONFIG_API_Enabled(Thread_pthread)

	namespace ZThread { using namespace ZThread_pthread; }
	namespace ZTSS = ZTSS_pthread;

	typedef ZCnd_pthread ZCnd;
	typedef ZMtx_pthread ZMtx;
	typedef ZSem_pthread ZSem;
	typedef ZSemNoTimeout_pthread ZSemNoTimeout;

#elif ZCONFIG_API_Enabled(Thread_MacMP)

	namespace ZThread { using namespace ZThread_MacMP; }
	namespace ZTSS = ZTSS_MacMP;

	typedef ZCnd_MacMP ZCnd;
	typedef ZMtx_MacMP ZMtx;
	typedef ZSem_MacMP ZSem;

#elif ZCONFIG_API_Enabled(Thread_Win)

	namespace ZThread { using namespace ZThread_Win; }
	namespace ZTSS = ZTSS_Win;

	typedef ZCnd_Win ZCnd;
	typedef ZMtx_Win ZMtx;
	typedef ZSem_Win ZSem;

#elif ZCONFIG_API_Enabled(Thread_boost)

	namespace ZThread {}
	namespace ZTSS {}

	typedef ZCnd_boost ZCnd;
	typedef ZMtx_boost ZMtx;
	typedef ZSem_boost ZSem;

#endif

typedef ZGuard_T<ZMtx> ZGuardMtx;

// =================================================================================================
#pragma mark -
#pragma mark * ZThread

namespace ZThread {

template <class T>
class Starter_T
	{
public:
	typedef void (*Proc)(T);

	struct ProxyParam
		{
		Proc fProc;
		T fParam;

		ProxyParam(Proc iProc, T iParam)
		:	fProc(iProc), fParam(iParam)
			{}
		};

	#if ZCONFIG_API_Enabled(Thread_Win)
		static ProcResult_t __stdcall sEntry(ProxyParam* iProxyParam)
	#else
		static ProcResult_t sEntry(ProxyParam* iProxyParam)
	#endif
		{
		ProxyParam localProxyParam = *iProxyParam;

		delete iProxyParam;

		localProxyParam.fProc(localProxyParam.fParam);

		return 0;
		}

	static void sCreate(Proc iProc, T iParam)
		{ sCreateRaw(0, (ProcRaw_t)sEntry, new ProxyParam(iProc, iParam)); }
	};

template <class T>
void sCreate_T(typename Starter_T<T>::Proc iProc, T iParam)
	{ Starter_T<T>::sCreate(iProc, iParam); }

typedef void (*ProcVoid_t)();
void sCreateVoid(ProcVoid_t iProcVoid);

} // namespace ZThread

NAMESPACE_ZOOLIB_END

#endif // __ZThread__
