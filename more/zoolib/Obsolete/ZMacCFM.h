/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZMacCFM_h__
#define __ZMacCFM_h__ 1
#include "zconfig.h"

#define ZCONFIG_CFM_Use_UPP (TARGET_RT_MAC_CFM && !TARGET_API_MAC_CARBON)

#if ZCONFIG_CFM_Use_UPP
	#include ZMACINCLUDE3(CoreServices,CarbonCore,MixedMode.h)
#endif

// =================================================================================================

#if ZCONFIG_CFM_Use_UPP

	#define ZMacCFM_CreateUPP(proc, func) \
	(proc##_UPP)NewRoutineDescriptor((ProcPtr)func, proc##_ProcInfo, GetCurrentArchitecture())

#else

	#define ZMacCFM_CreateUPP(proc, func) func

#endif

// =================================================================================================
// 0 Arguments

#define ZMacCFM_Priv_DefineProcPtr0(ret, proc) \
	typedef ret(*proc##ProcPtr)()

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call0(ret, proc, upp) \
			(ret) CallUniversalProc(upp, proc##_ProcInfo)
	#define ZMacCFM_Priv_DefineUPP0(ret, proc) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(0)) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}

	#define ZMacCFM_Priv_DefineUPP0_Void(proc) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(0)) \
			}
#else
	#define ZMacCFM_Call0(ret, proc, upp) \
			upp()
	#define ZMacCFM_Priv_DefineUPP0(ret, proc) \
			typedef proc##ProcPtr proc##_UPP

	#define ZMacCFM_Priv_DefineUPP0_Void(proc) \
			typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc0(ret, upp) \
	ZMacCFM_Priv_DefineProcPtr0(ret, upp); \
	ZMacCFM_Priv_DefineUPP0(ret, upp)

#define ZMacCFM_DefineProc0_Void(upp) \
	ZMacCFM_Priv_DefineProcPtr0(void, upp); \
	ZMacCFM_Priv_DefineUPP0_Void(upp)

// =================================================================================================
// 1 Argument

#define ZMacCFM_Priv_DefineProcPtr1(ret, proc, arg1) \
	typedef ret(*proc##ProcPtr)(arg1)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call1(ret, proc, upp, arg1) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1)
	#define ZMacCFM_Priv_DefineUPP1(ret, proc, arg1) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP1_Void(proc, arg1) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			}
#else
	#define ZMacCFM_Call1(ret, proc, upp, arg1) \
		upp(arg1)
	#define ZMacCFM_Priv_DefineUPP1(ret, proc, arg1) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP1_Void(proc, arg1) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc1(ret, upp, arg1) \
	ZMacCFM_Priv_DefineProcPtr1(ret, upp, arg1); \
	ZMacCFM_Priv_DefineUPP1(ret, upp, arg1)

#define ZMacCFM_DefineProc1_Void(upp, arg1) \
	ZMacCFM_Priv_DefineProcPtr1(void, upp, arg1); \
	ZMacCFM_Priv_DefineUPP1_Void(upp, arg1)

// =================================================================================================
// 2 Arguments

#define ZMacCFM_Priv_DefineProcPtr2(ret, proc, arg1, arg2) \
	typedef ret(*proc##ProcPtr)(arg1, arg2)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call2(ret, proc, upp, arg1, arg2) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2)
	#define ZMacCFM_Priv_DefineUPP2(ret, proc, arg1, arg2) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP2_Void(proc, arg1, arg2) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			}
#else
	#define ZMacCFM_Call2(ret, proc, upp, arg1, arg2) \
		upp(arg1, arg2)
	#define ZMacCFM_Priv_DefineUPP2(ret, proc, arg1, arg2) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP2_Void(proc, arg1, arg2) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc2(ret, upp, arg1, arg2) \
	ZMacCFM_Priv_DefineProcPtr2(ret, upp, arg1, arg2); \
	ZMacCFM_Priv_DefineUPP2(ret, upp, arg1, arg2)

#define ZMacCFM_DefineProc2_Void(upp, arg1, arg2) \
	ZMacCFM_Priv_DefineProcPtr2(void, upp, arg1, arg2); \
	ZMacCFM_Priv_DefineUPP2_Void(upp, arg1, arg2)

// =================================================================================================
// 3 Arguments

#define ZMacCFM_Priv_DefineProcPtr3(ret, proc, arg1, arg2, arg3) \
	typedef ret(*proc##ProcPtr)(arg1, arg2, arg3)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call3(ret, proc, upp, arg1, arg2, arg3) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3)
	#define ZMacCFM_Priv_DefineUPP3(ret, proc, arg1, arg2, arg3) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP3_Void(proc, arg1, arg2, arg3) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			}
#else
	#define ZMacCFM_Call3(ret, proc, upp, arg1, arg2, arg3) \
		upp(arg1, arg2, arg3)
	#define ZMacCFM_Priv_DefineUPP3(ret, proc, arg1, arg2, arg3) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP3_Void(proc, arg1, arg2, arg3) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc3(ret, upp, arg1, arg2, arg3) \
	ZMacCFM_Priv_DefineProcPtr3(ret, upp, arg1, arg2, arg3); \
	ZMacCFM_Priv_DefineUPP3(ret, upp, arg1, arg2, arg3)

#define ZMacCFM_DefineProc3_Void(upp, arg1, arg2, arg3) \
	ZMacCFM_Priv_DefineProcPtr3(void, upp, arg1, arg2, arg3); \
	ZMacCFM_Priv_DefineUPP3_Void(upp, arg1, arg2, arg3)

// =================================================================================================
// 4 Arguments

#define ZMacCFM_Priv_DefineProcPtr4(ret, proc, arg1, arg2, arg3, arg4) \
	typedef ret(*proc##ProcPtr)(arg1, arg2, arg3, arg4)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call4(ret, proc, upp, arg1, arg2, arg3, arg4) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4)
	#define ZMacCFM_Priv_DefineUPP4(ret, proc, arg1, arg2, arg3, arg4) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP4_Void(proc, arg1, arg2, arg3, arg4) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			}
#else
	#define ZMacCFM_Call4(ret, proc, upp, arg1, arg2, arg3, arg4) \
		upp(arg1, arg2, arg3, arg4)
	#define ZMacCFM_Priv_DefineUPP4(ret, proc, arg1, arg2, arg3, arg4) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP4_Void(proc, arg1, arg2, arg3, arg4) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc4(ret, upp, arg1, arg2, arg3, arg4) \
	ZMacCFM_Priv_DefineProcPtr4(ret, upp, arg1, arg2, arg3, arg4); \
	ZMacCFM_Priv_DefineUPP4(ret, upp, arg1, arg2, arg3, arg4)

#define ZMacCFM_DefineProc4_Void(upp, arg1, arg2, arg3, arg4) \
	ZMacCFM_Priv_DefineProcPtr4(void, upp, arg1, arg2, arg3, arg4); \
	ZMacCFM_Priv_DefineUPP4_Void(upp, arg1, arg2, arg3, arg4)

// =================================================================================================
// 5 Arguments

#define ZMacCFM_Priv_DefineProcPtr5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
	typedef ret(*proc##ProcPtr)(arg1, arg2, arg3, arg4, arg5)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call5(ret, proc, upp, arg1, arg2, arg3, arg4, arg5) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5)
	#define ZMacCFM_Priv_DefineUPP5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP5_Void(proc, arg1, arg2, arg3, arg4, arg5) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			}
#else
	#define ZMacCFM_Call5(ret, proc, upp, arg1, arg2, arg3, arg4, arg5) \
		upp(arg1, arg2, arg3, arg4, arg5)
	#define ZMacCFM_Priv_DefineUPP5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP5_Void(proc, arg1, arg2, arg3, arg4, arg5) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc5(ret, upp, arg1, arg2, arg3, arg4, arg5) \
	ZMacCFM_Priv_DefineProcPtr5(ret, upp, arg1, arg2, arg3, arg4, arg5); \
	ZMacCFM_Priv_DefineUPP5(ret, upp, arg1, arg2, arg3, arg4, arg5)

#define ZMacCFM_DefineProc5_Void(upp, arg1, arg2, arg3, arg4, arg5) \
	ZMacCFM_Priv_DefineProcPtr5(void, upp, arg1, arg2, arg3, arg4, arg5); \
	ZMacCFM_Priv_DefineUPP5_Void(upp, arg1, arg2, arg3, arg4, arg5)

// =================================================================================================
// 6 Arguments

#define ZMacCFM_Priv_DefineProcPtr6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
	typedef ret(*proc##ProcPtr)(arg1, arg2, arg3, arg4, arg5, arg6)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call6(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5, arg6)
	#define ZMacCFM_Priv_DefineUPP6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP6_Void(proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			}
#else
	#define ZMacCFM_Call6(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
		upp(arg1, arg2, arg3, arg4, arg5, arg6)
	#define ZMacCFM_Priv_DefineUPP6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP6_Void(proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
	ZMacCFM_Priv_DefineProcPtr6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6); \
	ZMacCFM_Priv_DefineUPP6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6)

#define ZMacCFM_DefineProc6_Void(upp, arg1, arg2, arg3, arg4, arg5, arg6) \
	ZMacCFM_Priv_DefineProcPtr6(void, upp, arg1, arg2, arg3, arg4, arg5, arg6); \
	ZMacCFM_Priv_DefineUPP6_Void(upp, arg1, arg2, arg3, arg4, arg5, arg6)

// =================================================================================================
// 7 Arguments

#define ZMacCFM_Priv_DefineProcPtr7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	typedef ret(*proc##ProcPtr)(arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#if ZCONFIG_CFM_Use_UPP
	#define ZMacCFM_Call7(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
	#define ZMacCFM_Priv_DefineUPP7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			| STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(arg7))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			}
	#define ZMacCFM_Priv_DefineUPP7_Void(proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			| STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(arg7))) \
			}
#else
	#define ZMacCFM_Call7(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		upp(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
	#define ZMacCFM_Priv_DefineUPP7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef proc##ProcPtr proc##_UPP
	#define ZMacCFM_Priv_DefineUPP7_Void(proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef proc##ProcPtr proc##_UPP
#endif

#define ZMacCFM_DefineProc7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	ZMacCFM_Priv_DefineProcPtr7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
	ZMacCFM_Priv_DefineUPP7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define ZMacCFM_DefineProc7_Void(upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	ZMacCFM_Priv_DefineProcPtr7(void, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
	ZMacCFM_Priv_DefineUPP7_Void(upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#endif // __ZMacCFM_h__
