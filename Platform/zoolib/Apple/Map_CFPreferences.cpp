// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

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
	if (ZP<CFPropertyListRef> theVal = sAdopt& ::CFPreferencesCopyAppValue(iName, fApplicationID))
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
