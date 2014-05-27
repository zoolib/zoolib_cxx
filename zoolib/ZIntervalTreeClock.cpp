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

#include "zoolib/ZCompat_algorithm.h" // for std::min/std::max
#include "zoolib/ZDebug.h"
#include "zoolib/ZIntervalTreeClock.h"
#include "zoolib/ZSingleton.h"

#include "zoolib/ZLog.h"

/*
See the paper "Interval Tree Clocks: A Logical Clock for Dynamic Systems"
by Paulo Sérgio Almeida, Carlos Baquero and Victor Fonte.
http://gsd.di.uminho.pt/members/cbm/ps/itc2008.pdf

Derived from the C and Java implementations at http://code.google.com/p/itclocks/
The Erlang version of that code indicates that it is licensed under the Erlang Public License.
The project page itself indicates that the code is licensed under the LGPL.
*/

namespace ZooLib {
namespace ZIntervalTreeClock {

using std::min;
using std::max;

// =================================================================================================
// MARK: - Identity

ZRef<Identity> Identity::sZero()
	{
	static const ZRef<Identity> spZero = new Identity;
	return spZero;
	}

ZRef<Identity> Identity::sOne()
	{
	static const ZRef<Identity> spOne = new Identity;
	return spOne;
	}

static const ZRef<Identity> spZeroOne = new Identity(Identity::sZero(), Identity::sOne());
static const ZRef<Identity> spOneZero = new Identity(Identity::sOne(), Identity::sZero());

Identity::Identity()
	{}

Identity::~Identity()
	{}

Identity::Identity(const ZRef<Identity>& iLeft, const ZRef<Identity>& iRight)
:	fLeft(iLeft)
,	fRight(iRight)
	{
	ZAssert(fLeft && fRight);
	ZAssert(not (fLeft->IsZero() && fRight->IsZero()));
	ZAssert(not (fLeft->IsOne() && fRight->IsOne()));
	}

bool Identity::IsOne() const
	{ return not fLeft && sZero() != this; }

bool Identity::IsZero() const
	{ return sZero() == this; }

bool Identity::IsLeaf() const
	{ return not fLeft; }

bool Identity::IsInternal() const
	{ return fLeft; }

ZRef<Identity> Identity::Left() const
	{ return fLeft; }

ZRef<Identity> Identity::Right() const
	{ return fRight; }

void Identity::Split(ZRef<Identity>& oLeft, ZRef<Identity>& oRight) const
	{
	if (this->IsZero())
		{
		oLeft = const_cast<Identity*>(this);
		oRight = const_cast<Identity*>(this);
		}
	else if (this->IsOne())
		{
		oLeft = spOneZero;
		oRight = spZeroOne;
		}
	else if (not fLeft->IsZero() && fRight->IsZero())
		{
		ZRef<Identity> newLeft, newRight;
		fLeft->Split(newLeft, newRight);

		oLeft = new Identity(newLeft, sZero());
		oRight = new Identity(newRight, sZero());
		}
	else if (fLeft->IsZero() && not fRight->IsZero())
		{
		ZRef<Identity> newLeft, newRight;
		fRight->Split(newLeft, newRight);

		oLeft = new Identity(sZero(), newLeft);
		oRight = new Identity(sZero(), newRight);
		}
	else
		{
		oLeft = new Identity(fLeft, sZero());
		oRight = new Identity(sZero(), fRight);
		}
	}

ZRef<Identity> Identity::Summed(const ZRef<Identity>& iOther) const
	{
	if (this->IsZero())
		{
		return iOther;
		}
	else if (iOther->IsZero())
		{
		return const_cast<Identity*>(this);
		}
	else if (this->IsOne() || iOther->IsOne())
		{
		// This case is not covered in the ITC paper, but seems reasonable to me.
		ZLOGTRACE(eDebug);
		return const_cast<Identity*>(this);
		}
	else
		{
		ZAssert(this->IsInternal() && iOther->IsInternal());
		ZRef<Identity> newLeft = fLeft->Summed(iOther->fLeft);
		ZRef<Identity> newRight = fRight->Summed(iOther->fRight);

		if ((newLeft->IsZero() && newRight->IsZero()) || (newLeft->IsOne() && newRight->IsOne()))
			return newLeft; // or equivalently newRight.

		return new Identity(newLeft, newRight);
		}
	}

// =================================================================================================
// MARK: - Event

ZAtomic_t sAllocated;
ZAtomic_t sFreed;

//namespace {
//
//ZSafePtrStack_WithDestroyer<Event,SafePtrStackLink_Event> spSafePtrStack_Event;
//
//} // anonymous namespace
//
//ZRef<ZMap_Any::Rep> ZMap_Any::Rep::sMake()
//	{
//	if (Rep* result = spSafePtrStack_Map_Any_Rep.PopIfNotEmpty<Rep>())
//		return result;
//
//	return new Rep;
//	}
//
ZRef<Event> Event::sZero()
	{ return sSingleton<ZRef_Counted<Event> >(); }

Event::Event()
:	fValue(0)
	{
	sAtomic_Inc(&sAllocated);
	}

Event::~Event()
	{
	sAtomic_Inc(&sFreed);
	}

Event::Event(size_t iValue)
:	fValue(iValue)
	{
	sAtomic_Inc(&sAllocated);
	}

Event::Event(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
:	fLeft(iLeft)
,	fRight(iRight)
,	fValue(iValue)
	{
	ZAssert(fLeft && fRight || not fLeft && not fRight);
	sAtomic_Inc(&sAllocated);
	}

//void Event::Rep::Finalize()
//	{
//	bool finalized = this->FinishFinalize();
//	ZAssert(finalized);
//	ZAssert(not this->IsReferenced());
//	fLeft.Clear();
//	fLeftLifted.Clear();
//	fRight.Clear();
//	fRightLifted.Clear();
//
//	spSafePtrStack_Event.Push(this);
//	}

size_t Event::Value() const
	{ return fValue; }

const ZRef<Event>& Event::Left() const
	{ return fLeft; }

const ZRef<Event>& Event::Right() const
	{ return fRight; }

bool Event::IsLeaf() const
	{ return not fLeft; }

bool Event::IsInternal() const
	{ return fLeft; }

ZAtomic_t sCalls_Left;
ZAtomic_t sCalls_Right;

ZAtomic_t sHits_Left;
ZAtomic_t sHits_Right;

typedef ZRef<Event> Ev;

bool Event::LessEqual(const ZRef<Event>& iOther) const
	{
	if (fLeft)
		{
		if (fValue > iOther->fValue)
			return false;

		Ev xl1 = fLeft->pLifted(fValue);
		if (iOther->fLeft)
			{
			Ev xl2 = iOther->fLeft->pLifted(iOther->fValue);
			if (not xl1->LessEqual(xl2))
				return false;

			Ev xr1 = fRight->pLifted(fValue);
			Ev xr2 = iOther->fRight->pLifted(iOther->fValue);
			return xr1->LessEqual(xr2);
			}
		else
			{
			if (not xl1->LessEqual(iOther))
				return false;
			Ev xr1 = fRight->pLifted(fValue);
			return xr1->LessEqual(iOther);
			}
		}
	else if (iOther->fLeft)
		{
		if (fValue < iOther->fValue)
			return true;
		Ev newEv = new Event(fValue, sZero(), sZero());
		return newEv->LessEqual(iOther);
		}
	else
		{
		return fValue <= iOther->fValue;
		}
	}

ZRef<Event> Event::Advanced(const ZRef<Identity>& iIdentity) const
	{
	ZRef<Event> newEvent = this->pFilled(iIdentity);
	if (newEvent->pEqual(const_cast<Event*>(this)))
		newEvent->pGrown(iIdentity, newEvent);
	return newEvent;
	}

ZRef<Event> Event::Joined(const ZRef<Event>& iOther) const
	{
	if (this->IsLeaf())
		{
		if (iOther->IsLeaf())
			{
			return new Event(max(this->fValue, iOther->fValue));
			}
		else
			{
			// Could call iOther->Joined(this);
			const ZRef<Event> tmp = new Event(this->fValue, sZero(), sZero());
			return tmp->Joined(iOther);
			}
		}
	else if (iOther->IsLeaf())
		{
		const ZRef<Event> tmp = new Event(iOther->fValue, sZero(), sZero());
		return this->Joined(tmp);
		}
	else if (fValue > iOther->fValue)
		{
		const size_t d = fValue - iOther->fValue;
		const ZRef<Event> newLeft = iOther->fLeft->Joined(fLeft->pLifted(d));
		const ZRef<Event> newRight = iOther->fRight->Joined(fRight->pLifted(d));
		const ZRef<Event> result = new Event(iOther->fValue, newLeft, newRight);
		return result->pNormalized();
		}
	else
		{
		const size_t d = iOther->fValue - this->fValue;
		const ZRef<Event> newLeft = fLeft->Joined(iOther->fLeft->pLifted(d));
		const ZRef<Event> newRight = fRight->Joined(iOther->fRight->pLifted(d));
		const ZRef<Event> result = new Event(fValue, newLeft, newRight);
		return result->pNormalized();
		}
	}

bool Event::pEqual(const ZRef<Event>& iOther) const
	{
	if (iOther == this)
		return true;

	if (fValue != iOther->fValue)
		return false;

	if (fLeft && iOther->fLeft)
		return fLeft->pEqual(iOther->fLeft) && fRight->pEqual(iOther->fRight);

	return not fLeft && not iOther->fLeft;
	}

size_t Event::pGrown(const ZRef<Identity>& iIdentity, ZRef<Event>& oEvent) const
	{
	if (this->IsLeaf())
		{
		if (iIdentity->IsOne())
			{
			oEvent = this->pLifted(1);
			return 0;
			}
		else
			{
			const ZRef<Event> tmp = new Event(fValue, sZero(), sZero());
			return 1000 + tmp->pGrown(iIdentity, oEvent);
			}
		}
	else
		{
		ZAssert(iIdentity->IsInternal());
		const ZRef<Identity> identityLeft = iIdentity->Left();
		const ZRef<Identity> identityRight = iIdentity->Right();
		if (identityLeft->IsZero())
			{
			ZRef<Event> tmp;
			size_t cost = 1 + fRight->pGrown(identityRight, tmp);
			oEvent = new Event(fValue, fLeft, tmp);
			return cost;
			}
		else if (identityRight->IsZero())
			{
			ZRef<Event> tmp;
			size_t cost = 1 + fLeft->pGrown(identityLeft, tmp);
			oEvent = new Event(fValue, tmp, fRight);
			return cost;
			}
		else
			{
			ZRef<Event> newEventLeft;
			const size_t costLeft = fLeft->pGrown(identityLeft, newEventLeft);

			ZRef<Event> newEventRight;
			size_t costRight = fRight->pGrown(identityRight, newEventRight);

			if (costLeft < costRight)
				{
				oEvent = new Event(fValue, newEventLeft, fRight);
				return 1 + costLeft;
				}
			else
				{
				oEvent = new Event(fValue, fLeft, newEventRight);
				return 1 + costRight;
				}
			}
		}
	}

ZRef<Event> Event::pFilled(const ZRef<Identity>& iIdentity) const
	{
	if (iIdentity->IsZero())
		{
		return const_cast<Event*>(this);
		}
	else if (iIdentity->IsOne())
		{
		return new Event(this->pHeight());
		}
	else if (this->IsLeaf())
		{
		return const_cast<Event*>(this);
		}
	else
		{
		const ZRef<Identity> identityLeft = iIdentity->Left();
		const ZRef<Identity> identityRight = iIdentity->Right();
		if (identityLeft->IsOne())
			{
			const ZRef<Event> newRight = fRight->pFilled(identityRight);
			const ZRef<Event> newLeft = new Event(max(fLeft->pHeight(), newRight->pHeight()));
			const ZRef<Event> result = new Event(fValue, newLeft, newRight);
			return result->pNormalized();
			}
		else if (identityRight->IsOne())
			{
			const ZRef<Event> newLeft = fLeft->pFilled(identityLeft);
			const ZRef<Event> newRight = new Event(max(newLeft->pHeight(), fRight->pHeight()));
			const ZRef<Event> result = new Event(fValue, newLeft, newRight);
			return result->pNormalized();
			}
		else
			{
			const ZRef<Event> result =
				new Event(fValue, fLeft->pFilled(identityLeft), fRight->pFilled(identityRight));
			return result->pNormalized();
			}
		}
	}

ZRef<Event> Event::pDropped(size_t d) const
	{
	ZAssert(fValue >= d);
	if (d)
		return new Event(fValue - d, fLeft, fRight);
	return const_cast<Event*>(this);
	}

ZRef<Event> Event::pLifted(size_t d) const
	{
	if (d)
		return new Event(fValue + d, fLeft, fRight);
	return const_cast<Event*>(this);
	}

ZRef<Event> Event::pNormalized() const
	{
	if (fLeft)
		{
		if (not fLeft->fLeft and not fRight->fLeft and fLeft->fValue == fRight->fValue)
			return new Event(fValue + fLeft->fValue, null, null);
		if (const size_t tmp = min(fLeft->fValue, fRight->fValue))
			return new Event(fValue + tmp, fLeft->pDropped(tmp), fRight->pDropped(tmp));
		}
	return const_cast<Event*>(this);
	}

size_t Event::pHeight() const
	{
	if (fLeft)
		return fValue + max(fLeft->pHeight(), fRight->pHeight());
	return fValue;
	}

// =================================================================================================
// MARK: -

bool sIsBefore(const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
	{ return iLeft->LessEqual(iRight) && not iRight->LessEqual(iLeft); }

bool sIsAfter(const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
	{ return not iLeft->LessEqual(iRight) && iRight->LessEqual(iLeft); }

bool sIsConcurrent(const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
	{ return not iLeft->LessEqual(iRight) && not iRight->LessEqual(iLeft); }

bool sIsSame(const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
	{ return iLeft->LessEqual(iRight) && iRight->LessEqual(iLeft); }

} // namespace ZIntervalTreeClock
} // namespace ZooLib
