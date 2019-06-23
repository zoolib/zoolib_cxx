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

#ifndef __ZooLib_Apple_Map_CFPreferences_h__
#define __ZooLib_Apple_Map_CFPreferences_h__ 1
#include "zconfig.h"

#include "zoolib/Apple/Val_CF.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include <CoreFoundation/CFPreferences.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - Map_CFPreferences

class Map_CFPreferences
	{
public:
	Map_CFPreferences();
	Map_CFPreferences(const Map_CFPreferences& iOther);
	~Map_CFPreferences();
	Map_CFPreferences& operator=(const Map_CFPreferences& iOther);

	Map_CFPreferences(const string8& iApplicationID);
	Map_CFPreferences(CFStringRef iApplicationID);

// ZMap protocol
	const ZQ<Val_CF> QGet(const string8& iName) const;
	const ZQ<Val_CF> QGet(CFStringRef iName) const;

	const Val_CF DGet(const Val_CF& iDefault, const string8& iName) const;
	const Val_CF DGet(const Val_CF& iDefault, CFStringRef iName) const;

	const Val_CF Get(const string8& iName) const;
	const Val_CF Get(CFStringRef iName) const;

	template <class S>
	const ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	const S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	Map_CFPreferences& Set(const string8& iName, const Val_CF& iVal);
	Map_CFPreferences& Set(CFStringRef iName, const Val_CF& iVal);

	template <class S>
	Map_CFPreferences& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, Val_CF(iVal)); }

	template <class S>
	Map_CFPreferences& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, Val_CF(iVal)); }

	Map_CFPreferences& Erase(const string8& iName);
	Map_CFPreferences& Erase(CFStringRef iName);

private:
	ZP<CFStringRef> fApplicationID;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

#endif // __ZooLib_Apple_Map_CFPreferences_h__
