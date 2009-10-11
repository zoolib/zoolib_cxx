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
#pragma mark -
#pragma mark * CodeWarrior/PPC

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)

#if 0
#	define ZAtomic_PPC405_ERR77(rA,rB) dcbt rA , rB;
#else
#	define ZAtomic_PPC405_ERR77(rA,rB)
#endif

// We attach a register declarator to each parameter so that CW does not generate
// code to save the values of the registers we use explicitly.

NAMESPACE_ZOOLIB_BEGIN

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

asm bool ZAtomic_CompareAndSwap(register ZAtomic_t* iAtomic,
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
	ZAtomic_PPC405_ERR77(0, r3)
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

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)

// =================================================================================================
#pragma mark -
#pragma mark * CodeWarrior/x86

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)

NAMESPACE_ZOOLIB_BEGIN

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
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
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
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
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
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
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
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
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock inc dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock dec dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)

// =================================================================================================
#pragma mark -
#pragma mark * Mach (OSX/iPhone)

#if defined(__MACH__) && ! ZCONFIG(Compiler, CodeWarrior)

#include <libkern/OSAtomic.h>

NAMESPACE_ZOOLIB_BEGIN

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{ return ::OSAtomicCompareAndSwap32Barrier(iOldValue, iNewValue, (int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{ return ::OSAtomicAdd32Barrier(iParam, (int32_t*)&iAtomic->fValue) - iParam; }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 0 == ::OSAtomicAdd32Barrier(-1, (int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{ ::OSAtomicIncrement32((int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{ ::OSAtomicDecrement32((int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------

NAMESPACE_ZOOLIB_END

#endif // defined(__MACH__)

// =================================================================================================
#pragma mark -
#pragma mark * Windows

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZWinHeader.h"

NAMESPACE_ZOOLIB_BEGIN

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	return iOldValue
		== ::InterlockedCompareExchange((LONG*)&iAtomic->fValue, iNewValue, iOldValue);
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{ return ::InterlockedExchange((LONG*)&iAtomic->fValue, iParam); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{ return ::InterlockedExchangeAdd((LONG*)&iAtomic->fValue, iParam); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 0 == ::InterlockedDecrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{ ::InterlockedIncrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{ ::InterlockedDecrement((LONG*)&iAtomic->fValue); }

#endif
// -----------------------------------------------

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
#pragma mark -
#pragma mark * A real CompareAndSwap must be defined by now

#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#	error No CompareAndSwap available
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Default implementations that depend on CAS or ZAtomic_Add.

NAMESPACE_ZOOLIB_BEGIN

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	for (;;)
		{
		int prior = iAtomic->fValue;
		if (ZAtomic_CompareAndSwap(iAtomic, prior, iParam))
			return prior;
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	for (;;)
		{
		int prior = iAtomic->fValue;
		if (ZAtomic_CompareAndSwap(iAtomic, prior, prior + iParam))
			return prior;
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 1 == ZAtomic_Add(iAtomic, -1); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{ ZAtomic_Add(iAtomic, 1); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{ ZAtomic_Add(iAtomic, -1); }

#endif
// -----------------------------------------------

NAMESPACE_ZOOLIB_END
