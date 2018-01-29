/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Chan_h__
#define __ZooLib_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h"
#include "zoolib/DeriveFrom.h"

#include "zoolib/ZStdInt.h" // For uint64
#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark UserOfElement

template <class EE>
class UserOfElement
	{
public:
	typedef EE Element_t;
	};

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Abort

class Aspect_Abort
	{
public:
	virtual void Abort() = 0;
	};

using ChanAbort = DeriveFrom<Aspect_Abort>;

inline void sAbort(const ChanAbort& iChan)
	{ sNonConst(iChan).Abort(); }

// =================================================================================================
#pragma mark -
#pragma mark AspectDisconnectRead

class Aspect_DisconnectRead
	{
public:
	virtual bool DisconnectRead(double iTimeout) = 0;
	};

using ChanDisconnectRead = DeriveFrom<Aspect_DisconnectRead>;

inline bool sDisconnectRead(const ChanDisconnectRead& iChan, double iTimeout)
	{ return sNonConst(iChan).DisconnectRead(iTimeout); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_DisconnectWrite

class Aspect_DisconnectWrite
	{
public:
	virtual void DisconnectWrite() = 0;
	};

using ChanDisconnectWrite = DeriveFrom<Aspect_DisconnectWrite>;

inline void sDisconnectWrite(const ChanDisconnectWrite& iChan)
	{ sNonConst(iChan).DisconnectWrite(); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Pos

class Aspect_Pos
	{
public:
	virtual uint64 Pos() = 0;

	virtual void SetPos(uint64 iPos) = 0;
	};

using ChanPos = DeriveFrom<Aspect_Pos>;

inline uint64 sPos(const ChanPos& iChan)
	{ return sNonConst(iChan).Pos(); }

inline void sPosSet(const ChanPos& iChan, uint64 iPos)
	{ sNonConst(iChan).SetPos(iPos); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Read

template <class EE>
class Aspect_Read
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t QRead(EE* oDest, size_t iCount) = 0;

	virtual uint64 Skip(uint64 iCount)
		{
		// buf will have space for at least one element.
		EE buf[(sStackBufferSize + sizeof(EE) - 1) / sizeof(EE)];
		return this->QRead(buf, std::min<size_t>(iCount, countof(buf)));
		}

	virtual size_t Readable()
		{ return 0; }
	};

template <class EE>
using ChanR = DeriveFrom<Aspect_Read<EE>>;

template <class EE>
inline size_t sRead(const ChanR<EE>& iChan, EE* oDest, size_t iCount)
	{ return sNonConst(iChan).QRead(oDest, iCount); }

template <class EE> //##
inline size_t sQRead(const ChanR<EE>& iChan, EE* oDest, size_t iCount)
	{ return sNonConst(iChan).QRead(oDest, iCount); }

template <class EE>
inline size_t sSkip(const ChanR<EE>& iChan, size_t iCount)
	{ return sNonConst(iChan).Skip(iCount); }

template <class EE>
inline size_t sReadable(const ChanR<EE>& iChan)
	{ return sNonConst(iChan).Readable(); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Size

template <class LL, class EE>
class Aspect_ReadAt
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t QReadAt(const LL& iLoc, EE* oDest, size_t iCount) = 0;
	};

template <class LL, class EE>
using ChanReadAt = DeriveFrom<Aspect_ReadAt<LL,EE>>;

template <class LL, class EE>
inline size_t sReadAt(const ChanReadAt<LL,EE>& iChan, const LL& iLoc, EE* oDest, size_t iCount)
	{ return sNonConst(iChan).QReadAt(iLoc, oDest, iCount); }

template <class LL, class EE> //##
inline size_t sQReadAt(const ChanReadAt<LL,EE>& iChan, const LL& iLoc, EE* oDest, size_t iCount)
	{ return sNonConst(iChan).QReadAt(iLoc, oDest, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Size

class Aspect_Size
	{
public:
	virtual uint64 Size() = 0;
	};

using ChanSize = DeriveFrom<Aspect_Size>;

inline uint64 sSize(const ChanSize& iChan)
	{ return sNonConst(iChan).Size(); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_SizeSet

class Aspect_SizeSet
	{
public:
	virtual void SizeSet(uint64 iSize) = 0;
	};

using ChanSizeSet = DeriveFrom<Aspect_SizeSet>;

inline void sSizeSet(const ChanSizeSet& iChan, uint64 iSize)
	{ sNonConst(iChan).SizeSet(iSize); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Unread

template <class EE>
class Aspect_Unread
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t Unread(const EE* iSource, size_t iCount) = 0;

	virtual size_t UnreadableLimit()
		{ return 0; }
	};

template <class EE>
using ChanU = DeriveFrom<Aspect_Unread<EE>>;

template <class EE>
inline size_t sUnread(const ChanU<EE>& iChan, const EE* iSource, size_t iCount)
	{ return sNonConst(iChan).Unread(iSource, iCount); }

template <class EE>
inline size_t sUnreadableLimit(const ChanU<EE>& iChan)
	{ return sNonConst(iChan).UnreadableLimit(); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_WaitReadable

class Aspect_WaitReadable
	{
public:
	virtual bool WaitReadable(double iTimeout)
		{ return false; }

// Placeholder -- don't yet know what the Sig should be for the Callable.
//	virtual bool CallWhenReadable(const ZRef<Callable>& iCallable)
//		{ return false; }
	};

using ChanWaitReadable = DeriveFrom<Aspect_WaitReadable>;

inline bool sWaitReadable(const ChanWaitReadable& iChan, double iTimeout)
	{ return sNonConst(iChan).WaitReadable(iTimeout); }

// =================================================================================================
#pragma mark -
#pragma mark Aspect_Write

template <class EE>
class Aspect_Write
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t QWrite(const EE* iSource, size_t iCount) = 0;

	virtual void Flush()
		{}
	};

template <class EE>
using ChanW = DeriveFrom<Aspect_Write<EE>>;

template <class EE>
inline size_t sWrite(const ChanW<EE>& iChan, const EE* iSource, size_t iCount)
	{ return sNonConst(iChan).QWrite(iSource, iCount); }

template <class EE> //##
inline size_t sQWrite(const ChanW<EE>& iChan, const EE* iSource, size_t iCount)
	{ return sNonConst(iChan).QWrite(iSource, iCount); }

template <class EE>
inline void sFlush(const ChanW<EE>& iChan)
	{ sNonConst(iChan).Flush(); }

// =================================================================================================
#pragma mark -
#pragma mark Common composites.

template <typename EE> using ChanRU = DeriveFrom
	<
	Aspect_Read<EE>,
	Aspect_Unread<EE>
	>;

template <typename EE> using ChanRPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Read<EE>,
	Aspect_Size,
	Aspect_Unread<EE>
	>;

template <typename EE> using ChanWPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Size,
	Aspect_SizeSet,
	Aspect_Write<EE>
	>;

template <typename EE> using ChanRWPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Read<EE>,
	Aspect_Size,
	Aspect_SizeSet,
	Aspect_Unread<EE>,
	Aspect_Write<EE>
	>;

template <typename EE> using ChanRW = DeriveFrom
	<
	Aspect_Read<EE>,
	Aspect_WaitReadable,
	Aspect_Write<EE>
	>;

template <typename EE> using ChanRCon = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectRead,
	Aspect_Read<EE>,
	Aspect_WaitReadable
	>;

template <typename EE> using ChanWCon = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectWrite,
	Aspect_Write<EE>
	>;

template <typename EE> using ChanConnection = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectRead,
	Aspect_DisconnectWrite,
	Aspect_Read<EE>,
	Aspect_WaitReadable,
	Aspect_Write<EE>
	>;

} // namespace ZooLib

#endif // __ZooLib_Chan_h__
