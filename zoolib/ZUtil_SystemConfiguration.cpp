/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZUtil_SystemConfiguration.h"

#if ZCONFIG_SPI_Enabled(SystemConfiguration)

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZYad_CF.h"

namespace ZooLib {
namespace ZUtil_SystemConfiguration {

using namespace ZUtil_CF;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_SystemConfiguration::Store

Store::Store()
	{}

Store::~Store()
	{}

void Store::Initialize()
	{
	ZCounted::Initialize();

	ZRef<WeakRefProxy> theWRP = this->GetWeakRefProxy();

	SCDynamicStoreContext context =
		{
		0,
		theWRP.Get(),
		(CFAllocatorRetainCallBack)&ZCountedWithoutFinalize::sRetain,
		(CFAllocatorReleaseCallBack)ZCountedWithoutFinalize::sRelease,
		nullptr
		};

	fStoreRef = Adopt& ::SCDynamicStoreCreate(nullptr,
		CFSTR("ZUtil_SystemConfiguration"),
		&Store::spCallback,
		&context);
	}

SCDynamicStoreRef Store::GetStoreRef()
	{ return fStoreRef; }

ZRef<Store::Callable> Store::GetSet_Callback(ZRef<Callable> iCallable)
	{ return fCallable.GetSet(iCallable); }

void Store::spCallback(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info)
	{
	if (ZRef<WeakRefProxy> theWRP = static_cast<WeakRefProxy*>(info))
		{
		if (ZRef<ZCountedBase> theCB = theWRP->GetCountedBase())
			{
			if (ZRef<Store> theStore = theCB.DynamicCast<Store>())
				{
				if (ZRef<Callable> theCallable = theStore->fCallable)
					theCallable->Call(theStore, changedKeys);
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * YadMapRPos (anonymous)

namespace { // anonymous 

class YadMapRPos
:	public ZYadMapRPos
	{
	YadMapRPos::YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys, uint64 iPosition)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(iPosition)
		{}

public:
	YadMapRPos::YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(0)
		{}

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName)
		{
		while (fPosition < fKeys.Count())
			{
			if (ZQ<ZRef<CFStringRef> > theQName = fKeys.QGet<ZRef<CFStringRef> >(fPosition++))
				{
				oName = ZUtil_CF::sAsUTF8(theQName.Get());
				if (ZVal_CF theVal = ::SCDynamicStoreCopyValue(fStoreRef, theQName.Get()))
					return sMakeYadR(theVal);
				}
			}
		return null;
		}

// From ZYadMapRPos
	virtual ZRef<ZYadMapRPos> Clone()
		{ return new YadMapRPos(fStoreRef, fKeys, fPosition); }

	virtual void SetPosition(const std::string& iName)
		{
		for (/*no init*/; fPosition < fKeys.Count(); ++fPosition)
			{
			if (ZQ<ZRef<CFStringRef> > theQName = fKeys.QGet<ZRef<CFStringRef> >(fPosition))
				{
				if (iName == ZUtil_CF::sAsUTF8(theQName.Get()))
					break;
				}
			}
		}

private:
	const ZRef<SCDynamicStoreRef> fStoreRef;
	const ZSeq_CF fKeys;
	uint64 fPosition;
	};

} // anonymous namespace 

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadMapRPos

ZRef<ZYadMapRPos> sMakeYadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	{ return new YadMapRPos(iStoreRef, iKeys); }

ZRef<ZYadMapRPos> sMakeYadMapRPos(SCDynamicStoreRef iStoreRef, const std::string& iPattern)
	{
	return new YadMapRPos(iStoreRef,
		TempCF& ::SCDynamicStoreCopyKeyList(iStoreRef, ZUtil_CF::sString(iPattern)));
	}

ZRef<ZYadMapRPos> sMakeYadMapRPos(SCDynamicStoreRef iStoreRef)
	{ return sMakeYadMapRPos(iStoreRef, ".+"); }

} // namespace ZUtil_SystemConfiguration
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(SystemConfiguration)
