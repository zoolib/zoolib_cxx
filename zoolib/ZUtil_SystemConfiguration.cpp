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

#include "zoolib/ZUtil_SystemConfiguration.h"

#if ZCONFIG_SPI_Enabled(SystemConfiguration)

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZUtil_CF_Context.h"
#include "zoolib/ZYad_CF.h"

namespace ZooLib {
namespace ZUtil_SystemConfiguration {

using namespace ZUtil_CF;

// =================================================================================================
// MARK: - ZUtil_SystemConfiguration::Store

Store::Store()
	{}

Store::~Store()
	{}

void Store::Initialize()
	{
	ZCounted::Initialize();

	fStoreRef = sAdopt& ::SCDynamicStoreCreate(nullptr,
		CFSTR("ZUtil_SystemConfiguration"),
		&Store::spCallback,
		ZUtil_CF::Context<SCDynamicStoreContext>(this->GetWeakRefProxy()).IParam());
	}

SCDynamicStoreRef Store::GetStoreRef()
	{ return fStoreRef; }

void Store::Set_Callback(ZRef<Callable> iCallable)
	{ return fCallable.Set(iCallable); }

void Store::spCallback(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info)
	{
	if (ZRef<Store> theStore = ZWeakRef<Store>(static_cast<WeakRefProxy*>(info)))
		sCall(theStore->fCallable.Get(), theStore, changedKeys);
	}

// =================================================================================================
// MARK: - YadMapRPos (anonymous)

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
	virtual ZRef<ZYadR> ReadInc(std::string& oName)
		{
		while (fPosition < fKeys.Count())
			{
			if (ZQ<ZRef<CFStringRef> > theQName = fKeys.QGet<ZRef<CFStringRef> >(fPosition++))
				{
				if (ZVal_CF theVal = ::SCDynamicStoreCopyValue(fStoreRef, theQName.Get()))
					{
					oName = ZUtil_CF::sAsUTF8(theQName.Get());
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
	size_t fPosition;
	};

} // anonymous namespace 

// =================================================================================================
// MARK: - sYadMapRPos

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	{ return new YadMapRPos(iStoreRef, iKeys); }

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, const std::string& iPattern)
	{
	return new YadMapRPos(iStoreRef,
		sTempCF& ::SCDynamicStoreCopyKeyList(iStoreRef, ZUtil_CF::sString(iPattern)));
	}

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef)
	{ return sYadMapRPos(iStoreRef, ".+"); }

} // namespace ZUtil_SystemConfiguration
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(SystemConfiguration)
