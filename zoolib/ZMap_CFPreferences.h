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

#ifndef __ZMap_CFPreferences_h__
#define __ZMap_CFPreferences_h__ 1
#include "zconfig.h"

#include "zoolib/ZVal_CF.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include <CoreFoundation/CFPreferences.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZMap_CFPreferences

class ZMap_CFPreferences
	{
public:
	ZMap_CFPreferences();
	ZMap_CFPreferences(const ZMap_CFPreferences& iOther);
	~ZMap_CFPreferences();
	ZMap_CFPreferences& operator=(const ZMap_CFPreferences& iOther);

	ZMap_CFPreferences(const string8& iApplicationID);
	ZMap_CFPreferences(CFStringRef iApplicationID);

// ZMap protocol
	ZQ<ZVal_CF> QGet(const string8& iName) const;
	ZQ<ZVal_CF> QGet(CFStringRef iName) const;

	ZVal_CF DGet(const ZVal_CF& iDefault, const string8& iName) const;
	ZVal_CF DGet(const ZVal_CF& iDefault, CFStringRef iName) const;

	ZVal_CF Get(const string8& iName) const;
	ZVal_CF Get(CFStringRef iName) const;

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	ZMap_CFPreferences& Set(const string8& iName, const ZVal_CF& iVal);
	ZMap_CFPreferences& Set(CFStringRef iName, const ZVal_CF& iVal);

	template <class S>
	ZMap_CFPreferences& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, ZVal_CF(iVal)); }

	template <class S>
	ZMap_CFPreferences& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, ZVal_CF(iVal)); }

	ZMap_CFPreferences& Erase(const string8& iName);
	ZMap_CFPreferences& Erase(CFStringRef iName);

private:
	ZRef<CFStringRef> fApplicationID;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

#endif // __ZMap_CFPreferences_h__
