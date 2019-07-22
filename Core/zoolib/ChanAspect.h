// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanAspect_h__
#define __ZooLib_ChanAspect_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h"
#include "zoolib/Time.h" // For Time::kDay

#include "zoolib/ZStdInt.h" // For uint64
#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
#pragma mark - sClamped

inline size_t sClamped(uint64 iVal)
	{
	if (sizeof(size_t) < sizeof(uint64))
		return size_t(std::min(iVal, uint64(size_t(-1))));
	else
		return size_t(iVal);
	}

inline uint64 sSubtractSaturated(uint64 iLHS, uint64 iRHS)
	{ return (iLHS - iRHS) & -(iRHS <= iLHS); }

inline uint64 sClampedAvailable(uint64 iSize, uint64 iPosition)
	{ return sSubtractSaturated(iSize, iPosition); }

inline uint64 sClamped(uint64 iCount, uint64 iSize, uint64 iPosition)
	{ return std::min(iCount, sClampedAvailable(iSize, iPosition)); }

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
		return this->Read(buf, std::min<size_t>(sClamped(iCount), countof(buf)));
		}

	virtual size_t Readable()
		{ return 0; }
	};

template <class EE>
size_t sRead(const ChanAspect_Read<EE>& iAspect, EE* oDest, size_t iCount)
	{ return sNonConst(iAspect).Read(oDest, iCount); }

template <class EE>
uint64 sSkip(const ChanAspect_Read<EE>& iAspect, uint64 iCount)
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
	virtual size_t Unread(const EE* iSource, size_t iCount) = 0;
	};

template <class EE>
inline size_t sUnread(const ChanAspect_Unread<EE>& iAspect, const EE* iSource, size_t iCount)
	{ return sNonConst(iAspect).Unread(iSource, iCount); }

// =================================================================================================
#pragma mark - ChanAspect_WaitReadable

class ChanAspect_WaitReadable
	{
public:
	virtual bool WaitReadable(double iTimeout)
		{ return false; }

// Placeholder -- don't yet know what the Sig should be for the Callable.
//	virtual bool CallWhenReadable(const ZP<Callable>& iCallable)
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

#endif // __ZooLib_ChanAspect_h__
