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

#include "zoolib/ZMap_CFPreferences.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/ZUtil_CF.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZMap_CFPreferences

ZMap_CFPreferences::ZMap_CFPreferences()
:	fApplicationID(kCFPreferencesCurrentApplication)
	{}

ZMap_CFPreferences::ZMap_CFPreferences(const ZMap_CFPreferences& iOther)
:	fApplicationID(iOther.fApplicationID)
	{}

ZMap_CFPreferences::~ZMap_CFPreferences()
	{}

ZMap_CFPreferences& ZMap_CFPreferences::operator=(const ZMap_CFPreferences& iOther)
	{
	fApplicationID = iOther.fApplicationID;
	return *this;
	}

ZMap_CFPreferences::ZMap_CFPreferences(const string8& iApplicationID)
:	fApplicationID(ZUtil_CF::sString(iApplicationID))
	{}

ZMap_CFPreferences::ZMap_CFPreferences(CFStringRef iApplicationID)
:	fApplicationID(iApplicationID)
	{}

const ZQ<ZVal_CF> ZMap_CFPreferences::QGet(const string8& iName) const
	{ return this->QGet(ZUtil_CF::sString(iName)); }

const ZQ<ZVal_CF> ZMap_CFPreferences::QGet(CFStringRef iName) const
	{
	if (ZRef<CFPropertyListRef> theVal = sAdopt& ::CFPreferencesCopyAppValue(iName, fApplicationID))
		return theVal;
	return null;
	}

const ZVal_CF ZMap_CFPreferences::DGet(const ZVal_CF& iDefault, const string8& iName) const
	{ return this->DGet(iDefault, ZUtil_CF::sString(iName)); }

const ZVal_CF ZMap_CFPreferences::DGet(const ZVal_CF& iDefault, CFStringRef iName) const
	{
	if (ZQ<ZVal_CF> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const ZVal_CF ZMap_CFPreferences::Get(const string8& iName) const
	{ return this->Get(ZUtil_CF::sString(iName)); }

const ZVal_CF ZMap_CFPreferences::Get(CFStringRef iName) const
	{
	if (ZQ<ZVal_CF> theQ = this->QGet(iName))
		return *theQ;
	return ZVal_CF();
	}

ZMap_CFPreferences& ZMap_CFPreferences::Set(const string8& iName, const ZVal_CF& iVal)
	{ return this->Set(ZUtil_CF::sString(iName), iVal); }

ZMap_CFPreferences& ZMap_CFPreferences::Set(CFStringRef iName, const ZVal_CF& iVal)
	{
	::CFPreferencesSetAppValue(iName, iVal, fApplicationID);
	return *this;
	}

ZMap_CFPreferences& ZMap_CFPreferences::Erase(const string8& iName)
	{ return this->Erase(ZUtil_CF::sString(iName)); }

ZMap_CFPreferences& ZMap_CFPreferences::Erase(CFStringRef iName)
	{
	::CFPreferencesSetAppValue(iName, nullptr, fApplicationID);
	return *this;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
