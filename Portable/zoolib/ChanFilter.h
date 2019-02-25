/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#ifndef __ZooLib_ChanFilter_h__
#define __ZooLib_ChanFilter_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

namespace ZooLib {

namespace ChanFiltration {

// =================================================================================================
#pragma mark - AspectCallForwarder

template <class ChanAspect_p, class Chan_p, class DerivedFrom_p> class AspectCallForwarder;

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Abort

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_Abort,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual void Abort()
		{ return sAbort(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_DisconnectRead

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_DisconnectRead,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual bool DisconnectRead(double iTimeout)
		{ return sDisconnectRead(DerivedFrom_p::pGetChan(), iTimeout); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_DisconnectWrite

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_DisconnectWrite,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual bool DisconnectWrite()
		{ return sDisconnectWrite(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Pos

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_Pos,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual uint64 Pos()
		{ return sPos(DerivedFrom_p::pGetChan()); }

	virtual void PosSet(uint64 iPos)
		{ sPosSet(DerivedFrom_p::pGetChan(), iPos); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Read

template <class Chan_p, class DerivedFrom_p, class EE>
class AspectCallForwarder<ChanAspect_Read<EE>,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual size_t Read(EE* oDest, size_t iCount)
		{ return sRead(DerivedFrom_p::pGetChan(), oDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return sSkip(DerivedFrom_p::pGetChan(), iCount); }

	virtual size_t Readable()
		{ return sReadable(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_ReadAt

template <class Chan_p, class DerivedFrom_p, class LL, class EE>
class AspectCallForwarder<ChanAspect_ReadAt<LL,EE>,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual size_t ReadAt(const LL& iLoc, EE* oDest, size_t iCount)
		{ return sReadAt(iLoc, oDest, iCount); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Size

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_Size,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual uint64 Size()
		{ return sSize(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_SizeSet

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_SizeSet,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual void SizeSet(uint64 iSize)
		{ return sSizeSet(DerivedFrom_p::pGetChan(), iSize); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Unread

template <class Chan_p, class DerivedFrom_p, class EE>
class AspectCallForwarder<ChanAspect_Unread<EE>,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual size_t Unread(const EE* iSource, size_t iCount)
		{ return sUnread(DerivedFrom_p::pGetChan(), iSource, iCount); }

	virtual size_t UnreadableLimit()
		{ return sUnreadableLimit(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_WaitReadable

template <class Chan_p, class DerivedFrom_p>
class AspectCallForwarder<ChanAspect_WaitReadable,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual bool WaitReadable(double iTimeout)
		{ return sWaitReadable(DerivedFrom_p::pGetChan(), iTimeout); }
	};

// =================================================================================================
#pragma mark - AspectCallForwarder, ChanAspect_Write

template <class Chan_p, class DerivedFrom_p, class EE>
class AspectCallForwarder<ChanAspect_Write<EE>,Chan_p,DerivedFrom_p>
:	public DerivedFrom_p
	{
public:
	AspectCallForwarder(const Chan_p& iChan) : DerivedFrom_p(iChan) {}

	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return sWrite(DerivedFrom_p::pGetChan(), iSource, iCount); }

	virtual void Flush()
		{ sFlush(DerivedFrom_p::pGetChan()); }
	};

// =================================================================================================
#pragma mark - ChanRefHolder

template <class Chan_p>
class ChanRefHolder
:	public Chan_p
	{
public:
	ChanRefHolder(const Chan_p& iChan) : fChan(iChan) {}

protected:
	const Chan_p& pGetChan() const { return fChan; }

	const Chan_p& fChan;
	};

// =================================================================================================
#pragma mark - AspectSplitter

template <class Chan_p, class DerivedFrom_p> class AspectSplitter;

template <class Chan_p, class P0>
class AspectSplitter<Chan_p,DeriveFrom<P0>>
:	public AspectCallForwarder<P0,Chan_p,ChanRefHolder<Chan_p>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,ChanRefHolder<Chan_p>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2, class P3>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2,P3>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2, class P3, class P4>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2,P3,P4>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2, class P3, class P4, class P5>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2,P3,P4,P5>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2,P3,P4,P5,P6>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5,P6>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5,P6>>>(iChan)
		{}
	};

template <class Chan_p,
	class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class AspectSplitter<Chan_p,DeriveFrom<P0,P1,P2,P3,P4,P5,P6,P7>>
:	public AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5,P6,P7>>>
	{
public:
	AspectSplitter(const Chan_p& iChan)
	:	AspectCallForwarder<P0,Chan_p,AspectSplitter<Chan_p,DeriveFrom<P1,P2,P3,P4,P5,P6,P7>>>(iChan)
		{}
	};

} // namespace ChanFiltration

// =================================================================================================
#pragma mark - ChanFilter

template <class DerivedFrom_p>
class ChanFilter
:	public ChanFiltration::AspectSplitter<DerivedFrom_p,DerivedFrom_p>
	{
public:
	ChanFilter(const DerivedFrom_p& iChan)
	:	ChanFiltration::AspectSplitter<DerivedFrom_p,DerivedFrom_p>(iChan)
		{}
	};

} // namespace ZooLib

#endif // __ZooLib_ChanFilter_h__
