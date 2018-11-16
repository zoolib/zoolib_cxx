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

#include "zoolib/Apple/Map_CFPreferences.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/Apple/Util_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Map_CFPreferences

Map_CFPreferences::Map_CFPreferences()
:	fApplicationID(kCFPreferencesCurrentApplication)
	{}

Map_CFPreferences::Map_CFPreferences(const Map_CFPreferences& iOther)
:	fApplicationID(iOther.fApplicationID)
	{}

Map_CFPreferences::~Map_CFPreferences()
	{ ::CFPreferencesAppSynchronize(fApplicationID); }

Map_CFPreferences& Map_CFPreferences::operator=(const Map_CFPreferences& iOther)
	{
	fApplicationID = iOther.fApplicationID;
	return *this;
	}

Map_CFPreferences::Map_CFPreferences(const string8& iApplicationID)
:	fApplicationID(Util_CF::sString(iApplicationID))
	{}

Map_CFPreferences::Map_CFPreferences(CFStringRef iApplicationID)
:	fApplicationID(iApplicationID)
	{}

const ZQ<Val_CF> Map_CFPreferences::QGet(const string8& iName) const
	{ return this->QGet(Util_CF::sString(iName)); }

const ZQ<Val_CF> Map_CFPreferences::QGet(CFStringRef iName) const
	{
	if (ZRef<CFPropertyListRef> theVal = sAdopt& ::CFPreferencesCopyAppValue(iName, fApplicationID))
		return theVal;
	return null;
	}

const Val_CF Map_CFPreferences::DGet(const Val_CF& iDefault, const string8& iName) const
	{ return this->DGet(iDefault, Util_CF::sString(iName)); }

const Val_CF Map_CFPreferences::DGet(const Val_CF& iDefault, CFStringRef iName) const
	{
	if (ZQ<Val_CF> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const Val_CF Map_CFPreferences::Get(const string8& iName) const
	{ return this->Get(Util_CF::sString(iName)); }

const Val_CF Map_CFPreferences::Get(CFStringRef iName) const
	{
	if (ZQ<Val_CF> theQ = this->QGet(iName))
		return *theQ;
	return Val_CF();
	}

Map_CFPreferences& Map_CFPreferences::Set(const string8& iName, const Val_CF& iVal)
	{ return this->Set(Util_CF::sString(iName), iVal); }

Map_CFPreferences& Map_CFPreferences::Set(CFStringRef iName, const Val_CF& iVal)
	{
	::CFPreferencesSetAppValue(iName, iVal, fApplicationID);
	return *this;
	}

Map_CFPreferences& Map_CFPreferences::Erase(const string8& iName)
	{ return this->Erase(Util_CF::sString(iName)); }

Map_CFPreferences& Map_CFPreferences::Erase(CFStringRef iName)
	{
	::CFPreferencesSetAppValue(iName, nullptr, fApplicationID);
	return *this;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
