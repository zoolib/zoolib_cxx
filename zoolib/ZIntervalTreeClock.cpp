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
#pragma mark -
#pragma mark * Identity

static const ZRef<Identity> spZero = new Identity;
static const ZRef<Identity> spOne = new Identity;

static const ZRef<Identity> spZeroOne = new Identity(spZero, spOne);
static const ZRef<Identity> spOneZero = new Identity(spOne, spZero);

ZRef<Identity> Identity::sZero()
	{ return spZero; }

ZRef<Identity> Identity::sOne()
	{ return spOne; }

Identity::Identity()
	{}

Identity::~Identity()
	{}

Identity::Identity(const ZRef<Identity>& iLeft, const ZRef<Identity>& iRight)
:	fLeft(iLeft)
,	fRight(iRight)
	{
	ZAssert(fLeft && fRight);
	ZAssert(!(fLeft->IsZero() && fRight->IsZero()));
	ZAssert(!(fLeft->IsOne() && fRight->IsOne()));
	}

bool Identity::IsLeaf()
	{ return !fLeft; }

bool Identity::IsOne()
	{ return !fLeft && spZero != this; }

bool Identity::IsZero()
	{ return !fLeft && spZero == this; }

bool Identity::IsInternal()
	{ return fLeft; }

ZRef<Identity> Identity::Left()
	{ return fLeft; }

ZRef<Identity> Identity::Right()
	{ return fRight; }

void Identity::Split(ZRef<Identity>& oLeft, ZRef<Identity>& oRight)
	{
	if (this->IsZero())
		{
		oLeft = this;
		oRight = this;
		}
	else if (this->IsOne())
		{
		oLeft = spOneZero;
		oRight = spZeroOne;
		}
	else if (!fLeft->IsZero() && fRight->IsZero())
		{
		ZRef<Identity> newLeft, newRight;
		fLeft->Split(newLeft, newRight);

		oLeft = new Identity(newLeft, spZero);
		oRight = new Identity(newRight, spZero);
		}
	else if (fLeft->IsZero() && !fRight->IsZero())
		{
		ZRef<Identity> newLeft, newRight;
		fRight->Split(newLeft, newRight);

		oLeft = new Identity(spZero, newLeft);
		oRight = new Identity(spZero, newRight);
		}
	else
		{
		oLeft = new Identity(fLeft, spZero);
		oRight = new Identity(spZero, fRight);
		}
	}

ZRef<Identity> Identity::Summed(const ZRef<Identity>& iOther)
	{
	if (this->IsZero())
		{
		return iOther;
		}
	else if (iOther->IsZero())
		{
		return this;
		}
	else if (this->IsOne() || iOther->IsOne())
		{
		return this; // or we could return iOther.
		}
	else
		{
		ZAssert(this->IsInternal() && iOther->IsInternal());
		ZRef<Identity> newLeft = fLeft->Summed(iOther->fLeft);
		ZRef<Identity> newRight = fRight->Summed(iOther->fRight);

		if (newLeft->IsZero() && newRight->IsZero() || newLeft->IsOne() && newRight->IsOne())
			return newLeft; // or equivalently newRight.

		return new Identity(newLeft, newRight);		
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Event

const ZRef<Event> spEventZero = new Event;

ZRef<Event> Event::sZero()
	{ return spEventZero; }

Event::Event()
:	fValue(0)
	{}

Event::~Event()
	{}

Event::Event(size_t iValue)
:	fValue(iValue)
	{}

Event::Event(size_t iValue, const ZRef<Event>& iLeft, const ZRef<Event>& iRight)
:	fValue(iValue)
,	fLeft(iLeft)
,	fRight(iRight)
	{
	ZAssert(fLeft && fRight || !fLeft && !fRight);
	}

Event::Event(bool iWithZeroChildren, size_t iValue)
:	fValue(iValue)
,	fLeft(spEventZero)
,	fRight(spEventZero)
	{
	ZAssert(iWithZeroChildren);
	}

size_t Event::Value()
	{ return fValue; }

bool Event::IsLeaf()
	{ return !fLeft; }

bool Event::IsInternal()
	{ return fLeft; }

ZRef<Event> Event::Left()
	{ return fLeft; }

ZRef<Event> Event::Right()
	{ return fRight; }

bool Event::Equals(const ZRef<Event>& iOther)
	{
	if (iOther == this)
		return true;

	if (fValue != iOther->fValue)
		return false;

	if (fLeft && iOther->fLeft)
		return fLeft->Equals(iOther->fLeft) && fRight->Equals(iOther->fRight);

	return !fLeft && !iOther->fLeft;
	}

bool Event::LessEqual(const ZRef<Event>& iOther)
	{
	if (fValue > iOther->fValue)
		return false;

	if (this->IsLeaf())
		return true;

	if (iOther->IsInternal())
		{
		ZRef<Event> thisLiftedLeft = fLeft->pLifted(fValue);
		ZRef<Event> otherLiftedLeft = iOther->fLeft->pLifted(iOther->fValue);
		if (thisLiftedLeft->LessEqual(otherLiftedLeft))
			{
			ZRef<Event> thisLiftedRight = fRight->pLifted(fValue);
			ZRef<Event> otherLiftedRight = iOther->fRight->pLifted(iOther->fValue);
			return thisLiftedRight->LessEqual(otherLiftedRight);
			}
		}
	else
		{
		ZRef<Event> thisLiftedLeft = fLeft->pLifted(fValue);
		if (thisLiftedLeft->LessEqual(iOther))
			{
			ZRef<Event> thisLiftedRight = fRight->pLifted(fValue);
			return thisLiftedRight->LessEqual(iOther);
			}
		}
	return false;
	}

bool Event::IsBefore(const ZRef<Event>& iOther)
	{ return this->LessEqual(iOther) && !iOther->LessEqual(this); }

bool Event::IsAfter(const ZRef<Event>& iOther)
	{ return !this->LessEqual(iOther) && iOther->LessEqual(this); }

bool Event::IsConcurrent(const ZRef<Event>& iOther)
	{ return !this->LessEqual(iOther) && !iOther->LessEqual(this); }

bool Event::IsSame(const ZRef<Event>& iOther)
	{ return this->LessEqual(iOther) && iOther->LessEqual(this); }

ZRef<Event> Event::Joined(const ZRef<Event>& iOther)
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
			ZRef<Event> tmp = new Event(true, this->fValue);
			return tmp->Joined(iOther);
			}
		}
	else if (iOther->IsLeaf())
		{
		ZRef<Event> tmp = new Event(true, iOther->fValue);
		return this->Joined(tmp);
		}
	else
		{
		if (fValue > iOther->fValue)
			{
			const size_t d = fValue - iOther->fValue;
			ZRef<Event> newLeft = iOther->fLeft->Joined(fLeft->pLifted(d));
			ZRef<Event> newRight = iOther->fRight->Joined(fRight->pLifted(d));
			ZRef<Event> result = new Event(iOther->fValue, newLeft, newRight);
			return result->pNormalized();
			}
		else
			{
			const size_t d = iOther->fValue - this->fValue;
			ZRef<Event> newLeft = fLeft->Joined(iOther->fLeft->pLifted(d));
			ZRef<Event> newRight = fRight->Joined(iOther->fRight->pLifted(d));
			ZRef<Event> result = new Event(fValue, newLeft, newRight);
			return result->pNormalized();
			}
		}
	}

ZRef<Event> Event::Filled(const ZRef<Identity>& iIdentity)
	{
	if (iIdentity->IsZero())
		{
		return this;
		}
	else if (iIdentity->IsOne())
		{
		return new Event(this->pHeight());
		}
	else if (this->IsLeaf())
		{
		return this;
		}
	else
		{
		assert(iIdentity->IsInternal());
		const ZRef<Identity> identityLeft = iIdentity->Left();
		const ZRef<Identity> identityRight = iIdentity->Right();
		if (identityLeft->IsOne())
			{
			ZRef<Event> newRight = fRight->Filled(identityRight);
			ZRef<Event> newLeft = new Event(max(fLeft->pHeight(), newRight->pHeight()));
			ZRef<Event> result = new Event(fValue, newLeft, newRight);
			return result->pNormalized();
			}
		else if (identityRight->IsOne())
			{
			ZRef<Event> newLeft = fLeft->Filled(identityLeft);
			ZRef<Event> newRight = new Event(max(newLeft->pHeight(), fRight->pHeight()));
			ZRef<Event> result = new Event(fValue, newLeft, newRight);
			return result->pNormalized();
			}
		else
			{
			ZRef<Event> result =
				new Event(fValue, fLeft->Filled(identityLeft), fRight->Filled(identityRight));
			return result->pNormalized();
			}
		}		
	}

ZRef<Event> Event::Grown(const ZRef<Identity>& iIdentity)
	{
	ZRef<Event> result;
	this->pGrown(iIdentity, result);
	return result;
	}

size_t Event::pGrown(const ZRef<Identity>& iIdentity, ZRef<Event>& oEvent)
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
			ZRef<Event> tmp = new Event(true, fValue);
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
			const int costLeft = fLeft->pGrown(identityLeft, newEventLeft);
			
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

ZRef<Event> Event::pDropped(size_t d)
	{
	ZAssert(fValue >= d);
	if (d)
		return new Event(fValue - d, fLeft, fRight);
	return this;
	}

ZRef<Event> Event::pLifted(size_t d)
	{
	if (d)
		return new Event(fValue + d, fLeft, fRight);
	return this;
	}

ZRef<Event> Event::pNormalized()
	{
	if (fLeft)
		{
		if (const int tmp = min(fLeft->fValue, fRight->fValue))
			return new Event(fValue + tmp, fLeft->pDropped(tmp), fRight->pDropped(tmp));
		}
	return this;
	}

size_t Event::pHeight()
	{
	if (fLeft)
		return fValue + max(fLeft->pHeight(), fRight->pHeight());
	return fValue;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Stamp

ZRef<Stamp> Stamp::sSeed()
	{ return new Stamp(Identity::sOne(), Event::sZero()); }

Stamp::Stamp(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent)
:	fIdentity(iIdentity)
,	fEvent(iEvent)
	{
	ZAssert(fIdentity && fEvent);
	}

Stamp::~Stamp()
	{}

ZRef<Stamp> Stamp::Evented()
	{
	ZRef<Event> newEvent = fEvent->Filled(fIdentity);
	if (newEvent->Equals(fEvent))
		newEvent = newEvent->Grown(fIdentity);
	return new Stamp(fIdentity, newEvent);
	}

void Stamp::Forked(ZRef<Stamp>& oLeft, ZRef<Stamp>& oRight)
	{
	ZRef<Identity> newLeft, newRight;
	fIdentity->Split(newLeft, newRight);
	oLeft = new Stamp(newLeft, fEvent);
	oRight = new Stamp(newRight, fEvent);
	}

ZRef<Stamp> Stamp::Joined(const ZRef<Stamp>& iOther)
	{
	ZRef<Identity> newIdentity = fIdentity->Summed(iOther->fIdentity);
	ZRef<Event> newEvent = fEvent->Joined(iOther->fEvent);
	return new Stamp(newIdentity, newEvent);
	}

ZRef<Identity> Stamp::GetIdentity()
	{ return fIdentity; }

ZRef<Event> Stamp::GetEvent()
	{ return fEvent; }

bool Stamp::LessEqual(const ZRef<Stamp>& iOther)
	{ return fEvent->LessEqual(iOther->fEvent); }

bool Stamp::IsBefore(const ZRef<Stamp>& iOther)
	{ return fEvent->IsBefore(iOther->fEvent); }
	
bool Stamp::IsAfter(const ZRef<Stamp>& iOther)
	{ return fEvent->IsAfter(iOther->fEvent); }

bool Stamp::IsConcurrent(const ZRef<Stamp>& iOther)
	{ return fEvent->IsConcurrent(iOther->fEvent); }

bool Stamp::IsSame(const ZRef<Stamp>& iOther)
	{ return fEvent->IsSame(iOther->fEvent); }

void sEvent(ZRef<Stamp>& ioStamp)
	{ ioStamp = ioStamp->Evented(); }

ZRef<Stamp> sFork(ZRef<Stamp>& ioStamp)
	{
	ZRef<Stamp> a, b;
	ioStamp->Forked(a, b);
	ioStamp = a;
	return b;
	}

} // namespace ZIntervalTreeClock
} // namespace ZooLib
