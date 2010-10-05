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

	bool IsLeaf();
	bool IsOne();
	bool IsZero();

	bool IsInternal();
	ZRef<Identity> Left();
	ZRef<Identity> Right();

	void Split(ZRef<Identity>& oLeft, ZRef<Identity>& oRight);

	ZRef<Identity> Summed(const ZRef<Identity>& iOther);

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
	size_t Value();

	bool IsLeaf();

	bool IsInternal();
	ZRef<Event> Left();
	ZRef<Event> Right();

// Comparison
	bool Equals(const ZRef<Event>& iOther);
	bool LessEqual(const ZRef<Event>& iOther);

	bool IsBefore(const ZRef<Event>& iOther);
	bool IsAfter(const ZRef<Event>& iOther);
	bool IsConcurrent(const ZRef<Event>& iOther);
	bool IsSame(const ZRef<Event>& iOther);

// Fundamental operations
	ZRef<Event> Evented(const ZRef<Identity>& iIdentity);
	ZRef<Event> Joined(const ZRef<Event>& iOther);

private:
	size_t pGrown(const ZRef<Identity>& iIdentity, ZRef<Event>& oEvent);

	ZRef<Event> pFilled(const ZRef<Identity>& iIdentity);
	ZRef<Event> pDropped(size_t d);
	ZRef<Event> pLifted(size_t d);
	ZRef<Event> pNormalized();
	size_t pHeight();

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
#pragma mark * Stamp

class Stamp
:	public ZCountedWithoutFinalize
	{
public:
	static ZRef<Stamp> sSeed();

	Stamp(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent);
	Stamp(const ZRef<Stamp>& iStamp, const ZRef<Event>& iEvent);
	Stamp(const ZRef<Identity>& iIdentity, const ZRef<Stamp>& iStamp);
	virtual ~Stamp();

// Accessors, mainly for text and binary streaming.
	ZRef<Identity> GetIdentity();
	ZRef<Event> GetEvent();

// Comparison
	bool LessEqual(const ZRef<Stamp>& iOther);

	bool IsBefore(const ZRef<Stamp>& iOther);
	bool IsAfter(const ZRef<Stamp>& iOther);
	bool IsConcurrent(const ZRef<Stamp>& iOther);
	bool IsSame(const ZRef<Stamp>& iOther);

// Higher level operations
	ZRef<Stamp> Sent();
	ZRef<Stamp> Received(const ZRef<Event>& iEvent);

// Fundamental operations
	ZRef<Stamp> Evented();
	ZRef<Stamp> Joined(const ZRef<Stamp>& iOther);
	void Forked(ZRef<Stamp>& oLeft, ZRef<Stamp>& oRight);
	
private:
	const ZRef<Identity> fIdentity;
	const ZRef<Event> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Stamp mutating operations

void sSend(ZRef<Stamp>& ioStamp);
void sReceive(ZRef<Stamp>& ioStamp, const ZRef<Event>& iEventReceived);

void sEvent(ZRef<Stamp>& ioStamp);
void sJoin(ZRef<Stamp>& ioStamp, const ZRef<Stamp>& iOther);
ZRef<Stamp> sFork(ZRef<Stamp>& ioStamp);

} // namespace ZIntervalTreeClock
} // namespace ZooLib

#endif // __ZIntervalTreeClock__
