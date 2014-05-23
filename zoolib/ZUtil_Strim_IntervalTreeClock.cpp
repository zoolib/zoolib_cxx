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

#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZUtil_Strim_Operators.h"

namespace ZooLib {
namespace ZIntervalTreeClock {

// =================================================================================================
// MARK: - ZIntervalTreeClock

const ZStrimW& operator<<(const ZStrimW& w, const ZRef<Identity>& iIdentity)
	{
	if (iIdentity->IsZero())
		w << "0";
	else if (iIdentity->IsOne())
		w << "1";
	else
		w << "(" << iIdentity->Left() << "," << iIdentity->Right() << ")";
	return w;
	}

const ZStrimW& operator<<(const ZStrimW& w, const ZRef<Event>& iEvent)
	{
	if (iEvent->IsLeaf())
		{
		w << iEvent->Value();
		}
	else
		{
		w << "(" << iEvent->Value()
			<< "+" << iEvent->Left() << "," << iEvent->Right() << ")";
		}
	return w;
	}

#if 0
const ZStrimW& operator<<(const ZStrimW& w, const ZRef<Clock>& iClock)
	{
	w << "[" << iClock->GetIdentity() << "/" << iClock->GetEvent() << "]";
	return w;
	}
#endif

} // namespace ZIntervalTreeClock
} // namespace ZooLib
