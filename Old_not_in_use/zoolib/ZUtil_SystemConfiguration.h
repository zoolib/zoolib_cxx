// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZUtil_SystemConfiguration_h__
#define __ZUtil_SystemConfiguration_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(SystemConfiguration)

#include "zoolib/Callable.h"

#include "zoolib/ZRef_CF.h"
#include "zoolib/ZYad.h"

#include <SystemConfiguration/SCDynamicStore.h>

namespace ZooLib {
namespace ZUtil_SystemConfiguration {

// =================================================================================================
#pragma mark - Store

class Store : public ZCounted
	{
public:
	typedef Callable<void(ZRef<Store>,CFArrayRef)> Callable;

	Store(ZRef<Callable> iCallable);
	virtual ~Store();

// From ZCounted
	virtual void Initialize();

// Our protocol
	SCDynamicStoreRef GetStoreRef();

private:
	static void spCallback(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info);

	const ZRef<Callable> fCallable;
	ZRef<SCDynamicStoreRef> fStoreRef;
	};

// =================================================================================================
#pragma mark - sYadMapRPos

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys);
ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, const std::string& iPattern);
ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef);

} // namespace ZUtil_SystemConfiguration
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(SystemConfiguration)

#endif // __ZUtil_SystemConfiguration_h__
