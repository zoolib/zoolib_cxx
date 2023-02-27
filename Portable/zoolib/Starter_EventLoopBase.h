// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Starter_EventLoopBase_h__
#define __ZooLib_Starter_EventLoopBase_h__ 1
#include "zconfig.h"

#include "zoolib/Starter.h"

#include "zoolib/ZThread.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_EventLoopBase

class Starter_EventLoopBase
:	public Starter
	{
public:
	Starter_EventLoopBase();
	virtual ~Starter_EventLoopBase();

// From Starter
	virtual bool QStart(const ZP<Startable>& iStartable);

protected:
// Called by concrete subclass
	bool pInvokeClearQueue();

	bool pDiscardPending();

// Implemented by concrete subclass
	virtual bool pTrigger() = 0;

private:
	ZMtx fMtx;
	bool fTriggered;
	std::vector<ZP<Startable>> fStartables;
	};

} // namespace ZooLib

#endif // __ZooLib_Starter_EventLoopBase_h__
