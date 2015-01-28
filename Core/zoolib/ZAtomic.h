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

#ifndef __ZAtomic_h__
#define __ZAtomic_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZAtomic

struct ZAtomic_t
	{
	ZAtomic_t() {}
	ZAtomic_t(int iValue) : fValue(iValue) {}
	volatile int fValue;
	};

inline int sAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void sAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

bool sAtomicPtr_CAS(void* iPtrAddress, void* iOldValue, void* iNewValue);

bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);
int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam);

int sAtomic_Add(ZAtomic_t* iAtomic, int iParam);

int sAtomic_And(ZAtomic_t* iAtomic, int iParam);
int sAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int sAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool sAtomic_DecAndTest(ZAtomic_t* iAtomic);
void sAtomic_Inc(ZAtomic_t* iAtomic);
void sAtomic_Dec(ZAtomic_t* iAtomic);

struct ZAtomicPtr_t
	{
	ZAtomicPtr_t() {}
	ZAtomicPtr_t(void* iValue) : fValue(iValue) {}
	void* fValue;
	};

inline void* ZAtomicPtr_Get(const ZAtomicPtr_t* iAtomicPtr)
	{ return iAtomicPtr->fValue; }

inline void ZAtomicPtr_Set(ZAtomicPtr_t* iAtomicPtr, void* iParam)
	{ iAtomicPtr->fValue = iParam; }

void sAtomic_Barrier();

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark "CodeWarrior/68K"

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, 68K)

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

inline asm bool sAtomic_CAS(ZAtomic_t* iAtomic : __A0,
	int iOldValue : __D0, int iNewValue : __D1)
	{
	dc.l 0x0ED00040 // cas.l d0, d1, (a0)
	seq d0
	moveq #1, d1
	and.l d1, d0
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, 68K)

// =================================================================================================
#pragma mark -
#pragma mark GCC/PPC

#if ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, PPC)

/* There's a reference in the Linux kernel source to Erratum #77 for the
PowerPC 405. I'm not sure which, if any, Macintoshes use this processor.
The issue is that a sync or dcbt instruction must be issued before any
stwcx. instruction. I'm defining a null macro for this for now, but it
can be enabled later if necessary. */

#if 0
	#define sAtomic_PPC405_ERR77(rA,rB) "dcbt " #ra "," #rb "\n"
#else
	#define sAtomic_PPC405_ERR77(rA,rB)
#endif

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

inline bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	int oldValue;
	asm volatile(
		
		"1: lwarx %1, 0, %2\n"
		"cmpw %1, %3\n"
		"beq 2f\n"
		"mr %4, %1\n"
		"2: stwcx. %4, 0, %2\n"
		"bne- 1b\n"
		"isync\n"
		: "=m" (iAtomic->fValue), "=&r" (oldValue)
		: "r" (iAtomic), "r" (iOldValue), "r" (iNewValue)
		: "cc"
		);
	return oldValue == iOldValue;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

inline int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	int oldValue;
	asm volatile(
		
		"1: lwarx %1, 0, %2\n"
		sAtomic_PPC405_ERR77(0, %2)
		"stwcx. %3, 0, %2\n"
		"bne- 1b\n"
		"isync\n"
		: "=m" (iAtomic->fValue), "=&r" (oldValue)
		: "r" (iAtomic), "r" (iParam)
		: "cc"
		);
	return oldValue;
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, PPC)

// =================================================================================================
#pragma mark -
#pragma mark GCC/x86

#if ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, x86)

#define ZooLib_SMPLockPrefix "lock; "

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

inline bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	bool result;
	asm volatile(
		
		ZooLib_SMPLockPrefix "cmpxchg %2, %0\n"
		"sete %1"
		: "=m" (iAtomic->fValue), "=qm" (result)
		: "q" (iNewValue), "a" (iOldValue), "m" (iAtomic->fValue)
		: "cc"
		);
	return result;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

inline int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	asm volatile(
		
		ZooLib_SMPLockPrefix "xchg %1, %0"
		: "=m" (iAtomic->fValue), "=q" (iParam)
		: "m" (iAtomic->fValue), "1" (iParam)
		);
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Add)
#define DEFINED_sAtomic_Add 1

inline int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	asm volatile(
		
		ZooLib_SMPLockPrefix "xadd %1, %0"
		: "=m" (iAtomic->fValue), "=q" (iParam)
		: "m" (iAtomic->fValue), "1" (iParam)
		);
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_DecAndTest)
#define DEFINED_sAtomic_DecAndTest 1

inline bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	bool isZero;
	asm volatile(
		
		ZooLib_SMPLockPrefix "decl %0\n"
		"sete %1"
		: "=m" (iAtomic->fValue), "=qm" (isZero)
		: "m" (iAtomic->fValue)
		);
	return isZero;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Inc)
#define DEFINED_sAtomic_Inc 1

inline void sAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm volatile(
		
		ZooLib_SMPLockPrefix "incl %0"
		: "=m" (iAtomic->fValue)
		: "m" (iAtomic->fValue)
		);
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

inline void sAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm volatile(
		
		ZooLib_SMPLockPrefix "decl %0"
		: "=m" (iAtomic->fValue)
		: "m" (iAtomic->fValue)
		);
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, x86)

// =================================================================================================
#pragma mark -
#pragma mark MSVC/x86

#if ZCONFIG(Compiler, MSVC) && ZCONFIG(Processor, x86)

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

inline bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	bool result;
	__asm
		{
		mov eax, iOldValue
		mov ecx, iNewValue
		mov edx, iAtomic
		lock cmpxchg [edx], ecx
		sete result
		}
	return result;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Swap)
#define DEFINED_sAtomic_Swap 1

inline int sAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	__asm
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

inline int sAtomic_Add(ZAtomic_t* iAtomic, register int iParam)
	{
	__asm
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

inline bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	bool isZero;
	__asm
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

inline void sAtomic_Inc(ZAtomic_t* iAtomic)
	{
	__asm
		{
		mov edx, iAtomic
		lock inc dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

inline void sAtomic_Dec(ZAtomic_t* iAtomic)
	{
	__asm
		{
		mov edx, iAtomic
		lock dec dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // ZCONFIG(Compiler, MSVC) && ZCONFIG(Processor, x86)

// =================================================================================================
#pragma mark -
#pragma mark Mach (OSX/iPhone)

#if defined(__MACH__) && ! ZCONFIG(Compiler, CodeWarrior)

#include <libkern/OSAtomic.h>

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_sAtomicPtr_CAS)
#define DEFINED_sAtomicPtr_CAS 1

inline
bool sAtomicPtr_CAS(void* iPtrAddress, void* iOldValue, void* iNewValue)
	{
	#if ZCONFIG_Is64Bit
		return ::OSAtomicCompareAndSwap64(
			(int64_t)iOldValue, (int64_t)iNewValue, (int64_t*)iPtrAddress);
	#else
		return ::OSAtomicCompareAndSwap32(
			(int32_t)iOldValue, (int32_t)iNewValue, (int32_t*)iPtrAddress);
	#endif
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_CAS)
#define DEFINED_sAtomic_CAS 1

inline
bool sAtomic_CAS(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{ return ::OSAtomicCompareAndSwap32(iOldValue, iNewValue, (int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Add)
#define DEFINED_sAtomic_Add 1

inline
int sAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{ return ::OSAtomicAdd32(iParam, (int32_t*)&iAtomic->fValue) - iParam; }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_DecAndTest)
#define DEFINED_sAtomic_DecAndTest 1

inline
bool sAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{ return 0 == ::OSAtomicAdd32(-1, (int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Inc)
#define DEFINED_sAtomic_Inc 1

inline
void sAtomic_Inc(ZAtomic_t* iAtomic)
	{ ::OSAtomicIncrement32((int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Dec)
#define DEFINED_sAtomic_Dec 1

inline
void sAtomic_Dec(ZAtomic_t* iAtomic)
	{ ::OSAtomicDecrement32((int32_t*)&iAtomic->fValue); }

#endif
// -----------------------------------------------
#if !defined(DEFINED_sAtomic_Barrier)
#define DEFINED_sAtomic_Barrier 1

inline
void sAtomic_Barrier()
	{ ::OSMemoryBarrier(); }

#endif
// -----------------------------------------------

} // namespace ZooLib

#endif // defined(__MACH__) && ! ZCONFIG(Compiler, CodeWarrior)

// =================================================================================================

#endif // __ZAtomic_h__
