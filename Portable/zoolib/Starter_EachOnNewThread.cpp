// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Starter_EachOnNewThread.h"

#include "zoolib/StartOnNewThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_EachOnNewThread

class Starter_EachOnNewThread
:	public Starter
	{
public:
// From Starter
	virtual bool QStart(const ZP<Startable>& iStartable)
		{
		if (iStartable)
			{
			try
				{
				sStartOnNewThread(iStartable);
				return true;
				}
			catch (...)
				{}
			}
		return false;
		}
	};

// =================================================================================================
#pragma mark - sStarter_EachOnNewThread

ZP<Starter> sStarter_EachOnNewThread()
	{ return sSingleton<ZP_Counted<Starter_EachOnNewThread>>(); }

} // namespace ZooLib
