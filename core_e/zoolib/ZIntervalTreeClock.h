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

#ifndef __ZIntervalTreeClock_h__
#define __ZIntervalTreeClock_h__ 1
#include "zconfig.h"

#include "zoolib/SafePtrStack.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZRef.h"

namespace ZooLib {
namespace ZIntervalTreeClock {

// =================================================================================================
// MARK: - Identity

class Identity
:	public CountedWithoutFinalize
	{
public:
	static ZRef<Identity> sZero();
	static ZRef<Identity> sOne();

	Identity();
	virtual ~Identity();

	Identity(const ZRef<Identity>& iLeft, const ZRef<Identity>& iRight);

	bool IsOne() const;
	bool IsZero() const;

	bool IsLeaf() const;
	bool IsInternal() const;

	const ZRef<Identity>& Left() const;
	const ZRef<Identity>& Right() const;

	void Splitted(ZRef<Identity>& oLeft, ZRef<Identity>& oRight) const;

	ZRef<Identity> Summed(const ZRef<Identity>& iOther) const;

private:
	const ZRef<Identity> fLeft;
	const ZRef<Identity> fRight;
	};

ZRef<Identity> sSplit(ZRef<Identity>& ioIdentity);

// =================================================================================================
// MARK: - Event

class Event;

class SafePtrStackLink_Event
:	public SafePtrStackLink<Event,SafePtrStackLink_Event>
	{};

class Event
:	public ZCounted
,	public SafePtrStackLink_Event
	{
public:
	static ZRef<Event> sZero();

	static ZRef<Event> sMake(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight);

	Event();
	virtual ~Event();

// From ZCounted
	virtual void Finalize();

// Accessors, mainly for text and binary streaming.
	size_t Value() const;

	const ZRef<Event>& Left() const;
	const ZRef<Event>& Right() const;

	bool IsLeaf() const;
	bool IsInternal() const;

// Comparison
	bool LessEqual(const ZRef<Event>& iOther) const;

// Fundamental operations
	ZRef<Event> Advanced(const ZRef<Identity>& iIdentity) const;
	ZRef<Event> Joined(const ZRef<Event>& iOther) const;

private:
	bool pEqual(const ZRef<Event>& iOther) const;
	size_t pGrown(const ZRef<Identity>& iIdentity, ZRef<Event>& oEvent) const;

	ZRef<Event> pFilled(const ZRef<Identity>& iIdentity) const;
	ZRef<Event> pDropped(size_t d) const;
	ZRef<Event> pLifted(size_t d) const;
	ZRef<Event> pNormalized() const;
	size_t pHeight() const;

	// Only changed by sMake and Finalize
	ZRef<Event> fLeft;
	ZRef<Event> fRight;
	size_t fValue;
	};

// =================================================================================================
// MARK: -

bool sIsBefore(const ZRef<Event>& iLeft, const ZRef<Event>& iRight);
bool sIsAfter(const ZRef<Event>& iLeft, const ZRef<Event>& iRight);
bool sIsConcurrent(const ZRef<Event>& iLeft, const ZRef<Event>& iRight);
bool sIsSame(const ZRef<Event>& iLeft, const ZRef<Event>& iRight);

} // namespace ZIntervalTreeClock
} // namespace ZooLib

#endif // __ZIntervalTreeClock_h__
