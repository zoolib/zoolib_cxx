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

#ifndef __ZNetscape_VariantPriv_h__
#define __ZNetscape_VariantPriv_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape_Variant.h"

#include <cstring>

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPVariant_T

template <class T>
ZAny NPVariant_T<T>::AsAny() const
	{ return this->AsAny(ZAny()); }

template <class T>
ZAny NPVariant_T<T>::AsAny(const ZAny& iDefault) const
	{
	switch (type)
		{
		case NPVariantType_Void:
		case NPVariantType_Null:
			{
			return null;
			}
		case NPVariantType_Bool:
			{
			return ZAny(bool(value.boolValue));
			}
		case NPVariantType_Int32:
			{
			return ZAny(int32(value.intValue));
			}
		case NPVariantType_Double:
			{
			return ZAny(value.doubleValue);
			}
		case NPVariantType_String:
			{
			return ZAny(std::string(
				sNPStringCharsConst(value.stringValue),
				sNPStringLengthConst(value.stringValue)));
			}
		case NPVariantType_Object:
			{
			return ZAny(ZRef<T>(static_cast<T*>(value.objectValue)));
			}
		}
	return iDefault;
	}

template <class T>
void NPVariant_T<T>::pCopyFrom(const NPVariant& iOther)
	{
	switch (iOther.type)
		{
		case NPVariantType_Void:
		case NPVariantType_Null:
			break;
		case NPVariantType_Bool:
			{
			value.boolValue = iOther.value.boolValue;
			break;
			}
		case NPVariantType_Int32:
			{
			value.intValue = iOther.value.intValue;
			break;
			}
		case NPVariantType_Double:
			{
			value.doubleValue = iOther.value.doubleValue;
			break;
			}
		case NPVariantType_String:
			{
			this->pSetString(
				sNPStringCharsConst(iOther.value.stringValue),
				sNPStringLengthConst(iOther.value.stringValue));
			break;
			}
		case NPVariantType_Object:
			{
			value.objectValue = iOther.value.objectValue;
			static_cast<T*>(value.objectValue)->Retain();
			break;
			}
		}
	type = iOther.type;
	}

template <class T>
void NPVariant_T<T>::pRelease()
	{ sVariantRelease_T(*this); }

template <class T>
NPVariant_T<T>::NPVariant_T()
	{ type = NPVariantType_Void; }

template <class T>
NPVariant_T<T>::NPVariant_T(const NPVariant_T& iOther)
	{
	ZAssert(this != &iOther);
	this->pCopyFrom(iOther);
	}

template <class T>
NPVariant_T<T>::~NPVariant_T()
	{ this->pRelease(); }

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(const NPVariant_T& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopyFrom(iOther);
		}
	return *this;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(const NPVariant& iOther)
	{
	ZAssert(this != &iOther);
	this->pCopyFrom(iOther);
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(const NPVariant& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopyFrom(iOther);
		}
	return *this;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(bool iValue)
	{
	type = NPVariantType_Bool;
	value.boolValue = iValue;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(int32 iValue)
	{
	type = NPVariantType_Int32;
	value.intValue = iValue;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(double iValue)
	{
	type = NPVariantType_Double;
	value.doubleValue = iValue;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(const char* iValue)
	{
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(const std::string& iValue)
	{
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

template <class T>
NPVariant_T<T>::NPVariant_T(T* iValue)
	{
	type = NPVariantType_Void;
	this->SetObject(iValue);
	}

template <class T>
NPVariant_T<T>::NPVariant_T(const ZRef<T>& iValue)
	{
	type = NPVariantType_Void;
	this->SetObject(iValue);
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(bool iValue)
	{
	this->SetBool(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(int32 iValue)
	{
	this->SetInt32(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(double iValue)
	{
	this->SetDouble(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(const char* iValue)
	{
	this->SetString(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(const std::string& iValue)
	{
	this->SetString(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(T* iValue)
	{
	this->SetObject(iValue);
	return *this;
	}

template <class T>
NPVariant_T<T>& NPVariant_T<T>::operator=(const ZRef<T>& iValue)
	{
	this->SetObject(iValue.Get());
	return *this;
	}

template <class T>
NPVariant_T<T>::operator ZRef<T>() const
	{ return ZRef<T>(this->GetObject()); }

template <class T>
bool NPVariant_T<T>::IsVoid() const
	{ return type == NPVariantType_Void; }

template <class T>
bool NPVariant_T<T>::IsNull() const
	{ return type == NPVariantType_Null; }

template <class T>
bool NPVariant_T<T>::IsBool() const
	{ return type == NPVariantType_Bool; }

template <class T>
bool NPVariant_T<T>::IsInt32() const
	{ return type == NPVariantType_Int32; }

template <class T>
bool NPVariant_T<T>::IsDouble() const
	{ return type == NPVariantType_Double; }

template <class T>
bool NPVariant_T<T>::IsString() const
	{ return type == NPVariantType_String; }

template <class T>
bool NPVariant_T<T>::IsObject() const
	{ return type == NPVariantType_Object; }

template <class T>
void NPVariant_T<T>::SetVoid()
	{
	this->pRelease();
	type = NPVariantType_Void;
	}

template <class T>
void NPVariant_T<T>::SetNull()
	{
	this->pRelease();
	type = NPVariantType_Null;
	}

template <class T>
void NPVariant_T<T>::SetBool(bool iValue)
	{
	this->pRelease();
	value.boolValue = iValue;
	type = NPVariantType_Bool;
	}

template <class T>
void NPVariant_T<T>::SetInt32(int32 iValue)
	{
	this->pRelease();
	value.intValue = iValue;
	type = NPVariantType_Int32;
	}

template <class T>
void NPVariant_T<T>::SetDouble(double iValue)
	{
	this->pRelease();
	value.doubleValue = iValue;
	type = NPVariantType_Double;
	}

template <class T>
void NPVariant_T<T>::SetString(const std::string& iValue)
	{
	this->pRelease();
	this->pSetString(iValue);
	type = NPVariantType_String;
	}

template <class T>
void NPVariant_T<T>::SetObject(T* iValue)
	{
	iValue->Retain();
	this->pRelease();
	value.objectValue = iValue;
	type = NPVariantType_Object;
	}

template <class T>
void NPVariant_T<T>::SetObject(const ZRef<T>& iValue)
	{ this->SetObject(iValue.Get()); }

template <class T>
void NPVariant_T<T>::pSetString(const char* iChars, size_t iLength)
	{
	sNPStringLength(value.stringValue) = iLength;
	char* p = static_cast<char*>(sMalloc_T(*this, iLength + 1));
	std::strncpy(p, iChars, iLength);
	sNPStringChars(value.stringValue) = p;
	}

template <class T>
void NPVariant_T<T>::pSetString(const std::string& iString)
	{
	if (size_t theLength = iString.length())
		this->pSetString(iString.data(), theLength);
	else
		this->pSetString(nullptr, 0);
	}

// =================================================================================================
// MARK: - NPVariant_T<T> typename accessors

ZMACRO_ZValAccessors_Def_GetX(template <class T>, NPVariant_T<T>, Bool, bool)
ZMACRO_ZValAccessors_Def_GetX(template <class T>, NPVariant_T<T>, Int32, int32)
ZMACRO_ZValAccessors_Def_GetX(template <class T>, NPVariant_T<T>, Double, double)
ZMACRO_ZValAccessors_Def_GetX(template <class T>, NPVariant_T<T>, String, std::string)
ZMACRO_ZValAccessors_Def_GetX(template <class T>, NPVariant_T<T>, Object, ZRef<T>)

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_VariantPriv_h__
