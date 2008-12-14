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

#ifndef __ZThreadImp__
#define __ZThreadImp__ 1
#include "zconfig.h"

#include "zoolib/ZThreadImp_boost.h"
#include "zoolib/ZThreadImp_MacMP.h"
#include "zoolib/ZThreadImp_pthread.h"
#include "zoolib/ZThreadImp_T.h" // For ZGuard_T
#include "zoolib/ZThreadImp_Win.h"

namespace ZooLib {

#if 0

#elif ZCONFIG_API_Enabled(ThreadImp_pthread)

	namespace ZTSS = ZTSS_pthread;
	namespace ZThreadImp = ZThreadImp_pthread;

	typedef ZMtx_pthread ZMtx;
	typedef ZCnd_pthread ZCnd;
	typedef ZSemNoTimeout_pthread ZSemNotimeout;
	typedef ZSem_pthread ZSem;

#elif ZCONFIG_API_Enabled(ThreadImp_MacMP)

	namespace ZTSS = ZTSS_MacMP;
	namespace ZThreadImp = ZThreadImp_MacMP;

	typedef ZMtx_MacMP ZMtx;
	typedef ZCnd_MacMP ZCnd;
	typedef ZSem_MacMP ZSem;

#elif ZCONFIG_API_Enabled(ThreadImp_Win)

	namespace ZTSS = ZTSS_Win;
	namespace ZThreadImp = ZThreadImp_Win;

	typedef ZMtx_Win ZMtx;
	typedef ZCnd_Win ZCnd;
	typedef ZSem_Win ZSem;

#elif ZCONFIG_API_Enabled(ThreadImp_boost)

	namespace ZTSS {}
	namespace ZThreadImp {}

	typedef ZMtx_boost ZMtx;
	typedef ZCnd_boost ZCnd;
	typedef ZSem_boost ZSem;

#endif

typedef ZGuard_T<ZMtx> ZGuardMtx;

} // namespace ZooLib

#ifndef ZooLib_SuppressInjection
	using ZooLib::ZCnd;
	using ZooLib::ZGuardMtx;
	using ZooLib::ZMtx;
	using ZooLib::ZSem;
	namespace ZTSS = ZooLib::ZTSS;
	namespace ZThreadImp = ZooLib::ZThreadImp;
#endif

#endif // __ZThreadImp__
