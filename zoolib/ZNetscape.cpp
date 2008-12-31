/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZNetscape.h"

#if ZCONFIG_SPI_Enabled(Netscape)

#include "zoolib/ZString.h"

using std::string;
using std::strncpy;

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

#define CASE(a) case a: return #a

std::string sAsString(NPNVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPNVxDisplay);
		CASE(NPNVxtAppContext);
		CASE(NPNVnetscapeWindow);
		CASE(NPNVjavascriptEnabledBool);
		CASE(NPNVasdEnabledBool);
		CASE(NPNVisOfflineBool);
		CASE(NPNVserviceManager);
		CASE(NPNVDOMElement);
		CASE(NPNVDOMWindow);
		CASE(NPNVToolkit);
		CASE(NPNVSupportsXEmbedBool);
		CASE(NPNVWindowNPObject);
		CASE(NPNVPluginElementNPObject);
		// Need to find a cleaner way to handle this portably.
		case 1000: return "NPNVpluginDrawingModel";
		case 2000: return "NPNVsupportsQuickDrawBool";
		case 2001: return "NPNVsupportsCoreGraphicsBool";
		}
	return ZString::sFormat("NPNVariable=%d", iVar);
	}

std::string sAsString(NPPVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPPVpluginNameString);
		CASE(NPPVpluginDescriptionString);
		CASE(NPPVpluginWindowBool);
		CASE(NPPVpluginTransparentBool);
		CASE(NPPVjavaClass);
		CASE(NPPVpluginWindowSize);
		CASE(NPPVpluginTimerInterval);
		CASE(NPPVpluginScriptableInstance);
		CASE(NPPVpluginScriptableIID);
		CASE(NPPVjavascriptPushCallerBool);
		CASE(NPPVpluginKeepLibraryInMemory);
		CASE(NPPVpluginNeedsXEmbed);
		CASE(NPPVpluginScriptableNPObject);
		}
	return ZString::sFormat("NPPVariable=%d", iVar);
	}

#undef CASE

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantBase

NPVariantBase::NPVariantBase()
	{
	type = NPVariantType_Void;
	}

NPVariantBase::NPVariantBase(bool iValue)
	{
	type = NPVariantType_Bool;
	value.boolValue = iValue;
	}

NPVariantBase::NPVariantBase(int32 iValue)
	{
	type = NPVariantType_Int32;
	value.intValue = iValue;
	}

NPVariantBase::NPVariantBase(double iValue)
	{
	type = NPVariantType_Double;
	value.doubleValue = iValue;
	}

NPVariantBase::NPVariantBase(const string& iValue)
	{
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

bool NPVariantBase::IsVoid() const
	{ return type == NPVariantType_Void; }

bool NPVariantBase::IsNull() const
	{ return type == NPVariantType_Null; }

bool NPVariantBase::IsBool() const
	{ return type == NPVariantType_Bool; }

bool NPVariantBase::IsInt32() const
	{ return type == NPVariantType_Int32; }

bool NPVariantBase::IsDouble() const
	{ return type == NPVariantType_Double; }

bool NPVariantBase::IsString() const
	{ return type == NPVariantType_String; }

bool NPVariantBase::IsObject() const
	{ return type == NPVariantType_Object; }

bool NPVariantBase::GetBool() const
	{ return this->DGetBool(false); }

bool NPVariantBase::GetBool(bool& oValue) const
	{
	if (type != NPVariantType_Bool)
		return false;
	oValue = value.boolValue;
	return true;
	}

bool NPVariantBase::DGetBool(bool iDefault) const
	{
	if (type == NPVariantType_Bool)
		return value.boolValue;
	return iDefault;
	}

int32 NPVariantBase::GetInt32() const
	{ return this->DGetInt32(0); }

bool NPVariantBase::GetInt32(int32& oValue) const
	{
	if (type != NPVariantType_Int32)
		return false;
	oValue = value.intValue;
	return true;
	}

int32 NPVariantBase::DGetInt32(int32 iDefault) const
	{
	if (type == NPVariantType_Int32)
		return value.intValue;
	return iDefault;
	}

double NPVariantBase::GetDouble() const
	{ return this->DGetDouble(0); }
	
bool NPVariantBase::GetDouble(double& oValue) const
	{
	if (type != NPVariantType_Double)
		return false;
	oValue = value.doubleValue;
	return true;
	}

double NPVariantBase::DGetDouble(double iDefault) const
	{
	if (type == NPVariantType_Double)
		return value.doubleValue;
	return iDefault;
	}

string NPVariantBase::GetString() const
	{ return this->DGetString(string()); }

bool NPVariantBase::GetString(string& oValue) const
	{
	if (type != NPVariantType_String)
		return false;
	oValue =
		string(sNPStringCharsConst(value.stringValue), sNPStringLengthConst(value.stringValue));
	return true;
	}

string NPVariantBase::DGetString(const string& iDefault) const
	{
	if (type != NPVariantType_String)
		return iDefault;
	return string(sNPStringCharsConst(value.stringValue), sNPStringLengthConst(value.stringValue));
	}

void NPVariantBase::pSetString(const char* iChars, size_t iLength)
	{
	sNPStringLength(value.stringValue) = iLength;
	sNPStringChars(value.stringValue) = static_cast<char*>(malloc(iLength));
	strncpy(sNPStringChars(value.stringValue), iChars, iLength);
	}

void NPVariantBase::pSetString(const std::string& iString)
	{
	if (size_t theLength = iString.length())
		this->pSetString(iString.data(), theLength);
	else
		this->pSetString(nil, 0);
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)
