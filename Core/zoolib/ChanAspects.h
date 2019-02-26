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
#pragma mark - ChanAspect_Abort

class ChanAspect_Abort
	{
public:
	virtual void Abort() = 0;
	};

inline void sAbort(const ChanAspect_Abort& iAspect)
	{ sNonConst(iAspect).Abort(); }

// =================================================================================================
#pragma mark - ChanAspect_DisconnectRead

class ChanAspect_DisconnectRead
	{
public:
	virtual bool DisconnectRead(double iTimeout) = 0;
	};

inline bool sDisconnectRead(const ChanAspect_DisconnectRead& iAspect, double iTimeout)
	{ return sNonConst(iAspect).DisconnectRead(iTimeout); }

inline void sDisconnectRead(const ChanAspect_DisconnectRead& iAspect)
	{
	while (not sDisconnectRead(iAspect, 1 * Time::kDay))
		{}
	}

// =================================================================================================
#pragma mark - ChanAspect_DisconnectWrite

class ChanAspect_DisconnectWrite
	{
public:
	virtual void DisconnectWrite() = 0;
	};

inline void sDisconnectWrite(const ChanAspect_DisconnectWrite& iAspect)
	{ sNonConst(iAspect).DisconnectWrite(); }

// =================================================================================================
#pragma mark - ChanAspect_Pos

class ChanAspect_Pos
	{
public:
	virtual uint64 Pos() = 0;

	virtual void PosSet(uint64 iPos) = 0;
	};

inline uint64 sPos(const ChanAspect_Pos& iAspect)
	{ return sNonConst(iAspect).Pos(); }

inline void sPosSet(const ChanAspect_Pos& iAspect, uint64 iPos)
	{ sNonConst(iAspect).PosSet(iPos); }

// =================================================================================================
#pragma mark - ChanAspect_Read

template <class EE>
class ChanAspect_Read
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
size_t sRead(const ChanAspect_Read<EE>& iAspect, EE* oDest, size_t iCount)
	{ return sNonConst(iAspect).Read(oDest, iCount); }

template <class EE>
size_t sSkip(const ChanAspect_Read<EE>& iAspect, size_t iCount)
	{ return sNonConst(iAspect).Skip(iCount); }

template <class EE>
size_t sReadable(const ChanAspect_Read<EE>& iAspect)
	{ return sNonConst(iAspect).Readable(); }

// =================================================================================================
#pragma mark - ChanAspect_Size

template <class LL, class EE>
class ChanAspect_ReadAt
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t ReadAt(const LL& iLoc, EE* oDest, size_t iCount) = 0;
	};

template <class LL, class EE>
size_t sReadAt(const ChanAspect_ReadAt<LL,EE>& iAspect, const LL& iLoc, EE* oDest, size_t iCount)
	{ return sNonConst(iAspect).ReadAt(iLoc, oDest, iCount); }

// =================================================================================================
#pragma mark - ChanAspect_Size

class ChanAspect_Size
	{
public:
	virtual uint64 Size() = 0;
	};

inline uint64 sSize(const ChanAspect_Size& iAspect)
	{ return sNonConst(iAspect).Size(); }

// =================================================================================================
#pragma mark - ChanAspect_SizeSet

class ChanAspect_SizeSet
	{
public:
	virtual void SizeSet(uint64 iSize) = 0;
	};

inline void sSizeSet(const ChanAspect_SizeSet& iAspect, uint64 iSize)
	{ sNonConst(iAspect).SizeSet(iSize); }

// =================================================================================================
#pragma mark - ChanAspect_Unread

template <class EE>
class ChanAspect_Unread
:	public virtual UserOfElement<EE>
	{
public:
	virtual void Unread(const EE* iSource, size_t iCount) = 0;

//	virtual size_t UnreadableLimit()
//		{ return 0; }
	};

template <class EE>
inline void sUnread(const ChanAspect_Unread<EE>& iAspect, const EE* iSource, size_t iCount)
	{ sNonConst(iAspect).Unread(iSource, iCount); }

//template <class EE>
//inline size_t sUnreadableLimit(const ChanAspect_Unread<EE>& iAspect)
//	{ return sNonConst(iAspect).UnreadableLimit(); }

// =================================================================================================
#pragma mark - ChanAspect_WaitReadable

class ChanAspect_WaitReadable
	{
public:
	virtual bool WaitReadable(double iTimeout)
		{ return false; }

// Placeholder -- don't yet know what the Sig should be for the Callable.
//	virtual bool CallWhenReadable(const ZRef<Callable>& iCallable)
//		{ return false; }
	};

inline bool sWaitReadable(const ChanAspect_WaitReadable& iAspect, double iTimeout)
	{ return sNonConst(iAspect).WaitReadable(iTimeout); }

template <class ChanAspect_p,
	bool enabled=std::is_base_of<ChanAspect_WaitReadable,ChanAspect_p>::value>
struct WaitReadableIf
	{ static bool sCall(const ChanAspect_p& iAspect, double iTimeout) { return false; } };

template <class ChanAspect_p>
struct WaitReadableIf<ChanAspect_p,true>
	{
	static bool sCall(const ChanAspect_p& iAspect, double iTimeout)
		{ return sNonConst(iAspect).WaitReadable(iTimeout); }
	};

// =================================================================================================
#pragma mark - ChanAspect_Write

template <class EE>
class ChanAspect_Write
:	public virtual UserOfElement<EE>
	{
public:
	virtual size_t Write(const EE* iSource, size_t iCount) = 0;

	virtual void Flush()
		{}
	};

template <class EE>
inline size_t sWrite(const ChanAspect_Write<EE>& iAspect, const EE* iSource, size_t iCount)
	{ return sNonConst(iAspect).Write(iSource, iCount); }

template <class EE>
inline void sFlush(const ChanAspect_Write<EE>& iAspect)
	{ sNonConst(iAspect).Flush(); }

} // namespace ZooLib

#endif // __ZooLib_ChanAspects_h__
