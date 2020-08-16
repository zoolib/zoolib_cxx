// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ZUtil_SystemConfiguration.h"

#if ZCONFIG_SPI_Enabled(SystemConfiguration)

#include "zoolib/Compare_T.h"

#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZUtil_CF_Context.h"
#include "zoolib/ZYad_CF.h"

namespace ZooLib {
namespace ZUtil_SystemConfiguration {

using namespace ZUtil_CF;

// =================================================================================================
#pragma mark - ZUtil_SystemConfiguration::Store

Store::Store(ZRef<Callable> iCallable)
:	fCallable(iCallable)
	{}

Store::~Store()
	{}

void Store::Initialize()
	{
	ZCounted::Initialize();

	fStoreRef = sAdopt& ::SCDynamicStoreCreate(nullptr,
		CFSTR("ZUtil_SystemConfiguration"),
		&Store::spCallback,
		ZUtil_CF::Context<SCDynamicStoreContext>(this->GetWeakRefProxy()));
	}

SCDynamicStoreRef Store::GetStoreRef()
	{ return fStoreRef; }

void Store::spCallback(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info)
	{
	if (ZRef<Store> theStore = ZWeakRef<Store>(static_cast<WeakRefProxy*>(info)))
		sCall(theStore->fCallable, theStore, changedKeys);
	}

// =================================================================================================
#pragma mark - YadMapRPos (anonymous)

namespace { // anonymous 

class YadMapRPos
:	public ZYadMapRPos
	{
	YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys, size_t iPosition)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(iPosition)
		{}

public:
	YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(0)
		{}

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(Name& oName)
		{
		while (fPosition < fKeys.Count())
			{
			if (ZQ<ZRef<CFStringRef> > qName = fKeys.QGet<ZRef<CFStringRef> >(fPosition++))
				{
				if (ZVal_CF theVal = ::SCDynamicStoreCopyValue(fStoreRef, *qName))
					{
					oName = ZUtil_CF::sAsUTF8(*qName);
					return sYadR(theVal);
					}
				}
			}
		return null;
		}

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone()
		{ return new YadMapRPos(fStoreRef, fKeys, fPosition); }

// From ZYadMapRPos
	virtual void SetPosition(const Name& iName)
		{
		for (/*no init*/; fPosition < fKeys.Count(); ++fPosition)
			{
			if (ZQ<ZRef<CFStringRef> > qName = fKeys.QGet<ZRef<CFStringRef> >(fPosition))
				{
				if (iName == ZUtil_CF::sAsUTF8(*qName))
					break;
				}
			}
		}

private:
	const ZRef<SCDynamicStoreRef> fStoreRef;
	const ZSeq_CF fKeys;
	size_t fPosition;
	};

} // anonymous namespace 

// =================================================================================================
#pragma mark - sYadMapRPos

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	{ return new YadMapRPos(iStoreRef, iKeys); }

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, const std::string& iPattern)
	{
	return new YadMapRPos(iStoreRef,
		sTempPtrRef& ::SCDynamicStoreCopyKeyList(iStoreRef, ZUtil_CF::sString(iPattern)));
	}

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef)
	{ return sYadMapRPos(iStoreRef, ".+"); }

} // namespace ZUtil_SystemConfiguration
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(SystemConfiguration)
