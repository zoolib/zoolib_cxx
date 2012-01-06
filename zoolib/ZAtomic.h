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
#pragma mark * ZAtomic

struct ZAtomic_t
	{
	ZAtomic_t() {}
	ZAtomic_t(int iValue) : fValue(iValue) {}
	volatile int fValue;
	};

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

bool ZAtomic_CompareAndSwapPtr(void* iPtrAddress, void* iOldValue, void* iNewValue);

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);
int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);

int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);

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

bool ZAtomic_CompareAndSwapPtr(ZAtomicPtr_t* iAtomicPtr, void* iOldValue, void* iNewValue);

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * "CodeWarrior/68K"

#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, 68K)

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

inline asm bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic : __A0,
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
#pragma mark * GCC/PPC

#if ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, PPC)

/* There's a reference in the Linux kernel source to Erratum #77 for the
PowerPC 405. I'm not sure which, if any, Macintoshes use this processor.
The issue is that a sync or dcbt instruction must be issued before any
stwcx. instruction. I'm defining a null macro for this for now, but it
can be enabled later if necessary. */

#if 0
#	define ZAtomic_PPC405_ERR77(rA,rB) "dcbt " #ra "," #rb "\n"
#else
#	define ZAtomic_PPC405_ERR77(rA,rB)
#endif

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

inline bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	int oldValue;
	asm volatile
		(
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
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

inline int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	int oldValue;
	asm volatile
		(
		"1: lwarx %1, 0, %2\n"
		ZAtomic_PPC405_ERR77(0, %2)
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
#pragma mark * GCC/x86

#if ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, x86)

#define ZooLib_SMPLockPrefix "lock; "

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

inline bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
	{
	bool result;
	asm volatile
		(
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
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

inline int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
	{
	asm volatile
		(
		ZooLib_SMPLockPrefix "xchg %1, %0"
		: "=m" (iAtomic->fValue), "=q" (iParam)
		: "m" (iAtomic->fValue), "1" (iParam)
		);
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

inline int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	asm volatile
		(
		ZooLib_SMPLockPrefix "xadd %1, %0"
		: "=m" (iAtomic->fValue), "=q" (iParam)
		: "m" (iAtomic->fValue), "1" (iParam)
		);
	return iParam;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

inline bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	bool isZero;
	asm volatile
		(
		ZooLib_SMPLockPrefix "decl %0\n"
		"sete %1"
		: "=m" (iAtomic->fValue), "=qm" (isZero)
		: "m" (iAtomic->fValue)
		);
	return isZero;
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

inline void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm volatile
		(
		ZooLib_SMPLockPrefix "incl %0"
		: "=m" (iAtomic->fValue)
		: "m" (iAtomic->fValue)
		);
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

inline void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm volatile
		(
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
#pragma mark * MSVC/x86

#if ZCONFIG(Compiler, MSVC) && ZCONFIG(Processor, x86)

namespace ZooLib {

// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_CompareAndSwap)
#define DEFINED_ZAtomic_CompareAndSwap 1

inline bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue)
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
#if !defined(DEFINED_ZAtomic_Swap)
#define DEFINED_ZAtomic_Swap 1

inline int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
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
#if !defined(DEFINED_ZAtomic_Add)
#define DEFINED_ZAtomic_Add 1

inline int ZAtomic_Add(ZAtomic_t* iAtomic, register int iParam)
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
#if !defined(DEFINED_ZAtomic_DecAndTest)
#define DEFINED_ZAtomic_DecAndTest 1

inline bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
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
#if !defined(DEFINED_ZAtomic_Inc)
#define DEFINED_ZAtomic_Inc 1

inline void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	__asm
		{
		mov edx, iAtomic
		lock inc dword ptr [edx]
		}
	}

#endif
// -----------------------------------------------
#if !defined(DEFINED_ZAtomic_Dec)
#define DEFINED_ZAtomic_Dec 1

inline void ZAtomic_Dec(ZAtomic_t* iAtomic)
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

#endif // __ZAtomic_h__
