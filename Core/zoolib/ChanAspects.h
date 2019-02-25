/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_ChanAspects_h__
#define __ZooLib_ChanAspects_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h"
#include "zoolib/DeriveFrom.h"
#include "zoolib/Time.h" // For Time::kDay

#include "zoolib/ZStdInt.h" // For uint64
#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
#pragma mark - UserOfElement

template <class EE>
class UserOfElement
	{
public:
	typedef EE Element_t;
	};

// =================================================================================================
#pragma mark - Aspect_Abort

class Aspect_Abort
	{
public:
	virtual void Abort() = 0;
	};

inline void sAbort(const Aspect_Abort& iAspect)
	{ sNonConst(iAspect).Abort(); }

// =================================================================================================
#pragma mark - Aspect_DisconnectRead

class Aspect_DisconnectRead
	{
public:
	virtual bool DisconnectRead(double iTimeout) = 0;
	};

inline bool sDisconnectRead(const Aspect_DisconnectRead& iAspect, double iTimeout)
	{ return sNonConst(iAspect).DisconnectRead(iTimeout); }

inline void sDisconnectRead(const Aspect_DisconnectRead& iAspect)
	{
	while (not sDisconnectRead(iAspect, 1 * Time::kDay))
		{}
	}

// =================================================================================================
#pragma mark - Aspect_DisconnectWrite

class Aspect_DisconnectWrite
	{
public:
	virtual void DisconnectWrite() = 0;
	};

inline void sDisconnectWrite(const Aspect_DisconnectWrite& iAspect)
	{ sNonConst(iAspect).DisconnectWrite(); }

// =================================================================================================
#pragma mark - Aspect_Pos

class Aspect_Pos
	{
public:
	virtual uint64 Pos() = 0;

	virtual void PosSet(uint64 iPos) = 0;
	};

inline uint64 sPos(const Aspect_Pos& iAspect)
	{ return sNonConst(iAspect).Pos(); }

inline void sPosSet(const Aspect_Pos& iAspect, uint64 iPos)
	{ sNonConst(iAspect).PosSet(iPos); }

// =================================================================================================
#pragma mark - Aspect_Read

template <class EE>
class Aspect_Read
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t Read(EE* oDest, size_t iCount) = 0;

	virtual uint64 Skip(uint64 iCount)
		{
		// buf will have space for at least one element.
		EE buf[(sStackBufferSize + sizeof(EE) - 1) / sizeof(EE)];
		return this->Read(buf, std::min<size_t>(iCount, countof(buf)));
		}

	virtual size_t Readable()
		{ return 0; }
	};

template <class EE>
inline size_t sRead(const Aspect_Read<EE>& iAspect, EE* oDest, size_t iCount)
	{ return sNonConst(iAspect).Read(oDest, iCount); }

template <class EE>
inline size_t sSkip(const Aspect_Read<EE>& iAspect, size_t iCount)
	{ return sNonConst(iAspect).Skip(iCount); }

template <class EE>
inline size_t sReadable(const Aspect_Read<EE>& iAspect)
	{ return sNonConst(iAspect).Readable(); }

// =================================================================================================
#pragma mark - Aspect_Size

template <class LL, class EE>
class Aspect_ReadAt
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t ReadAt(const LL& iLoc, EE* oDest, size_t iCount) = 0;
	};

template <class LL, class EE>
inline size_t sReadAt(const Aspect_ReadAt<LL,EE>& iAspect, const LL& iLoc, EE* oDest, size_t iCount)
	{ return sNonConst(iAspect).ReadAt(iLoc, oDest, iCount); }

// =================================================================================================
#pragma mark - Aspect_Size

class Aspect_Size
	{
public:
	virtual uint64 Size() = 0;
	};

inline uint64 sSize(const Aspect_Size& iAspect)
	{ return sNonConst(iAspect).Size(); }

// =================================================================================================
#pragma mark - Aspect_SizeSet

class Aspect_SizeSet
	{
public:
	virtual void SizeSet(uint64 iSize) = 0;
	};

inline void sSizeSet(const Aspect_SizeSet& iAspect, uint64 iSize)
	{ sNonConst(iAspect).SizeSet(iSize); }

// =================================================================================================
#pragma mark - Aspect_Unread

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
inline size_t sUnread(const Aspect_Unread<EE>& iAspect, const EE* iSource, size_t iCount)
	{ return sNonConst(iAspect).Unread(iSource, iCount); }

template <class EE>
inline size_t sUnreadableLimit(const Aspect_Unread<EE>& iAspect)
	{ return sNonConst(iAspect).UnreadableLimit(); }

// =================================================================================================
#pragma mark - Aspect_WaitReadable

class Aspect_WaitReadable
	{
public:
	virtual bool WaitReadable(double iTimeout)
		{ return false; }

// Placeholder -- don't yet know what the Sig should be for the Callable.
//	virtual bool CallWhenReadable(const ZRef<Callable>& iCallable)
//		{ return false; }
	};

inline bool sWaitReadable(const Aspect_WaitReadable& iAspect, double iTimeout)
	{ return sNonConst(iAspect).WaitReadable(iTimeout); }

template <class Aspect_p, bool enabled=std::is_base_of<Aspect_WaitReadable,Aspect_p>::value>
struct WaitReadableIf
	{ static bool sCall(const Aspect_p& iAspect, double iTimeout) { return false; } };

template <class Aspect_p>
struct WaitReadableIf<Aspect_p,true>
	{
	static bool sCall(const Aspect_p& iAspect, double iTimeout)
		{ return sNonConst(iAspect).WaitReadable(iTimeout); }
	};

// =================================================================================================
#pragma mark - Aspect_Write

template <class EE>
class Aspect_Write
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t Write(const EE* iSource, size_t iCount) = 0;

	virtual void Flush()
		{}
	};

template <class EE>
inline size_t sWrite(const Aspect_Write<EE>& iAspect, const EE* iSource, size_t iCount)
	{ return sNonConst(iAspect).Write(iSource, iCount); }

template <class EE>
inline void sFlush(const Aspect_Write<EE>& iAspect)
	{ sNonConst(iAspect).Flush(); }

} // namespace ZooLib

#endif // __ZooLib_ChanAspects_h__
