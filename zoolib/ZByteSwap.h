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

#ifndef __ZByteSwap_h__
#define __ZByteSwap_h__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h"

namespace ZooLib {

// AG 2000-02-06. Much of the code in this file inlines. For that to work we need the various
// compilers to see quite different code. The first section of this file is #ifed out of
// existence, but should be considered to be the canonical API. The second section has the
// various versions of the real code.

#if 0
// =================================================================================================
// MARK: - API synopsis

// We define processor-specific versions of these calls below.
int16 ZByteSwap_16(int16 iValue);
int32 ZByteSwap_32(int32 iValue);
int64 ZByteSwap_64(int64 iValue);

void ZByteSwap_16(volatile void* ioValueAddress);
void ZByteSwap_32(volatile void* ioValueAddress);
void ZByteSwap_64(volatile void* ioValueAddress);

int16 ZByteSwap_Read16(const volatile void* iValueAddress);
int32 ZByteSwap_Read32(const volatile void* iValueAddress);
int64 ZByteSwap_Read64(const volatile void* iValueAddress);

void ZByteSwap_Write16(volatile void* oValueAddress, int16 iValue);
void ZByteSwap_Write32(volatile void* oValueAddress, int32 iValue);
void ZByteSwap_Write64(volatile void* oValueAddress, int64 iValue);



// And we define endian-specific versions of these calls that map to the appropriate
// unconditional calls, or are no-ops. We define the symbol ZByteSwap_EndianBig
// for use only in this file. It is undefed before we exit. You should use
// ZCONFIG_Endian to determine the endianism of your target.

int16 ZByteSwap_BigToLittle16(int16 iValue);
int32 ZByteSwap_BigToLittle32(int32 iValue);
int64 ZByteSwap_BigToLittle64(int64 iValue);

void ZByteSwap_BigToLittle16(int16* ioValueAddress);
void ZByteSwap_BigToLittle32(int32* ioValueAddress);
void ZByteSwap_BigToLittle64(int64* ioValueAddress);



int16 ZByteSwap_LittleToBig16(int16 iValue);
int32 ZByteSwap_LittleToBig32(int32 iValue);
int64 ZByteSwap_LittleToBig64(int64 iValue);

void ZByteSwap_LittleToBig16(int16* ioValueAddress);
void ZByteSwap_LittleToBig32(int32* ioValueAddress);
void ZByteSwap_LittleToBig64(int64* ioValueAddress);



int16 ZByteSwap_HostToBig16(int16 iValue);
int32 ZByteSwap_HostToBig32(int32 iValue);
int64 ZByteSwap_HostToBig64(int64 iValue);

void ZByteSwap_HostToBig16(int16* ioValueAddress);
void ZByteSwap_HostToBig32(int32* ioValueAddress);
void ZByteSwap_HostToBig64(int64* ioValueAddress);



int16 ZByteSwap_HostToLittle16(int16 iValue);
int32 ZByteSwap_HostToLittle32(int32 iValue);
int64 ZByteSwap_HostToLittle64(int64 iValue);

void ZByteSwap_HostToLittle16(int16* ioValueAddress);
void ZByteSwap_HostToLittle32(int32* ioValueAddress);
void ZByteSwap_HostToLittle64(int64* ioValueAddress);



int16 ZByteSwap_BigToHost16(int16 iValue);
int32 ZByteSwap_BigToHost32(int32 iValue);
int64 ZByteSwap_BigToHost64(int64 iValue);

void ZByteSwap_BigToHost16(int16* ioValueAddress);
void ZByteSwap_BigToHost32(int32* ioValueAddress);
void ZByteSwap_BigToHost64(int64* ioValueAddress);



int16 ZByteSwap_LittleToHost16(int16 iValue);
int32 ZByteSwap_LittleToHost32(int32 iValue);
int64 ZByteSwap_LittleToHost64(int64 iValue);

void ZByteSwap_LittleToHost16(int16* ioValueAddress);
void ZByteSwap_LittleToHost32(int32* ioValueAddress);
void ZByteSwap_LittleToHost64(int64* ioValueAddress);



int16 ZByteSwap_ReadBig16(const volatile void* iValueAddress);
int32 ZByteSwap_ReadBig32(const volatile void* iValueAddress);
int64 ZByteSwap_ReadBig64(const volatile void* iValueAddress);

int16 ZByteSwap_ReadLittle16(const volatile void* iValueAddress);
int32 ZByteSwap_ReadLittle32(const volatile void* iValueAddress);
int64 ZByteSwap_ReadLittle64(const volatile void* iValueAddress);


void ZByteSwap_WriteBig16(volatile void* oValueAddress, int16 iValue);
void ZByteSwap_WriteBig32(volatile void* oValueAddress, int32 iValue);
void ZByteSwap_WriteBig64(volatile void* oValueAddress, int64 iValue);

void ZByteSwap_WriteLittle16(volatile void* oValueAddress, int16 iValue);
void ZByteSwap_WriteLittle32(volatile void* oValueAddress, int32 iValue);
void ZByteSwap_WriteLittle64(volatile void* oValueAddress, int64 iValue);

#endif // 0

// =================================================================================================
#if ZCONFIG(Processor, 68K)
#pragma mark -
#pragma mark * Processor is 68K

#define ZByteSwap_EndianBig 1

#if ZCONFIG(Compiler, CodeWarrior)
asm inline int16 ZByteSwap_16(int16 : __D0)
	{
	rol.w #8, d0
	}

asm inline int32 ZByteSwap_32(int32 : __D0)
	{
	rol.w #8, d0
	swap d0
	rol.w #8, d0
	}



asm inline void ZByteSwap_16(volatile void* : __A0)
	{
	move.w (a0), d0
	rol.w #8, d0
	move.w d0, (a0)
	}

asm inline void ZByteSwap_32(volatile void* : __A0)
	{
	move.l (a0), d0
	rol.w #8, d0
	swap d0
	rol.w #8, d0
	move.l d0, (a0)
	}



asm inline int16 ZByteSwap_Read16(const volatile void* iValueAddress : __A0)
	{
	move.w (a0), d0
	rol.w #8, d0
	}

asm inline int32 ZByteSwap_Read32(const volatile void* iValueAddress)
	{
	move.l (a0), d0
	rol.w #8, d0
	swap d0
	rol.w #8, d0
	}



asm inline void ZByteSwap_Write16(volatile void* ioValueAddress : __A0, int16 iValue : __D0)
	{
	rol.w #8, d0
	move.w d0, (a0)
	}

asm inline void ZByteSwap_Write32(volatile void* ioValueAddress : __A0, int32 iValue : __D0)
	{
	rol.w #8, d0
	swap d0
	rol.w #8, d0
	move.l d0, (a0)
	}
#endif // ZCONFIG(Compiler, CodeWarrior)
#endif // ZCONFIG(Processor, 68K)

// =================================================================================================
// MARK: - Processor is PPC

#if ZCONFIG(Processor, PPC)

#define ZByteSwap_EndianBig 1

#if ZCONFIG(Compiler, CodeWarrior)

	inline int16 ZByteSwap_Read16(register const volatile void* iValueAddress)
		{
		register int16 result;
		asm
			{
			lhbrx result, r0, iValueAddress
			}
		return result;
		}

	inline int32 ZByteSwap_Read32(register const volatile void* iValueAddress)
		{
		register int32 result;
		asm
			{
			lwbrx result, r0, iValueAddress
			}
		return result;
		}

	inline void ZByteSwap_Write16(register volatile void* oValueAddress, register int16 iValue)
		{
		asm
			{
			sthbrx iValue, r0, oValueAddress
			}
		}

	inline void ZByteSwap_Write32(register volatile void* oValueAddress, register int32 iValue)
		{
		asm
			{
			stwbrx iValue, r0, oValueAddress
			}
		}

#elif ZCONFIG(Compiler, GCC)

	inline int16 ZByteSwap_Read16(const volatile void* iValueAddress)
		{
		int16 result;
		__asm__
			(
			"lhbrx %0, 0, %1\n"
			: "=r" (result)
			: "r" (iValueAddress)
			: "memory"
			);
		return result;
		}

	inline int32 ZByteSwap_Read32(const volatile void* iValueAddress)
		{
		int32 result;
		__asm__
			(
			"lwbrx %0, 0, %1\n"
			: "=r" (result)
			: "r" (iValueAddress)
			: "memory"
			);
		return result;
		}

	inline void ZByteSwap_Write16(volatile void* oValueAddress, int16 iValue)
		{
		__asm__
			(
			"sthbrx %0, 0, %1\n"
			:
			: "r" (iValue), "r" (oValueAddress)
			: "memory"
			);
		}

	inline void ZByteSwap_Write32(volatile void* oValueAddress, int32 iValue)
		{
		__asm__
			(
			"stwbrx %0, 0, %1\n"
			:
			: "r" (iValue), "r" (oValueAddress)
			: "memory"
			);
		}

#endif // ZCONFIG(Compiler)

inline int16 ZByteSwap_16(int16 iValue)
	{ return ZByteSwap_Read16(&iValue); }

inline int32 ZByteSwap_32(int32 iValue)
	{ return ZByteSwap_Read32(&iValue); }


inline void ZByteSwap_16(volatile void* ioValueAddress)
	{ *static_cast<volatile int16*>(ioValueAddress) = ZByteSwap_Read16(ioValueAddress); }

inline void ZByteSwap_32(volatile void* ioValueAddress)
	{ *static_cast<volatile int32*>(ioValueAddress) = ZByteSwap_Read32(ioValueAddress); }

#endif // ZCONFIG(Processor, PPC)

// =================================================================================================
// MARK: - Processor is x86

#if ZCONFIG(Processor, x86)

#define ZByteSwap_EndianBig 0

#if ZCONFIG(Compiler, CodeWarrior)

	// Just as in ZAtomic I've found that CodeWarrior sucks rocks when it comes to reliably
	// including inline x86 assembly code with general C/C++ code, or indeed generating the
	// correct opcodes for parameters. To work around the problem the we have to do superfluous
	// register/register transfers, for example in this case moving the parameter iValueAddress
	// into edx, and then using [edx] instead of [iValueAddress]. The code bloat problem is not
	// so serious here as in ZAtomic, so I'm leaving these as inline routines, but we should
	// revisit this file when CW improves.

	inline int16 ZByteSwap_16(int16 iValue)
		{
		asm
			{
			mov ax, iValue
			xchg ah, al
			mov iValue, ax
			}
		return iValue;
		}

	inline int32 ZByteSwap_32(int32 iValue)
		{
		asm
			{
			mov eax, iValue
			bswap eax
			mov iValue, eax
			}
		return iValue;
		}



	inline void ZByteSwap_16(volatile void* ioValueAddress)
		{
		asm
			{
			mov edx, ioValueAddress
			mov ax, [edx]
			xchg ah, al
			mov [edx], ax
			}
		}

	inline void ZByteSwap_32(volatile void* ioValueAddress)
		{
		asm
			{
			mov edx, ioValueAddress
			mov eax, [edx]
			bswap eax
			mov [edx], eax
			}
		}



	inline int16 ZByteSwap_Read16(const volatile void* iValueAddress)
		{
		int16 result;
		asm
			{
			mov edx, iValueAddress
			mov ax, [edx]
			xchg ah, al
			mov result, ax
			}
		return result;
		}

	inline int32 ZByteSwap_Read32(const volatile void* iValueAddress)
		{
		int32 result;
		asm
			{
			mov edx, iValueAddress
			mov eax, [edx]
			bswap eax
			mov result, eax
			}
		return result;
		}



	inline void ZByteSwap_Write16(volatile void* oValueAddress, int16 iValue)
		{
		asm
			{
			mov ax, iValue
			xchg ah, al
			mov edx, oValueAddress
			mov [edx], ax
			}
		}

	inline void ZByteSwap_Write32(volatile void* oValueAddress, int32 iValue)
		{
		asm
			{
			mov eax, iValue
			bswap eax
			mov edx, oValueAddress
			mov [edx], eax
			}
		}

#else // ZCONFIG(Compiler, CodeWarrior)

	#if ZCONFIG(Compiler, GCC)

		inline int16 ZByteSwap_16(int16 iValue)
			{
			__asm__ ("xchgb %b0, %h0" : "+q" (iValue));
			return iValue;
			}

		inline int32 ZByteSwap_32(int32 iValue)
			{
		#if defined(__i486__) || defined(__i586__) || defined(__i686__)
			__asm__ ("bswap %0" : "+r" (iValue));
		#else
			__asm__
				(
				"xchgb %b0, %h0\n"
				"rorl $16, %0\n"
				"xchgb %b0, %h0"
				: "+q" (iValue)
				);
		#endif
			return iValue;
			}

	#elif ZCONFIG(Compiler, MSVC)

		inline int16 ZByteSwap_16(int16 iValue)
			{
			__asm
				{
				mov ax, iValue
				xchg ah, al
				mov iValue, ax
				}
			return iValue;
			}

		inline int32 ZByteSwap_32(int32 iValue)
			{
			__asm
				{
				mov eax, iValue
				bswap eax
				mov iValue, eax
				}
			return iValue;
			}

	#else // ZCONFIG(Compiler)

		// Dumb C version

		inline int16 ZByteSwap_16(int16 iValue)
			{
			uint16 theValue = iValue;
			return (theValue >> 8) | (theValue << 8);
			}

		inline int32 ZByteSwap_32(int32 iValue)
			{
			uint32 theValue = iValue;
			return
				(theValue >> 24)
				| ((theValue >> 8) & 0xFF00)
				| ((theValue << 8) & 0xFF0000)
				| (theValue << 24);
			}

	#endif // ZCONFIG(Compiler)

	inline int16 ZByteSwap_Read16(const volatile void* iValueAddress)
		{ return ZByteSwap_16(*static_cast<const volatile int16*>(iValueAddress)); }

	inline int32 ZByteSwap_Read32(const volatile void* iValueAddress)
		{ return ZByteSwap_32(*static_cast<const volatile int32*>(iValueAddress)); }

	inline void ZByteSwap_Write16(volatile void* oValueAddress, int16 iValue)
		{ *static_cast<volatile int16*>(oValueAddress) = ZByteSwap_16(iValue); }

	inline void ZByteSwap_Write32(volatile void* oValueAddress, int32 iValue)
		{ *static_cast<volatile int32*>(oValueAddress) = ZByteSwap_32(iValue); }

	inline void ZByteSwap_16(volatile void* ioValueAddress)
		{ *static_cast<volatile int16*>(ioValueAddress) = ZByteSwap_Read16(ioValueAddress); }

	inline void ZByteSwap_32(volatile void* ioValueAddress)
		{ *static_cast<volatile int32*>(ioValueAddress) = ZByteSwap_Read32(ioValueAddress); }

#endif // ZCONFIG(Compiler, CodeWarrior)

#endif // ZCONFIG(Processor, x86)

// =================================================================================================
// MARK: - Dumb C version

#if !defined(ZByteSwap_EndianBig)

#	if ZCONFIG(Endian, Big)
#		define ZByteSwap_EndianBig 1
#	else
#		define ZByteSwap_EndianBig 0
#	endif

	inline uint16 ZByteSwap_16(uint16 iValue)
		{ return (iValue >> 8) | (iValue << 8); }

	inline uint32 ZByteSwap_32(uint32 iValue)
		{ return ZByteSwap_16(iValue) << 16 | ZByteSwap_16(iValue >> 16); }

	inline void ZByteSwap_16(void* iValue)
		{ *static_cast<uint16*>(iValue) = ZByteSwap_16(*static_cast<uint16*>(iValue)); }

	inline void ZByteSwap_32(void* iValue)
		{ *static_cast<uint32*>(iValue) = ZByteSwap_32(*static_cast<uint32*>(iValue)); }

	inline uint16 ZByteSwap_Read16(const volatile void* iValueAddress)
		{ return ZByteSwap_16(*static_cast<const volatile uint16*>(iValueAddress)); }

	inline uint32 ZByteSwap_Read32(const volatile void* iValueAddress)
		{ return ZByteSwap_32(*static_cast<const volatile uint32*>(iValueAddress)); }

	inline void ZByteSwap_Write16(volatile void* oValueAddress, uint16 iValue)
		{ *static_cast<volatile uint16*>(oValueAddress) = ZByteSwap_16(iValue); }

	inline void ZByteSwap_Write32(volatile void* oValueAddress, uint32 iValue)
		{ *static_cast<volatile uint32*>(oValueAddress) = ZByteSwap_32(iValue); }

#endif // ZByteSwap_EndianBig

// =================================================================================================
// MARK: - 64 bit stuff

inline uint64 ZByteSwap_64(uint64 iValue)
	{
	union
		{
		uint64 fUInt64;
		uint32 fUInt32[2];
		} u;

	u.fUInt32[1] = ZByteSwap_32(uint32(iValue & 0xffffffff));
	u.fUInt32[0] = ZByteSwap_32(uint32(iValue >> 32));
	return u.fUInt64;
	}

inline void ZByteSwap_64(volatile void* ioValueAddress)
	{
	union u_t
		{
		uint64 fUInt64;
		uint32 fUInt32[2];
		};

	volatile u_t* theU = static_cast<volatile u_t*>(ioValueAddress);
	uint32 temp = theU->fUInt32[1];
	theU->fUInt32[1] = ZByteSwap_32(theU->fUInt32[0]);
	theU->fUInt32[0] = ZByteSwap_32(temp);
	}

inline int64 ZByteSwap_Read64(const volatile void* ioValueAddress)
	{
	union
		{
		uint64 fUInt64;
		uint32 fUInt32[2];
		} u;

	const volatile uint32* source = static_cast<const volatile uint32*>(ioValueAddress);
	u.fUInt32[1] = source[0];
	u.fUInt32[0] = source[1];
	u.fUInt32[1] = ZByteSwap_32(u.fUInt32[1]);
	u.fUInt32[0] = ZByteSwap_32(u.fUInt32[0]);
	return u.fUInt64;
	}

inline void ZByteSwap_Write64(volatile void* oValueAddress, int64 iValue)
	{ *static_cast<volatile int64*>(oValueAddress) = ZByteSwap_64(iValue); }

// =================================================================================================
// MARK: - The Real Calls


#if ZByteSwap_EndianBig

inline int16 ZByteSwap_BigToLittle16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_BigToLittle16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_BigToLittle16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_BigToLittle32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_BigToLittle32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_BigToLittle32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_BigToLittle64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_BigToLittle64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_BigToLittle64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_LittleToBig16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_LittleToBig16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_LittleToBig16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_LittleToBig32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_LittleToBig32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_LittleToBig32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_LittleToBig64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_LittleToBig64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_LittleToBig64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_HostToBig16(int16 iValue) { return iValue; }
inline void ZByteSwap_HostToBig16(int16* ioValueAddress) {}
inline void ZByteSwap_HostToBig16(uint16* ioValueAddress) {}

inline int32 ZByteSwap_HostToBig32(int32 iValue) { return iValue; }
inline void ZByteSwap_HostToBig32(int32* ioValueAddress) {}
inline void ZByteSwap_HostToBig32(uint32* ioValueAddress) {}

inline int64 ZByteSwap_HostToBig64(int64 iValue) { return iValue; }
inline void ZByteSwap_HostToBig64(int64* ioValueAddress) {}
inline void ZByteSwap_HostToBig64(uint64* ioValueAddress) {}



inline int16 ZByteSwap_HostToLittle16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_HostToLittle16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_HostToLittle16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_HostToLittle32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_HostToLittle32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_HostToLittle32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_HostToLittle64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_HostToLittle64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_HostToLittle64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_BigToHost16(int16 iValue) { return iValue; }
inline void ZByteSwap_BigToHost16(int16* ioValueAddress) {}
inline void ZByteSwap_BigToHost16(uint16* ioValueAddress) {}

inline int32 ZByteSwap_BigToHost32(int32 iValue) { return iValue; }
inline void ZByteSwap_BigToHost32(int32* ioValueAddress) {}
inline void ZByteSwap_BigToHost32(uint32* ioValueAddress) {}

inline int64 ZByteSwap_BigToHost64(int64 iValue) { return iValue; }
inline void ZByteSwap_BigToHost64(int64* ioValueAddress) {}
inline void ZByteSwap_BigToHost64(uint64* ioValueAddress) {}



inline int16 ZByteSwap_LittleToHost16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_LittleToHost16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_LittleToHost16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_LittleToHost32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_LittleToHost32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_LittleToHost32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_LittleToHost64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_LittleToHost64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_LittleToHost64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_ReadBig16(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int16*>(iValueAddress); }
inline int32 ZByteSwap_ReadBig32(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int32*>(iValueAddress); }
inline int64 ZByteSwap_ReadBig64(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int64*>(iValueAddress); }

inline int16 ZByteSwap_ReadLittle16(const volatile void* iValueAddress)
	{ return ZByteSwap_Read16(iValueAddress); }
inline int32 ZByteSwap_ReadLittle32(const volatile void* iValueAddress)
	{ return ZByteSwap_Read32(iValueAddress); }
inline int64 ZByteSwap_ReadLittle64(const volatile void* iValueAddress)
	{ return ZByteSwap_Read64(iValueAddress); }



inline void ZByteSwap_WriteBig16(volatile void* oValueAddress, int16 iValue)
	{ *static_cast<volatile int16*>(oValueAddress) = iValue; }
inline void ZByteSwap_WriteBig32(volatile void* oValueAddress, int32 iValue)
	{ *static_cast<volatile int32*>(oValueAddress) = iValue; }
inline void ZByteSwap_WriteBig64(volatile void* oValueAddress, int64 iValue)
	{ *static_cast<volatile int64*>(oValueAddress) = iValue; }

inline void ZByteSwap_WriteLittle16(volatile void* oValueAddress, int16 iValue)
	{ ZByteSwap_Write16(oValueAddress, iValue); }
inline void ZByteSwap_WriteLittle32(volatile void* oValueAddress, int32 iValue)
	{ ZByteSwap_Write32(oValueAddress, iValue); }
inline void ZByteSwap_WriteLittle64(volatile void* oValueAddress, int64 iValue)
	{ ZByteSwap_Write64(oValueAddress, iValue); }


#else // ZByteSwap_EndianBig

inline int16 ZByteSwap_BigToLittle16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_BigToLittle16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_BigToLittle16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_BigToLittle32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_BigToLittle32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_BigToLittle32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_BigToLittle64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_BigToLittle64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_BigToLittle64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_LittleToBig16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_LittleToBig16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_LittleToBig16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_LittleToBig32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_LittleToBig32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_LittleToBig32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_LittleToBig64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_LittleToBig64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_LittleToBig64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_HostToBig16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_HostToBig16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_HostToBig16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_HostToBig32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_HostToBig32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_HostToBig32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_HostToBig64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_HostToBig64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_HostToBig64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_HostToLittle16(int16 iValue) { return iValue; }
inline void ZByteSwap_HostToLittle16(int16* ioValueAddress) {}
inline void ZByteSwap_HostToLittle16(uint16* ioValueAddress) {}

inline int32 ZByteSwap_HostToLittle32(int32 iValue) { return iValue; }
inline void ZByteSwap_HostToLittle32(int32* ioValueAddress) {}
inline void ZByteSwap_HostToLittle32(uint32* ioValueAddress) {}

inline int64 ZByteSwap_HostToLittle64(int64 iValue) { return iValue; }
inline void ZByteSwap_HostToLittle64(int64* ioValueAddress) {}
inline void ZByteSwap_HostToLittle64(uint64* ioValueAddress) {}



inline int16 ZByteSwap_BigToHost16(int16 iValue) { return ZByteSwap_16(iValue); }
inline void ZByteSwap_BigToHost16(int16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }
inline void ZByteSwap_BigToHost16(uint16* ioValueAddress) { ZByteSwap_16(ioValueAddress); }

inline int32 ZByteSwap_BigToHost32(int32 iValue) { return ZByteSwap_32(iValue); }
inline void ZByteSwap_BigToHost32(int32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }
inline void ZByteSwap_BigToHost32(uint32* ioValueAddress) { ZByteSwap_32(ioValueAddress); }

inline int64 ZByteSwap_BigToHost64(int64 iValue) { return ZByteSwap_64(iValue); }
inline void ZByteSwap_BigToHost64(int64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }
inline void ZByteSwap_BigToHost64(uint64* ioValueAddress) { ZByteSwap_64(ioValueAddress); }



inline int16 ZByteSwap_LittleToHost16(int16 iValue) { return iValue; }
inline void ZByteSwap_LittleToHost16(int16* ioValueAddress) {}
inline void ZByteSwap_LittleToHost16(uint16* ioValueAddress) {}

inline int32 ZByteSwap_LittleToHost32(int32 iValue) { return iValue; }
inline void ZByteSwap_LittleToHost32(int32* ioValueAddress) {}
inline void ZByteSwap_LittleToHost32(uint32* ioValueAddress) {}

inline int64 ZByteSwap_LittleToHost64(int64 iValue) { return iValue; }
inline void ZByteSwap_LittleToHost64(int64* ioValueAddress) {}
inline void ZByteSwap_LittleToHost64(uint64* ioValueAddress) {}



inline int16 ZByteSwap_ReadBig16(const volatile void* iValueAddress)
	{ return ZByteSwap_Read16(iValueAddress); }
inline int32 ZByteSwap_ReadBig32(const volatile void* iValueAddress)
	{ return ZByteSwap_Read32(iValueAddress); }
inline int64 ZByteSwap_ReadBig64(const volatile void* iValueAddress)
	{ return ZByteSwap_Read64(iValueAddress); }

inline int16 ZByteSwap_ReadLittle16(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int16*>(iValueAddress); }
inline int32 ZByteSwap_ReadLittle32(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int32*>(iValueAddress); }
inline int64 ZByteSwap_ReadLittle64(const volatile void* iValueAddress)
	{ return *static_cast<const volatile int64*>(iValueAddress); }



inline void ZByteSwap_WriteBig16(volatile void* oValueAddress, int16 iValue)
	{ ZByteSwap_Write16(oValueAddress, iValue); }
inline void ZByteSwap_WriteBig32(volatile void* oValueAddress, int32 iValue)
	{ ZByteSwap_Write32(oValueAddress, iValue); }
inline void ZByteSwap_WriteBig64(volatile void* oValueAddress, int64 iValue)
	{ ZByteSwap_Write64(oValueAddress, iValue); }

inline void ZByteSwap_WriteLittle16(volatile void* oValueAddress, int16 iValue)
	{ *static_cast<volatile int16*>(oValueAddress) = iValue; }
inline void ZByteSwap_WriteLittle32(volatile void* oValueAddress, int32 iValue)
	{ *static_cast<volatile int32*>(oValueAddress) = iValue; }
inline void ZByteSwap_WriteLittle64(volatile void* oValueAddress, int64 iValue)
	{ *static_cast<volatile int64*>(oValueAddress) = iValue; }


#endif // ZByteSwap_EndianBig

#undef ZByteSwap_EndianBig

} // namespace ZooLib

#endif // __ZByteSwap_h__
