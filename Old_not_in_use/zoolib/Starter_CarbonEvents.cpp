// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/OSX/Starter_CarbonEvents.h"

#if ZCONFIG_SPI_Enabled(Carbon64)

#include "zoolib/ZUtil_CarbonEvents.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_CarbonEvents

ZRef<Starter_CarbonEvents> Starter_CarbonEvents::sGet()
	{ return sSingleton<ZRef_Counted<Starter_CarbonEvents> >(); }

bool Starter_CarbonEvents::pTrigger()
	{
	this->Retain();
	ZUtil_CarbonEvents::sInvokeOnMainThread(true, spCallback, this);
	return true;
	}

void Starter_CarbonEvents::spCallback(void* iRefcon)
	{
	if (ZRef<Starter_CarbonEvents> theStarter = static_cast<Starter_CarbonEvents*>(iRefcon))
		{
		theStarter->Release();
		theStarter->pInvokeClearQueue();
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon64)
