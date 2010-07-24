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

#include "zoolib/ZRef_Counted.h"

namespace ZooLib {
namespace ZIntervalTreeClock { 

// =================================================================================================
#pragma mark -
#pragma mark * Identity

class Identity
:	public ZCountedWithoutFinalize
	{
private:
	Identity();

public:
	static ZRef<Identity> sZero();
	static ZRef<Identity> sOne();

	Identity(const ZRef<Identity>& iLeft, const ZRef<Identity>& iRight);

	virtual ~Identity();

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

	static const ZRef<Identity> spZero;
	static const ZRef<Identity> spOne;

	static const ZRef<Identity> spZeroOne;
	static const ZRef<Identity> spOneZero;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Event

class Event
:	public ZCountedWithoutFinalize
	{
private:
	Event(); // Not implemented

public:
	static ZRef<Event> sZero();

	Event(size_t iValue);
	Event(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight);
	Event(bool iWithZeroChildren, size_t iValue);

	virtual ~Event();

	size_t Value();

	bool IsLeaf();

	bool IsInternal();
	ZRef<Event> Left();
	ZRef<Event> Right();

	bool Equals(const ZRef<Event>& iOther);
	bool LessEqual(const ZRef<Event>& iOther);

	bool IsBefore(const ZRef<Event>& iOther);
	bool IsAfter(const ZRef<Event>& iOther);
	bool IsConcurrent(const ZRef<Event>& iOther);
	bool IsSame(const ZRef<Event>& iOther);

	ZRef<Event> Joined(const ZRef<Event>& iOther);
	ZRef<Event> Filled(const ZRef<Identity>& iIdentity);
	ZRef<Event> Grown(ZRef<Identity> iIdentity);

private:
	size_t pGrown(ZRef<Identity> iIdentity, ZRef<Event>& oEvent);
	ZRef<Event> pDropped(size_t d);
	ZRef<Event> pLifted(size_t d);
	ZRef<Event> pNormalized();
	size_t pHeight();

	const ZRef<Event> fLeft;
	const ZRef<Event> fRight;
	const size_t fValue;

	static const ZRef<Event> spZero;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Stamp

class Stamp
:	public ZCountedWithoutFinalize
	{
public:
	static ZRef<Stamp> sSeed();
	Stamp(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent);

	ZRef<Stamp> Evented();
	void Forked(ZRef<Stamp>& oLeft, ZRef<Stamp>& oRight);
	ZRef<Stamp> Joined(const ZRef<Stamp>& iOther);

	ZRef<Identity> GetIdentity();
	ZRef<Event> GetEvent();
	
	bool LessEqual(const ZRef<Stamp>& iOther);

	bool IsBefore(const ZRef<Stamp>& iOther);
	bool IsAfter(const ZRef<Stamp>& iOther);
	bool IsConcurrent(const ZRef<Stamp>& iOther);
	bool IsSame(const ZRef<Stamp>& iOther);

private:
	const ZRef<Identity> fIdentity;
	const ZRef<Event> fEvent;
	};

void sEvent(ZRef<Stamp>& ioStamp);
ZRef<Stamp> sFork(ZRef<Stamp>& ioStamp);

} // namespace ZIntervalTreeClock
} // namespace ZooLib

#endif // __ZIntervalTreeClock__
