/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/Starter_EventLoopBase.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark - Starter_EventLoopBase

Starter_EventLoopBase::Starter_EventLoopBase()
:	fTriggered(false)
	{}

Starter_EventLoopBase::~Starter_EventLoopBase()
	{}

bool Starter_EventLoopBase::QStart(const ZP<Startable>& iStartable)
	{
	ZAcqMtx acq(fMtx);
	if (iStartable)
		{
		if (fTriggered || (fTriggered = this->pTrigger()))
			{
			fStartables.push_back(iStartable);
			return true;
			}
		}
	return false;
	}

void Starter_EventLoopBase::pInvokeClearQueue()
	{
	vector<ZP<Startable>> toStart;

	{
	ZAcqMtx acq(fMtx);
	fTriggered = false;
	fStartables.swap(toStart);
	}

	for (vector<ZP<Startable>>::iterator iter = toStart.begin();
		iter != toStart.end(); ++iter)
		{
		try { (*iter)->Call(); }
		catch (...) {}
		}
	}

void Starter_EventLoopBase::pDiscardPending()
	{
	ZAcqMtx acq(fMtx);
	fStartables.clear();
	fTriggered = false;
	}

} // namespace ZooLib
