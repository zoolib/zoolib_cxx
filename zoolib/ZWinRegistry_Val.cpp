/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZWinRegistry_Val.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

namespace ZWinRegistry {

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry

static bool spCount(const ZRef<HKEY>& iHKEY,
	DWORD* oCountKeys, DWORD* oMaxLengthKeyName,
	DWORD* oCountValues, DWORD* oMaxLengthValueName)
	{
	if (iHKEY)
		{
		if (ERROR_SUCCESS == ::RegQueryInfoKeyW(
			iHKEY,
			nullptr, // lpClass
			nullptr, // lpcClass
			nullptr, // lpReserved,
			oCountKeys,
			oMaxLengthKeyName,
			nullptr, // lpcMaxClassLen,
			oCountValues,
			oMaxLengthValueName,
			nullptr, // lpcMaxValueLen,
			nullptr, // lpcbSecurityDescriptor,
			nullptr // lpftLastWriteTime
			))
			{
			if (oMaxLengthKeyName)
				++*oMaxLengthKeyName;
			if (oMaxLengthValueName)
				++*oMaxLengthValueName;
			return true;
			}
		}
	return false;
	}

static string16 spReadLimited(const UTF16* iSource, size_t iLimit)
	{
	string16 result;
	while (iLimit--)
		{
		if (UTF32 theCP = ZUnicode::sReadInc(iSource))
			result += theCP;
		else
			break;
		}
	return result;
	}

static ZQ_T<Val> spQGet(DWORD iType, const void* iBuffer, DWORD iLength)
	{
	switch (iType)
		{
		case REG_NONE:
			{
			return Val();
			}
		case REG_BINARY:
			{
			return Data(iBuffer, iLength);
			}
		case REG_DWORD_LITTLE_ENDIAN:
			{
			if (iLength == 4)
				return int32(ZByteSwap_ReadLittle32(iBuffer));
			break;
			}
		case REG_DWORD_BIG_ENDIAN:
			{
			if (iLength == 4)
				return int32(ZByteSwap_ReadBig32(iBuffer));
			break;
			}
		case REG_QWORD_LITTLE_ENDIAN:
			{
			if (iLength == 8)
				return int64(ZByteSwap_ReadLittle64(iBuffer));
			break;
			}
		case REG_EXPAND_SZ:
		case REG_LINK:
		case REG_SZ:
			{
			if (0 == (iLength & 1))
				{
				const string16 asString16 =
					spReadLimited(static_cast<const UTF16*>(iBuffer), iLength / 2);

				if (REG_EXPAND_SZ == iType)
					{
					return String_Env(asString16);
					}
				else if (REG_LINK == iType)
					{
					return String_Link(asString16);
					}
				else
					{
					return asString16;
					}
				}
			break;
			}
		case REG_MULTI_SZ:
			{
			vector<string16> theVec;
			for (const WCHAR* cur = static_cast<const WCHAR*>(iBuffer);
				*cur; /*no inc*/)
				{
				const string16 curString(cur);
				theVec.push_back(curString);
				cur += curString.length() + 1;
				}

			return theVec;
			}
		}
	return null;
	}

static string8 spKeyName(const ZRef<HKEY>& iHKEY, DWORD iMaxLengthKeyName, size_t iIndex)
	{
	vector<WCHAR> theName(iMaxLengthKeyName);
	if (ERROR_SUCCESS == ::RegEnumKeyExW(
		iHKEY,
		iIndex,
		&theName[0],
		&iMaxLengthKeyName,
		nullptr, // lpReserved
		nullptr, // lpClass
		nullptr, // lpcClass
		nullptr // lpftLastWriteTime
		))
		{
		return ZUnicode::sAsUTF8(spReadLimited(&theName[0], iMaxLengthKeyName));
		}
	return string8();
	}

static string8 spValueName(const ZRef<HKEY>& iHKEY, DWORD iMaxLengthValueName, size_t iIndex)
	{
	vector<WCHAR> theName(iMaxLengthValueName);
	if (ERROR_SUCCESS == ::RegEnumValueW(
		iHKEY,
		iIndex,
		&theName[0],
		&iMaxLengthValueName,
		nullptr, // lpReserved
		nullptr, // lpType,
		nullptr, // lpData,
		nullptr // lpcbData
		))
		{
		return ZUnicode::sAsUTF8(spReadLimited(&theName[0], iMaxLengthValueName));
		}
	return string8();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry::Val

/**
\class ZWinRegistry::Val
\ingroup ZVal

\brief A ZVal-compatible entity for use with KeyRef
*/

ZAny Val::AsAny() const
	{ return *this; }

Val::Val()
	{}

Val::Val(const Val& iOther)
:	inherited(static_cast<const ZAny&>(iOther))
	{}

Val::~Val()
	{}

Val& Val::operator=(const Val& iOther)
	{
	inherited::operator=(static_cast<const ZAny&>(iOther));
	return *this;
	}

Val::Val(const string8& iVal)
:	inherited(ZUnicode::sAsUTF16(iVal))
	{}

Val::Val(const string16& iVal)
:	inherited(iVal)
	{}

Val::Val(const vector<string16>& iVal)
:	inherited(iVal)
	{}

Val::Val(const String_Env& iVal)
:	inherited(iVal)
	{}

Val::Val(const String_Link& iVal)
:	inherited(iVal)
	{}

Val::Val(const int32& iVal)
:	inherited(iVal)
	{}

Val::Val(const int64& iVal)
:	inherited(iVal)
	{}

Val::Val(const Data& iVal)
:	inherited(iVal)
	{}

Val::Val(const KeyRef& iVal)
:	inherited(iVal)
	{}

ZMACRO_ZValAccessors_Def_GetP(,Val, String16, string16)
ZMACRO_ZValAccessors_Def_GetP(,Val, StringList, vector<string16>)
ZMACRO_ZValAccessors_Def_GetP(,Val, String_Env, String_Env)
ZMACRO_ZValAccessors_Def_GetP(,Val, String_Link, String_Link)
ZMACRO_ZValAccessors_Def_GetP(,Val, Int32, int32)
ZMACRO_ZValAccessors_Def_GetP(,Val, Int64, int64)
ZMACRO_ZValAccessors_Def_GetP(,Val, Data, Data)
ZMACRO_ZValAccessors_Def_GetP(,Val, KeyRef, KeyRef)

// =================================================================================================
#pragma mark -
#pragma mark * ZWinRegistry::KeyRef

/**
\class ZWinRegistry::KeyRef
\ingroup ZVal

\brief Compatible with ZMap & HKEY
*/

KeyRef KeyRef::sHKCR()
	{ return KeyRef(HKEY_CLASSES_ROOT); }

KeyRef KeyRef::sHKCU()
	{ return KeyRef(HKEY_CURRENT_USER); }

KeyRef KeyRef::sHKLM()
	{ return KeyRef(HKEY_LOCAL_MACHINE); }

KeyRef KeyRef::sHKU()
	{ return KeyRef(HKEY_USERS); }

//KeyRef::operator operator_bool_type() const
//	{ return operator_bool_generator_type::translate(fHKEY); }

void KeyRef::swap(KeyRef& rhs)
	{ inherited::swap(rhs); }

KeyRef::KeyRef()
	{}

KeyRef::KeyRef(const KeyRef& iOther)
:	inherited(iOther)
	{}

KeyRef::~KeyRef()
	{}

KeyRef& KeyRef::operator=(const KeyRef& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

KeyRef::KeyRef(const ZRef<HKEY>& iOther)
:	inherited(iOther)
	{}

KeyRef& KeyRef::operator=(const ZRef<HKEY>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZQ_T<Val> KeyRef::QGet(const string16& iName) const
	{
	if (iName.empty())
		return null;

	if (iName[0] != '!')
		{
		KeyRef subKey;
		if (ERROR_SUCCESS == ::RegOpenKeyExW(
			*this,
			iName.c_str(),
			0, // ulOptions
			KEY_READ,
			&subKey.OParam()))
			{
			return subKey;
			}
		return null;
		}

	DWORD curLength = 1024;
	for (;;)
		{
		DWORD type;
		vector<BYTE> bufferVec(curLength);
		DWORD length = curLength;
		LONG result = ::RegQueryValueExW(
			*this,
			&iName.c_str()[1],
			nullptr, // lpReserved
			&type,
			&bufferVec[0],
			&length);

		if (ERROR_SUCCESS == result)
			return spQGet(type, &bufferVec[0], length);

		if (ERROR_MORE_DATA == result)
			{
			// If we're hitting HKEY_PERFORMANCE_DATA then the value of length is not
			// useful, the next call may need more space. So we just double curLength.
			curLength *= 2;
			continue;
			}

		return null;
		}
	}

ZQ_T<Val> KeyRef::QGet(const string8& iName) const
	{ return this->QGet(ZUnicode::sAsUTF16(iName)); }

ZQ_T<Val> KeyRef::QGet(const Index_t& iIndex) const
	{
	DWORD countKeys, maxLengthKeyName, countValues, maxLengthValueName;
	if (!spCount(*this, &countKeys, &maxLengthKeyName, &countValues, &maxLengthValueName))
		return null;

	size_t offset = iIndex;
	if (offset < countKeys)
		{
		vector<WCHAR> theName(maxLengthKeyName);
		if (ERROR_SUCCESS == ::RegEnumKeyExW(
			*this,
			offset,
			&theName[0],
			&maxLengthKeyName,
			nullptr, // lpReserved
			nullptr, // lpClass
			nullptr, // lpcClass
			nullptr // lpftLastWriteTime
			))
			{
			KeyRef subKey;
			if (ERROR_SUCCESS == ::RegOpenKeyExW(
				*this,
				&theName[0],
				0, // ulOptions
				KEY_READ,
				&subKey.OParam()))
				{
				return subKey;
				}
			}
		return null;
		}

	offset -= countKeys;
	if (offset >= countValues)
		return null;

	vector<WCHAR> theName(maxLengthValueName);
	DWORD curLength = 1024;
	for (;;)
		{
		DWORD nameLength = theName.size();
		DWORD type;
		vector<BYTE> bufferVec(curLength);
		DWORD length = curLength;
		LONG result = ::RegEnumValueW(
			*this,
			offset,
			&theName[0], // lpValueName
			&nameLength, // lpcbValueName
			nullptr, // lpReserved
			&type,
			&bufferVec[0],
			&length);

		if (ERROR_SUCCESS == result)
			return spQGet(type, &bufferVec[0], length);

		if (ERROR_MORE_DATA == result)
			{
			// See comment in other QGet method.
			curLength *= 2;
			continue;
			}

		return null;
		}
	}

Val KeyRef::DGet(const Val& iDefault, const string16& iName) const
	{
	if (ZQ_T<Val> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

Val KeyRef::DGet(const Val& iDefault, const string8& iName) const
	{
	if (ZQ_T<Val> theQ = this->QGet(ZUnicode::sAsUTF16(iName)))
		return theQ.Get();
	return iDefault;
	}

Val KeyRef::DGet(const Val& iDefault, const Index_t& iIndex) const
	{
	if (ZQ_T<Val> theQ = this->QGet(iIndex))
		return theQ.Get();
	return iDefault;
	}

Val KeyRef::Get(const string16& iName) const
	{ return this->DGet(Val(), iName); }

Val KeyRef::Get(const string8& iName) const
	{ return this->DGet(Val(), ZUnicode::sAsUTF16(iName)); }

Val KeyRef::Get(const Index_t& iIndex) const
	{ return this->DGet(Val(), iIndex); }

//void KeyRef::Set(const string16& iName, const Val& iVal);
//void KeyRef::Set(const string8& iName, const Val& iVal);
//void KeyRef::Set(const Index_t& iIndex, const Val& iVal);

//void KeyRef::Erase(const string16& iName);
//void KeyRef::Erase(const string8& iName);
//void KeyRef::Erase(const Index_t& iIndex);

KeyRef::Index_t KeyRef::Begin() const
	{ return 0; }

KeyRef::Index_t KeyRef::End() const
	{
	DWORD countKeys, countValues;
	if (spCount(*this, &countKeys, nullptr, &countValues, nullptr))
		return countKeys + countValues;
	return 0;
	}

string8 KeyRef::NameOf(const Index_t& iIndex) const
	{
	DWORD countKeys, maxLengthKeyName, countValues, maxLengthValueName;
	if (spCount(*this, &countKeys, &maxLengthKeyName, &countValues, &maxLengthValueName))
		{
		size_t offset = iIndex;
		if (offset < countKeys)
			return spKeyName(*this, maxLengthKeyName, offset);

		offset -= countKeys;
		if (offset < countValues)
			return "!" + spValueName(*this, maxLengthValueName, offset);
		}
	return string8();
	}

KeyRef::Index_t KeyRef::IndexOf(const string8& iName) const
	{
	const Index_t end = this->End();
	for (Index_t x = this->Begin(); x != end; ++x)
		{
		if (iName == this->NameOf(x))
			return x;
		}
	return end;
	}

KeyRef::Index_t KeyRef::IndexOf(const KeyRef& iOther, const Index_t& iOtherIndex) const
	{ return iOtherIndex; }

HKEY& KeyRef::OParam()
	{
	inherited::Clear();
	return this->GetPtrRef();
	}

} // namespace ZWinRegistry

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
