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

#ifndef __ZAtomic__
#define __ZAtomic__ 1
#include "zconfig.h"

#include "zoolib/ZDebug.h" // For ZAssertCompile

// AG 2000-02-06. Much of the code in this file inlines. For that to work we need the various
// compilers to see quite different code. The first section of this file is #ifed out of existence,
// but should be considered to be the canonical API. The second section has the various versions of
// the real code.

NAMESPACE_ZOOLIB_BEGIN

struct ZAtomic_t
	{
	ZAtomic_t() {}
	ZAtomic_t(int iValue) : fValue(iValue) {}
	volatile int fValue;
	};

// Ensure that ZAtomic_t packs properly
ZAssertCompile(sizeof(ZAtomic_t) == sizeof(int));

#if 0
int ZAtomic_Get(const ZAtomic_t* iAtomic);
void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam);

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam);
bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);
#endif


// AG 2003-06-17. I just came across a posting by Linus Torvalds to LKML wherein he says that
// the lock prefix on operations causes a factor of 12 slowdown of those operations. It's
// only needed on SMP machines which we continue to assume we may be running on. If you know
// different, override by defining ZCONFIG_Atomic_SMP as 0 in your zconfig.h.
#ifndef ZCONFIG_Atomic_SMP
#	define ZCONFIG_Atomic_SMP 1
#endif

// =================================================================================================
/* There's a reference in the Linux kernel source to Erratum #77 for the
PowerPC 405. I'm not sure which, if any, Macintoshes use this processor.
The issue is that a sync or dcbt instruction must be issued before any
stwcx. instruction. I'm defining a null macro for this for now, but it
can be enabled later if necessary. */

#if 1
#	define ZAtomic_PPC405_ERR77(rA,rB)
#else
#	if ZCONFIG(Compiler, GCC)
#		define ZAtomic_PPC405_ERR77(rA,rB) "dcbt " #ra "," #rb "\n" 
#	elif ZCONFIG(Compiler, CodeWarrior)
#		define ZAtomic_PPC405_ERR77(rA,rB) dcbt rA , rB;
#	else
#		error Unsupported
#	endif
#endif

// =================================================================================================
#if ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, 68K)
#pragma mark -
#pragma mark * CodeWarrior/68K

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

asm inline int ZAtomic_Swap(ZAtomic_t* iAtomic : __A0, int iParam : __D1)
	{
tryagain:
	move.l (a0), d0
	dc.l 0x0ED00040 // cas.l d0, d1, (a0)
	bne.s tryagain
	}

asm inline bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic : __A0,
	int iOldValue : __D0, int iNewValue : __D1)
	{
	dc.l 0x0ED00040 // cas.l d0, d1, (a0)
	seq d0
	moveq #1, d1
	and.l d1, d0
	}

// dc.l 0x0ED00080 = cas.l d0, d2, (a0)

#define ZAtomic_Op_Macro(theOp) \
tryagain: \
	move.l (a0), d0; \
	move.l d0, d2; \
	theOp d1, d2; \
	dc.l 0x0ED00080; \
	bne tryagain

asm inline int ZAtomic_Add(ZAtomic_t* iAtomic : __A0, int iParam : __D1)
	{
	ZAtomic_Op_Macro(add.l);
	}

asm inline int ZAtomic_And(ZAtomic_t* iAtomic : __A0, int iParam : __D1)
	{
	ZAtomic_Op_Macro(and.l);
	}

asm inline int ZAtomic_Or(ZAtomic_t* iAtomic : __A0, int iParam : __D1)
	{
	ZAtomic_Op_Macro(or.l);
	}

asm inline int ZAtomic_Xor(ZAtomic_t* iAtomic : __A0, int iParam : __D1)
	{
	ZAtomic_Op_Macro(eor.l);
	}

#undef ZAtomic_Op_Macro

asm inline bool ZAtomic_DecAndTest(ZAtomic_t* : __A0)
	{
	subq.l #1, (a0)
	seq d0
	moveq #1, d1
	and.l d1, d0
	}

asm inline void ZAtomic_Inc(ZAtomic_t* : __A0)
	{
	add.l #1, (a0)
	}

asm inline void ZAtomic_Dec(ZAtomic_t* : __A0)
	{
	sub.l #1, (a0)
	}

// =================================================================================================
#elif ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, PPC)
#pragma mark -
#pragma mark * CodeWarrior/PPC/Not BeOS

// Although CW can inline PPC assembler, for some reason it has a hard time satisfying the
// constraints of enclosing code and the inlined stuff simultaneously. We're able to use one (!)
// register in inlined code, but almost all this code requires two or more. The code is thus defined
// out of line in ZAtomic.cpp.

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

int ZAtomic_Swap(ZAtomic_t* iAtomic, int iParam);
bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);

// =================================================================================================
#elif ZCONFIG(Compiler, CodeWarrior) && ZCONFIG(Processor, x86)
#pragma mark -
#pragma mark * CodeWarrior/Intel x86

// CodeWarrior sucks rocks when it comes to reliably including inline x86 assmbly with general C/C++
// code, or generating the correct opcodes for parameters. To work around the problem the code has
// to do superfluous register/register transfers, causing massive bloat. So for the time being these
// routines are defined out of line in ZAtomic.cpp, which will also make it easier to tweak them
// without having massive rebuilds.

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

int ZAtomic_Swap(register ZAtomic_t* iAtomic, int iParam);
bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);

// =================================================================================================
#elif ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, PPC)
#pragma mark -
#pragma mark * GCC/PPC

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

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
		:  "r" (iAtomic), "r" (iOldValue), "r" (iNewValue)
		: "cc"
		);
	return oldValue == iOldValue;
	}

#define ZAtomic_Op_Macro(theOp) \
	int oldValue, temp; \
	asm volatile \
		( \
		"1: lwarx %1, 0, %3\n" \
		#theOp" %2, %1, %4\n" \
		ZAtomic_PPC405_ERR77(0, %3) \
		"stwcx. %2, 0, %3\n" \
		"bne- 1b\n" \
		"isync\n" \
		: "=m" (iAtomic->fValue), "=&r" (oldValue), "=&r" (temp) \
		:  "r" (iAtomic), "r" (iParam) \
		: "cc" \
		); \
	return oldValue

inline int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(add);
	}

inline int ZAtomic_And(ZAtomic_t* iAtomic,int iParam)
	{
	ZAtomic_Op_Macro(and);
	}

inline int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(or);
	}

inline int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(xor);
	}

#undef ZAtomic_Op_Macro

inline bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic)
	{
	return ZAtomic_Add(iAtomic, -1) == 1;
	}

inline void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	int temp;
	asm volatile
		(
		"1: lwarx %1, 0, %2\n"
		"addic %1, %1, 1\n"
		ZAtomic_PPC405_ERR77(0, %2)
		"stwcx. %1, 0, %2\n"
		"bne- 1b"
		: "=m" (iAtomic->fValue), "=&r" (temp)
		:  "r" (iAtomic)
		: "cc"
		);
	}

inline void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	int temp;
	asm volatile
		(
		"1: lwarx %1, 0, %2\n"
		"addic %1, %1, -1\n"
		ZAtomic_PPC405_ERR77(0, %2)
		"stwcx. %1, 0, %2\n"
		"bne- 1b"
		: "=m" (iAtomic->fValue), "=&r" (temp)
		:  "r" (iAtomic)
		: "cc"
		);
	}

// =================================================================================================
#elif ZCONFIG(Compiler, GCC) && ZCONFIG(Processor, x86)
#pragma mark -
#pragma mark * GCC/Intel x86

#if ZCONFIG_Atomic_SMP
#	define ZooLib_SMPLockPrefix "lock; "
#else
#	define ZooLib_SMPLockPrefix
#endif

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

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

#define ZAtomic_Op_Macro(theOp) \
	int oldValue, temp1; \
	asm volatile \
		( \
		"1: mov %0, %2\n" \
		"mov %2, %3\n" \
		#theOp" %1, %3\n" \
		ZooLib_SMPLockPrefix "cmpxchg %3, %0\n" \
		"jne 1b" \
		: "=m" (iAtomic->fValue), "=qm" (iParam), "=a" (oldValue), "=q" (temp1) \
		: "m" (iAtomic->fValue), "1" (iParam)\
		: "cc" \
		); \
	return oldValue

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

inline int ZAtomic_And(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(and);
	}

inline int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(or);
	}

inline int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam)
	{
	ZAtomic_Op_Macro(xor);
	}

#undef ZAtomic_Op_Macro

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

inline void ZAtomic_Inc(ZAtomic_t* iAtomic)
	{
	asm volatile
		(
		ZooLib_SMPLockPrefix "incl %0"
		: "=m" (iAtomic->fValue)
		: "m" (iAtomic->fValue)
		);
	}

inline void ZAtomic_Dec(ZAtomic_t* iAtomic)
	{
	asm volatile
		(
		ZooLib_SMPLockPrefix "decl %0"
		: "=m" (iAtomic->fValue)
		: "m" (iAtomic->fValue)
		);
	}

#undef ZooLib_SMPLockPrefix

// =================================================================================================
#elif ZCONFIG(Compiler, MSVC) && ZCONFIG(Processor, x86)
#pragma mark -
#pragma mark * MSVC/Intel x86

// Defined out of line in ZAtomic.cpp till I can determine how effective MSVC is at
// inlining assembly code.

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

int ZAtomic_Swap(register ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);


// =================================================================================================
#else // ZCONFIG(Compiler), ZCONFIG(Processor)
#pragma mark -
#pragma mark * Dumb version

#define ZCONFIG_AtomicDumb 1

// Defined out of line in ZAtomic.cpp, using a global mutex to ensure atomicity. Performance will
// not be great, but it will at least work everywhere that we have a ZThread implementation.

inline int ZAtomic_Get(const ZAtomic_t* iAtomic)
	{ return iAtomic->fValue; }

inline void ZAtomic_Set(ZAtomic_t* iAtomic, int iParam)
	{ iAtomic->fValue = iParam; }

int ZAtomic_Swap(register ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_CompareAndSwap(ZAtomic_t* iAtomic, int iOldValue, int iNewValue);

int ZAtomic_Add(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_And(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Or(ZAtomic_t* iAtomic, int iParam);
int ZAtomic_Xor(ZAtomic_t* iAtomic, int iParam);

bool ZAtomic_DecAndTest(ZAtomic_t* iAtomic);
void ZAtomic_Inc(ZAtomic_t* iAtomic);
void ZAtomic_Dec(ZAtomic_t* iAtomic);

#endif // ZCONFIG(Compiler)/ZCONFIG(Processor)

// =================================================================================================

NAMESPACE_ZOOLIB_END

#endif // __ZAtomic__
