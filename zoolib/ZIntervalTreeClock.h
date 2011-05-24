/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZIntervalTreeClock__
#define __ZIntervalTreeClock__ 1
#include "zconfig.h"

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZRef.h"

namespace ZooLib {
namespace ZIntervalTreeClock {

// =================================================================================================
#pragma mark -
#pragma mark * Identity

class Identity
:	public ZCountedWithoutFinalize
	{
public:
	static ZRef<Identity> sZero();
	static ZRef<Identity> sOne();

	Identity();
	virtual ~Identity();

	Identity(const ZRef<Identity>& iLeft, const ZRef<Identity>& iRight);

	bool IsLeaf() const;
	bool IsOne() const;
	bool IsZero() const;

	bool IsInternal() const;
	ZRef<Identity> Left() const;
	ZRef<Identity> Right() const;

	void Split(ZRef<Identity>& oLeft, ZRef<Identity>& oRight) const;

	ZRef<Identity> Summed(const ZRef<Identity>& iOther) const;

private:
	const ZRef<Identity> fLeft;
	const ZRef<Identity> fRight;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Event

class Event
:	public ZCountedWithoutFinalize
	{
public:
	static ZRef<Event> sZero();

	Event();
	virtual ~Event();

	Event(size_t iValue);
	Event(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight);
	Event(bool iWithZeroChildren, size_t iValue);

// Accessors, mainly for text and binary streaming.
	size_t Value() const;

	bool IsLeaf() const;

	bool IsInternal() const;
	ZRef<Event> Left() const;
	ZRef<Event> Right() const;

// Comparison
	bool Equals(const ZRef<Event>& iOther) const;
	bool LessEqual(const ZRef<Event>& iOther) const;

	bool IsBefore(const ZRef<Event>& iOther) const;
	bool IsAfter(const ZRef<Event>& iOther) const;
	bool IsConcurrent(const ZRef<Event>& iOther) const;
	bool IsSame(const ZRef<Event>& iOther) const;

// Fundamental operations
	ZRef<Event> Evented(const ZRef<Identity>& iIdentity) const;
	ZRef<Event> Joined(const ZRef<Event>& iOther) const;

private:
	size_t pGrown(const ZRef<Identity>& iIdentity, ZRef<Event>& oEvent) const;

	ZRef<Event> pFilled(const ZRef<Identity>& iIdentity) const;
	ZRef<Event> pDropped(size_t d) const;
	ZRef<Event> pLifted(size_t d) const;
	ZRef<Event> pNormalized() const;
	size_t pHeight() const;

	const ZRef<Event> fLeft;
	const ZRef<Event> fRight;
	const size_t fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Event mutating operations

void sEvent(ZRef<Event>& ioEvent, const ZRef<Identity>& iIdentity);
void sJoin(ZRef<Event>& ioEvent, const ZRef<Event>& iOther);

// =================================================================================================
#pragma mark -
#pragma mark * Clock

class Clock
:	public ZCountedWithoutFinalize
	{
public:
	static ZRef<Clock> sSeed();

	Clock(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent);
	Clock(const ZRef<Clock>& iClock, const ZRef<Event>& iEvent);
	Clock(const ZRef<Identity>& iIdentity, const ZRef<Clock>& iClock);
	virtual ~Clock();

// Accessors, mainly for text and binary streaming.
	ZRef<Identity> GetIdentity() const;
	ZRef<Event> GetEvent() const;

// Comparison
	bool LessEqual(const ZRef<Clock>& iOther) const;

	bool IsBefore(const ZRef<Clock>& iOther) const;
	bool IsAfter(const ZRef<Clock>& iOther) const;
	bool IsConcurrent(const ZRef<Clock>& iOther) const;
	bool IsSame(const ZRef<Clock>& iOther) const;

// Higher level operations
	ZRef<Clock> Sent() const;
	ZRef<Clock> Received(const ZRef<Event>& iEvent) const;

// Fundamental operations
	ZRef<Clock> Evented() const;
	ZRef<Clock> Joined(const ZRef<Clock>& iOther) const;
	void Forked(ZRef<Clock>& oLeft, ZRef<Clock>& oRight) const;
	
private:
	const ZRef<Identity> fIdentity;
	const ZRef<Event> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Clock mutating operations

void sSend(ZRef<Clock>& ioClock);
void sReceive(ZRef<Clock>& ioClock, const ZRef<Event>& iEventReceived);

void sEvent(ZRef<Clock>& ioClock);
void sJoin(ZRef<Clock>& ioClock, const ZRef<Clock>& iOther);
ZRef<Clock> sFork(ZRef<Clock>& ioClock);

} // namespace ZIntervalTreeClock
} // namespace ZooLib

#endif // __ZIntervalTreeClock__
