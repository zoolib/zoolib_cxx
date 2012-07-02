/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZAtomic.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZDebug.h" // For ZAssertCompile

// Ensure that ZAtomic_t packs properly
ZAssertCompile(sizeof(ZooLib::ZAtomic_t) == sizeof(int));

// =================================================================================================
// MARK: - CodeWarrior/PPC

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)

#if 0
	#define sAtomic_PPC405_ERR77(rA,rB) dcbt rA , rB;
#else
	#define sAtomic_PPC405_ERR77(rA,rB)
#endif

// We attach a register declarator to each parameter so that CW does not generate
// code to save the values of the registers we use explicitly.

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

asm bool sAtomic_CAS(register ZAtomic_t* iAtomic,
	register int iOldValue, register int iNewValue)
	{
	// r3 = iAtomic
	// r4 = iOldValue
	// r5 = iNewValue
	// r6 = realOldValue
tryAgain:
	lwarx r6, 0, r3;
	cmpw r6, r4
	bne- noGood
	sAtomic_PPC405_ERR77(0, r3)
	stwcx. r5, 0, r3;
	bne- tryAgain
	isync
	li r3, 1
	blr
noGood:
	// Store the value we read, as suggested by "970Programming_Note_larx_stcx.body.fm.0.1":
	// <http://www-306.ibm.com/chips/techlib/techlib.nsf/techdocs/
	// 3A2D397F9A3202BD87256D4B007164C0/$file/970Programming_Note_larx_stcx.d20030618.pdf>
	stwcx. r6, 0, r3;
	li r3, 0
	blr
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)

// =================================================================================================
// MARK: - CodeWarrior/x86

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	asm
		{
		mov eax, iOldValue
		mov ecx, iNewValue
		mov edx, iAtomic
		lock cmpxchg [edx], ecx
		sete al
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	asm
		{
		mov eax, iParam
		mov edx, iAtomic
		lock xchg [edx], eax
		mov iParam, eax
		}
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Add)
#define DEFINED_sAtomic_Add 1

int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	asm
		{
		mov eax, iParam
		mov edx, iAtomic
		lock xadd [edx], eax
		mov iParam, eax
		}
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_DecAndTest)
#define DEFINED_sAtomic_DecAndTest 1

bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	bool isZero;
	asm
		{
		mov edx, iAtomic
		lock dec dword ptr [edx]
		sete isZero
		}
	return isZero;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Inc)
#define DEFINED_sAtomic_Inc 1

void sAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock inc dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

void sAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock dec dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)

// =================================================================================================
// MARK: - Windows

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_Win.h"

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CASPtr)
#define DEFINED_sAtomic_CASPtr 1

bool sAtomic_CASPtr(void* iPtrAddress, void* iOldValue, void* iNewValue)
	{
	return iOldValue
		== ::InterlockedCompareExchangePointer((PVOID*)iPtrAddress, iNewValue, iOldValue);
//		== ::InterlockedCompareExchangePointer((volatile PVOID*)iPtrAddress, iNewValue, iOldValue);
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	return iOldValue
		== ::InterlockedCompareExchange((LONG*)&iAtomic->fValue, iNewValue, iOldValue);
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{ return ::InterlockedExchange((LONG*)&iAtomic->fValue, iParam); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Add)
#define DEFINED_sAtomic_Add 1

int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{ return ::InterlockedExchangeAdd((LONG*)&iAtomic->fValue, iParam); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_DecAndTest)
#define DEFINED_sAtomic_DecAndTest 1

bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 0 == ::InterlockedDecrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Inc)
#define DEFINED_sAtomic_Inc 1

void sAtomic_Inc(ZAtomic_t* iAtomic)
	{ ::InterlockedIncrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

void sAtomic_Dec(ZAtomic_t* iAtomic)
	{ ::InterlockedDecrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
// MARK: - Android

#if defined(__ANDROID__)

#include <sys/atomics.h>

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{ return 0 == __atomic_cmpxchg(iOldValue, iNewValue, &iAtomic->fValue); }

#endif

} // namespace ZooLib

#endif // defined(__ANDROID__)

// =================================================================================================
// MARK: - A real CompareAndSet must be defined by now

#if !defined(DEFINED_sAtomic_CAS)
	#error No CompareAndSet available
#endif

// =================================================================================================
// MARK: - Default implementations that depend on CAS or sAtomic_Add.

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	for (;;)
		{
		int prior = iAtomic->fValue;
		if (sAtomic_CAS(iAtomic, prior, iParam))
			return prior;
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Add)
#define DEFINED_sAtomic_Add 1

int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	for (;;)
		{
		int prior = iAtomic->fValue;
		if (sAtomic_CAS(iAtomic, prior, prior + iParam))
			return prior;
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_DecAndTest)
#define DEFINED_sAtomic_DecAndTest 1

bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 1 == sAtomic_Add(iAtomic, -1); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Inc)
#define DEFINED_sAtomic_Inc 1

void sAtomic_Inc(ZAtomic_t* iAtomic)
	{ sAtomic_Add(iAtomic, 1); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

void sAtomic_Dec(ZAtomic_t* iAtomic)
	{ sAtomic_Add(iAtomic, -1); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CASPtr)
#define DEFINED_sAtomic_CASPtr 1

bool sAtomic_CASPtr(void* iPtrAddress, void* iOldValue, void* iNewValue)
	{ return sAtomic_CAS((ZAtomic_t*)iPtrAddress, (int)iOldValue, (int)iNewValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Barrier)
#define DEFINED_sAtomic_Barrier 1

void sAtomic_Barrier()
	{}

#endif
// -----------------------------------------------

} // namespace ZooLib
