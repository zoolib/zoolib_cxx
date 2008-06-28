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

// =================================================================================================
#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)
#pragma mark -
#pragma mark * CodeWarrior/PPC/Not BeOS

// We attach a register declarator to each parameter so that CW does not generate
// code to save the values of the registers we use explicitly.

asm int ZAtomic_Swap(register ZAtomic_t* iAtomic, register int iParam)
	{
	// r3 = iAtomic
	// r4 = iParam
	// r5 = oldValue
tryAgain:
	lwarx r5, 0, r3
	ZAtomic_PPC405_ERR77(0, r3)
	stwcx. r4, 0, r3
	bne- tryAgain
	isync
	mr r3, r5
	blr
	}

asm bool ZAtomic_CompareAndSwap(register ZAtomic_t* iAtomic,
	register int iOldValue, register int inNewValue)
	{
	// r3 = iAtomic
	// r4 = iOldValue
	// r5 = inNewValue
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

// r3 = iAtomic
// r4 = iParam
// r5 = oldValue (returned in r3)
// r6 = newValue
#define ZAtomic_Op_Macro(theOp) \
tryAgain: \
	lwarx r5, 0, r3; \
	theOp r6, r5, r4; \
	ZAtomic_PPC405_ERR77(0, r3) \
	stwcx. r6, 0, r3; \
	bne- tryAgain; \
	mr r3, r5; \
	isync; \
	blr

asm int ZAtomic_Add(register ZAtomic_t* iAtomic, register int iParam)
	{
	ZAtomic_Op_Macro(add);
	}

asm int ZAtomic_And(register ZAtomic_t* iAtomic, register int iParam)
	{
	ZAtomic_Op_Macro(and);
	}

asm int ZAtomic_Or(register ZAtomic_t* iAtomic, register int iParam)
	{
	ZAtomic_Op_Macro(or);
	}

asm int ZAtomic_Xor(register ZAtomic_t* iAtomic, register int iParam)
	{
	ZAtomic_Op_Macro(xor);
	}

#undef ZAtomic_Op_Macro

asm bool ZAtomic_DecAndTest(register ZAtomic_t* iAtomic)
	{
	// r3 = iAtomic
	// r4 = oldValue
	// r5 = newValue
tryAgain:
	lwarx r4, 0, r3
	subi r5, r4, 1
	ZAtomic_PPC405_ERR77(0, r3)
	stwcx. r5, 0, r3
	bne tryAgain
	cmpi cr0, r5, 0
	bne+ cr0, notZero
	li r3, 1
	isync
	blr
notZero:
	li r3, 0
	blr
	}

asm void ZAtomic_Inc(register ZAtomic_t* iAtomic)
	{
	// r3 = iAtomic
	// r4 = oldValue
	// r5 = newValue;
tryAgain:
	lwarx r4, 0, r3
	addi r5, r4, 1
	ZAtomic_PPC405_ERR77(0, r3)
	stwcx. r5, 0, r3
	bne tryAgain
	blr
	}

asm void ZAtomic_Dec(register ZAtomic_t* iAtomic)
	{
	// r3 = iAtomic
	// r4 = oldValue
	// r5 = newValue;
tryAgain:
	lwarx r4, 0, r3
	subi r5, r4, 1
	ZAtomic_PPC405_ERR77(0, r3)
	stwcx. r5, 0, r3
	bne tryAgain
	blr
	}

// =================================================================================================
#elif ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)
#pragma mark -
#pragma mark * CodeWarrior/Intel x86

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

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int inNewValue)
	{
	asm
		{
		mov eax, iOldValue
		mov ecx, inNewValue
		mov edx, iAtomic
		lock cmpxchg [edx], ecx
		sete al
		}
	}

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

#define ZAtomic_Op_Macro(theOp) \
	int oldValue; \
	__asm \
		{ \
		__asm mov edx, iAtomic \
		__asm tryAgain: mov eax, [edx] \
		__asm mov ecx, eax \
		__asm theOp ecx, iParam \
		__asm lock cmpxchg [edx], ecx \
		__asm jne tryAgain \
		__asm mov oldValue, eax \
		} \
	return oldValue;

int ZAtomic_And(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(and);
	}

int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(or);
	}

int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(xor);
	}

#undef ZAtomic_Op_Macro

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

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock inc dword ptr [edx]
		}
	}

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm
		{
		mov edx, iAtomic
		lock dec dword ptr [edx]
		}
	}

// =================================================================================================
#elif ZCONFIG(Compiler, MSVC) && ZCONFIG(Processor, x86)
#pragma mark -
#pragma mark * MSVC/Intel x86

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam)
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

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int inNewValue)
	{
	bool result;
	__asm
		{
		mov eax, iOldValue
		mov ecx, inNewValue
		mov edx, iAtomic
		lock cmpxchg [edx], ecx
		sete result
		}
	return result;
	}


int ZAtomic_Add(ZAtomic_t* iAtomic, register int iParam)
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

int ZAtomic_And(ZAtomic_t* iAtomic, int iParam)
	{
	int oldValue;
	__asm
		{
		mov edx, [iAtomic]
	tryAgain:
		mov eax, [edx]
		mov ebx, eax
		and ebx, iParam
		lock cmpxchg [edx], ebx
		jne tryAgain
		mov oldValue, eax
		}
	return oldValue;
	}

int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam)
	{
	int oldValue;
	__asm
		{
		mov edx, iAtomic
	tryAgain:
		mov eax, [edx]
		mov ebx, eax
		or ebx, iParam
		lock cmpxchg [edx], ebx
		jne tryAgain
		mov oldValue, eax
		}
	return oldValue;
	}

int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam)
	{
	int oldValue;
	__asm
		{
		mov edx, iAtomic
	tryAgain:
		mov eax, [edx]
		mov ebx, eax
		xor ebx, iParam
		lock cmpxchg [edx], ebx
		jne tryAgain
		mov oldValue, eax
		}
	return oldValue;
	}

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
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

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	__asm
		{
		mov edx, iAtomic

		lock inc dword ptr [edx]
		}
	}

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	__asm
		{
		mov edx, iAtomic

		lock dec dword ptr [edx]
		}
	}

// =================================================================================================
#elif ZCONFIG_AtomicDumb
#pragma mark -
#pragma mark * Dumb version

#include "zoolib/ZThread.h"

// Note that we actually use a semaphore as our locking primitive, not a ZMutexNR, which
// is a benaphore and thus relies on calls to ZAtomic_Inc and ZAtomic_Dec -- which
// are the very methods we're trying to implement here.

static ZSemaphore sSemaphore(1);

int ZAtomic_Swap(register ZAtomic_t* iAtomic, int iParam)
	{
	sSemaphore.Wait(1);
	int priorValue = iAtomic->fValue;
	iAtomic->fValue = iParam;
	sSemaphore.Signal(1);
	return priorValue;
	}

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int inNewValue)
	{
	sSemaphore.Wait(1);
	if (iAtomic->fValue == iOldValue)
		{
		iAtomic->fValue = inNewValue;
		sSemaphore.Signal(1);
		return true;
		}
	else
		{
		sSemaphore.Signal(1);
		return false;
		}
	}

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	sSemaphore.Wait(1);
	int priorValue = iAtomic->fValue;
	iAtomic->fValue = priorValue + iParam;
	sSemaphore.Signal(1);
	return priorValue;
	}

int ZAtomic_And(ZAtomic_t* iAtomic, int iParam)
	{
	sSemaphore.Wait(1);
	int priorValue = iAtomic->fValue;
	iAtomic->fValue = priorValue & iParam;
	sSemaphore.Signal(1);
	return priorValue;
	}

int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam)
	{
	sSemaphore.Wait(1);
	int priorValue = iAtomic->fValue;
	iAtomic->fValue = priorValue | iParam;
	sSemaphore.Signal(1);
	return priorValue;
	}

int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam)
	{
	sSemaphore.Wait(1);
	int priorValue = iAtomic->fValue;
	iAtomic->fValue = priorValue ^ iParam;
	sSemaphore.Signal(1);
	return priorValue;
	}

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	sSemaphore.Wait(1);
	bool isZero = --iAtomic->fValue == 0;
	sSemaphore.Signal(1);
	return isZero;
	}

void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	sSemaphore.Wait(1);
	++iAtomic->fValue;
	sSemaphore.Signal(1);
	}

void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	sSemaphore.Wait(1);
	--iAtomic->fValue;
	sSemaphore.Signal(1);
	}

#endif

// =================================================================================================
static void sTestAtomic()
	{
	ZAtomic_t t1;
	ZAtomic_Swap(&t1, 3);
	int result1 = ZAtomic_Get(&t1);
	int result2 = ZAtomic_CompareAndSwap(&t1, 3, 5);
	int result3 = ZAtomic_Add(&t1, 10);
	int result4 = ZAtomic_And(&t1, 0xFFFF0000);
	ZAtomic_Inc(&t1);
	ZAtomic_Dec(&t1);
	if (ZAtomic_DecAndTest(&t1))
		ZAtomic_Inc(&t1);
	}
