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

#include "zoolib/ZCounted.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZSafePtrStack.h"

namespace ZooLib {
namespace ZIntervalTreeClock {

// =================================================================================================
// MARK: - Identity

class Identity
:	public ZCountedWithoutFinalize
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

	ZRef<Identity> Left() const;
	ZRef<Identity> Right() const;

	void Split(ZRef<Identity>& oLeft, ZRef<Identity>& oRight) const;

	ZRef<Identity> Summed(const ZRef<Identity>& iOther) const;

private:
	const ZRef<Identity> fLeft;
	const ZRef<Identity> fRight;
	};

// =================================================================================================
// MARK: - Event

class Event
:	public ZCounted
	{
public:
	static ZRef<Event> sZero();

	Event();
	virtual ~Event();

	Event(size_t iValue);
	Event(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight);

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

	const ZRef<Event> fLeft;
	const ZRef<Event> fRight;
	const size_t fValue;

	mutable ZRef<Event> fLeftLifted;
	mutable ZRef<Event> fRightLifted;
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
