// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

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
	fMtx.lock();

	if (not fTriggered)
		{
		fMtx.unlock();
		return;
		}

	fTriggered = false;

	vector<ZP<Startable>> toStart(std::move(fStartables));

	fMtx.unlock();

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
